/*! \file
    \brief Interface for obtaining assets
*/

#pragma once


#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>

//
#include "i_assets_manager.h"

//
#include "umba/filename.h"
#include "umba/string_plus.h"

//
#include "nlohmann/json.hpp"
#include "marty_yaml_toml_json/json_utils.h"
#include "marty_yaml_toml_json/yaml_json.h"
#include "marty_cpp/marty_cpp_exceptions.h"

//
#include "defs.h"

//
#include "marty_virtual_fs/i_app_paths.h"
#include "marty_virtual_fs/i_app_paths_common.h"
#include "marty_virtual_fs/app_paths_impl.h"
#include "marty_virtual_fs/i_filesystem.h"
#include "marty_virtual_fs/i_virtual_fs.h"

//
#include "umba/simple_formatter.h"



namespace marty_assets_manager {


// Тут у нас нет понятия текущий каталог, путь начинается с корня - '/', если корень отсутствует явно, то считается, что все равно путь начинается с корня
struct AssetsManager : public IAssetsManager
{

protected:

    std::shared_ptr<marty_virtual_fs::IFileSystem> m_pFs        ;
    std::wstring                                   m_projectName;



    nlohmann::json readGenericJsonFromUtfString( const std::string &jsonStr
                                               //, const std::string &fileName
                                               ) const
    {
     
        std::string errMsg;
        std::string tmpJson;
        marty::json_utils::FileFormat detectedFormat = marty::json_utils::FileFormat::unknown;
        //nlohmann::json j = marty::json_utils::parseJsonOrYaml( "null" );
        nlohmann::json j = marty::json_utils::parseJsonOrYaml( jsonStr, true /* allowComments */ , &errMsg, &tmpJson, &detectedFormat );
     
        if (detectedFormat==marty::json_utils::FileFormat::unknown)
        {
            std::ostringstream oss;
            oss <<  /* encodingApi.encode(fileName) << ":  */ "error: " << errMsg << "\n";
            if (!tmpJson.empty())
            {
                oss << "JSON:" << "\n";
                oss << tmpJson << "\n";
            }
     
            throw std::runtime_error(oss.str());
        }
     
        return j;
    }


    template<typename StringType>
    StringType filenameFromText(const std::string &str) const
    {
        if constexpr (sizeof(typename StringType::value_type)>1)
        {
            return m_pFs->decodeText(str);
        }
        else
        {
            return str;
        }
    }

    template<typename StringType>
    ErrorCode readNutProjectImpl( const StringType               &fileName
                                , NutProjectT<StringType>        &prj
                                , std::unordered_set<StringType> &loadedProjects
                                , std::unordered_set<StringType> &loadedNuts
                                ) const
    {
        static const StringType nutsJsonExt = umba::string_plus::make_string<StringType>(".NUTS.JSON"); // project suffix (double ext)
        static const StringType jsonExt     = umba::string_plus::make_string<StringType>(".JSON"); // project suffix
        static const StringType nutExt      = umba::string_plus::make_string<StringType>(".NUT") ; // single file suffix

        if (!m_pFs->isFileExistAndReadable(fileName))
        {
            return ErrorCode::notFound;
        }


        StringType fileNameUpper = umba::string_plus::toupper_copy(fileName);

        // Тут варианты такие
        // Любые .json файлы рассматривать как файл проекта
        // или именно .nuts.json?
        // У последнего - красивое имя, но .nuts.lib.json - тоже неплохое расширение для либ, например

        // В то же время readNutProjectCompleteImpl сама проверяет наличие тех или иных файлов, и изначально она сама
        // пробует загрузить либо .nuts.json, либо просто .nut

        // Наверное, лучше ограничится проверкой расширения на .json, или, скорее, на .nut - это точно отдельный файл

        //

        bool isProjectFile = true;

        #if 0
        if (loadedProjects.empty())
        {
            // это первый файл, он должен быть строго .NUTS.JSON
            if (!umba::string_plus::ends_with(fileNameUpper, nutsJsonExt))
            {
                isProjectFile = false;
            }
        }
        #endif

        if (umba::string_plus::ends_with(fileNameUpper, nutExt))
        {
            isProjectFile = false;
        }

        
        //if (!umba::string_plus::ends_with(fileNameUpper, nutsJsonExt))
        if (!isProjectFile)
        {
            // Читаем проект из единственного .nut файла

            if (loadedNuts.find(fileNameUpper)!=loadedNuts.end())
            {
                return ErrorCode::ok; // уже есть такой
            }
 
            loadedNuts.insert(fileNameUpper);

            prj.projectFileName = fileName ;
            prj.nuts.emplace_back(fileName);

            return ErrorCode::ok;
        }
        else
        {
            // read JSON project  here

            StringType filePath = m_pFs->getPath(fileName);

            try
            {
                std::string nutsJsonPrjText;
                ErrorCode err = m_pFs->readTextFile(fileName, nutsJsonPrjText);
                if (err!=ErrorCode::ok)
                {
                    return err;
                }

                //StringType fileNameUpper = umba::string_plus::toupper_copy(fileName);
                loadedProjects.insert(fileNameUpper);

                auto jConf = readGenericJsonFromUtfString(nutsJsonPrjText);

                auto filesNodeIter = jConf.find("files");
                if (filesNodeIter!=jConf.end())
                {
                    auto &jFiles = filesNodeIter.value();
                    if (!jFiles.is_array())
                    {
                        umba::lout << ".nuts.json: 'files' node must be an array\n";
                        return ErrorCode::invalidFormat;
                    }

                    for (nlohmann::json::iterator jFileIt=jFiles.begin(); jFileIt!=jFiles.end(); ++jFileIt)
                    {
                        auto &jFileNode = jFileIt.value();

                        if (jFileNode.is_object()) 
                        {
                            // разбор импорта
                            auto includeNodeIter = jFileNode.find("include");
                            if (includeNodeIter==jFileNode.end())
                            {
                               // continue; // Это объект, но там нет include - хз, что это, может ошибка 
                               return ErrorCode::invalidFormat; // сообщим, что формат некорректный
                            }

                            auto &jIcludeNode    = includeNodeIter.value();

                            // Импорт может быть как для одного элемента, и тогда нет массива, или для списка элементов
                            // Но мы всегда сначала собираем всё это в вектор
                            std::vector<StringType> includeProjectFiles;

                            if (!jIcludeNode.is_array())
                            {
                                auto str = jIcludeNode.get<std::string>();
                                includeProjectFiles.emplace_back(filenameFromText<StringType>(str));
                            }
                            else
                            {
                                for (nlohmann::json::iterator jIncFileIt=jIcludeNode.begin(); jIncFileIt!=jIcludeNode.end(); ++jIncFileIt)
                                {
                                    //auto &jIncFileNode = jIncFileIt.value();
                                    auto str = jIncFileIt->get<std::string>();
                                    includeProjectFiles.emplace_back(filenameFromText<StringType>(str));
                                }
                            }

                            for(const auto& incPrjFile : includeProjectFiles)
                            {
                                auto incPrjFullName = m_pFs->normalizeFilename(m_pFs->appendPath(filePath, incPrjFile));
                                StringType incPrjFullNameUpper = umba::string_plus::toupper_copy(incPrjFullName);

                                if (loadedProjects.find(incPrjFullNameUpper)!=loadedProjects.end())
                                {
                                    continue;
                                }

                                loadedProjects.insert(incPrjFullNameUpper);

                                NutProjectT<StringType> incPrj;
                                ErrorCode err2 = readNutProjectImpl(incPrjFullName, incPrj, loadedProjects, loadedNuts);
                                if (err2!=ErrorCode::ok)
                                {
                                    return err2;
                                }

                                prj.nuts.insert(prj.nuts.end(), incPrj.nuts.begin(), incPrj.nuts.end());

                            }

                        }
                        else // single file
                        {
                            auto str             = jFileIt->get<std::string>();
                            auto nutFile         = m_pFs->normalizeFilename(m_pFs->appendPath(filePath, filenameFromText<StringType>(str)));
                            auto nutFileUpper    = umba::string_plus::toupper_copy(nutFile);

                            if (loadedNuts.find(nutFileUpper)!=loadedNuts.end())
                            {
                                continue;
                            }
                             
                            loadedNuts.insert(nutFileUpper);

                            prj.nuts.emplace_back(nutFile);
                            
                            // if constexpr (sizeof(typename StringType::value_type)>1)
                            // {
                            //     prj.nuts.emplace_back(m_pFs->normalizeFilename(m_pFs->appendPath(filePath, m_pFs->decodeText(str))));
                            // }
                            // else
                            // {
                            //     prj.nuts.emplace_back(m_pFs->normalizeFilename(m_pFs->appendPath(filePath, str)));
                            // }
                           
                            if (!m_pFs->isFileExistAndReadable(prj.nuts.back()))
                            {
                                umba::lout << "Missing file '" << m_pFs->encodeFilename(prj.nuts.back()) << "\n";
                                return ErrorCode::missingFiles;
                            }
                        }

                    }
                }

                prj.projectFileName = fileName;
                
            }
            catch(const std::exception &e)
            {
                // Заллогировать
                // e.what()
                //MARTY_ASSMAN_ARG_USED(e);
                umba::lout << "Failed to read nut project '" << m_pFs->encodeFilename(fileName) << "': " << e.what() << "\n";

                return ErrorCode::unknownFormat;
                // ErrorCode::invalidFormat;
            }
        
            return ErrorCode::ok;
        }

    }

    template<typename StringType>
    ErrorCode readNutProjectFilesImpl(NutProjectT<StringType> &prj) const
    {
        for(auto nutFile : prj.nuts)
        {
            StringType fileText;
            ErrorCode err = m_pFs->readTextFile(nutFile, fileText);
            if (err!=ErrorCode::ok)
            {
                return err; // По идее, этого не должно происходить, файлы на доступность для чтения уже проверены
            }

            prj.nutsData.emplace_back(fileText);
        }

        return ErrorCode::ok;

    }

    template<typename StringType>
    ErrorCode readNutProjectCompleteImpl(NutProjectT<StringType> &prj) const
    {
        StringType projectName;
        ErrorCode err = getProjectName(projectName);
        if (err!=ErrorCode::ok)
        {
            return err;
        }

        StringType fullNameBase    = m_pFs->appendPath(umba::string_plus::make_string<StringType>("/nuts"), projectName);
        
        StringType fullNameNutJson = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nuts.json"));
        StringType fullNameNut     = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nut"));

        std::unordered_set<StringType> loadedProjects;
        std::unordered_set<StringType> loadedNuts    ;
        err = readNutProjectImpl(fullNameNutJson, prj, loadedProjects, loadedNuts);
        if ( err==ErrorCode::missingFiles  // не все файлы в проекте реально существуют
          || err==ErrorCode::invalidFormat // проект есть, но ошибка формата/синтаксиса файла JSON/YAML
          || err==ErrorCode::unknownFormat // проект есть, но ошибка формата, не JSON/YAML
           )
        {
            // Файл проекта есть, но там ошибка - отдельный nut файл не пытаемся читать - у нас есть файл проекта, просто там косяк
            return err;
        }

        if (err!=ErrorCode::ok)
        {
            err = readNutProjectImpl(fullNameNut, prj, loadedProjects, loadedNuts);
        }

        if (err!=ErrorCode::ok)
        {
            return err;
        }

        return readNutProjectFilesImpl(prj);

    }



#if 0

enum class ErrorCode : std::uint32_t
{
    invalid              = (std::uint32_t)(-1),
    unknown              = (std::uint32_t)(-1),
    ok                   = 0x00,
    genericError         = 0x01,
    notFound             = 0x02,
    notExist             = 0x03,
    alreadyExist         = 0x04,
    accessDenied         = 0x05,
    invalidName          = 0x06,
    notSupported         = 0x07,
    invalidMountPoint    = 0x08,
    invalidMountTarget   = 0x09,
    notDirectory         = 0x0A,
    invalidFormat        = 0x0B,
    unknownFormat        = 0x0C,
    missingFiles         = 0x0D

}; // enum class ErrorCode : std::uint32_t

#endif

public:

    AssetsManager(std::shared_ptr<marty_virtual_fs::IFileSystem> pFs)
    : m_pFs(pFs)
    {}

    
    
    virtual ErrorCode setProjectName(const std::string  &projectName) override
    {
        m_projectName = m_pFs->decodeFilename(projectName);
        return ErrorCode::ok;
    }

    virtual ErrorCode setProjectName(const std::wstring &projectName) override
    {
        m_projectName = projectName;
        return ErrorCode::ok;
    }


    virtual ErrorCode getProjectName(std::string  &projectName) const override
    {
        if (m_projectName.empty())
        {
            projectName = "nutApplication";
        }
        else
        {
            projectName = m_pFs->encodeFilename(m_projectName);
        }

        return ErrorCode::ok;
    }

    virtual ErrorCode getProjectName(std::wstring &projectName) const override
    {
        if (m_projectName.empty())
        {
            projectName = L"nutApplication";
        }
        else
        {
            projectName = m_projectName;
        }

        return ErrorCode::ok;
    }


    virtual ErrorCode readNutProject(const std::string  &fileName, NutProjectA &prj) const override
    {
        std::unordered_set<std::string> loadedProjects;
        std::unordered_set<std::string> loadedNuts;
        prj.clear();
        return readNutProjectImpl(fileName, prj, loadedProjects, loadedNuts);
    }

    virtual ErrorCode readNutProject(const std::wstring &fileName, NutProjectW &prj) const override
    {
        std::unordered_set<std::wstring> loadedProjects;
        std::unordered_set<std::wstring> loadedNuts;
        prj.clear();
        return readNutProjectImpl(fileName, prj, loadedProjects, loadedNuts);
    }


    virtual ErrorCode readNutProjectFiles(NutProjectA &prj) const override
    {
        return readNutProjectFilesImpl(prj);
    }

    virtual ErrorCode readNutProjectFiles(NutProjectW &prj) const override
    {
        return readNutProjectFilesImpl(prj);
    }

    // Чтение проекта (из одного nut-файла или из файла проекта), а также всех nut-файлов
    virtual ErrorCode readNutProjectComplete(NutProjectA &prj) const override
    {
        return readNutProjectCompleteImpl(prj);
    }

    virtual ErrorCode readNutProjectComplete(NutProjectW &prj) const override
    {
        return readNutProjectCompleteImpl(prj);
    }





}; // struct IAssetsManager



inline
std::shared_ptr<IAssetsManager> makeAssetsManager( std::shared_ptr<marty_virtual_fs::IFileSystem> pFileSystem
                                                 )
{
    auto pAppPaths = std::make_shared<marty_virtual_fs::AppPathsImpl>();
    std::wstring appName;
    pAppPaths->getAppName(appName);

    auto pAssetsManager = std::make_shared<marty_assets_manager::AssetsManager>(pFileSystem);
    pAssetsManager->setProjectName(appName);

    return pAssetsManager;
}





} // namespace marty_assets_manager


// #include "warnings_restore.h"


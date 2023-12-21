/*! \file
    \brief Interface for obtaining assets
*/

#pragma once


#include <string>
#include <vector>
#include <sstream>

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
    ErrorCode readNutProjectImpl(const StringType &fileName, NutProjectT<StringType> &prj) const
    {
        static const StringType nutsJsonExt = umba::string_plus::make_string<StringType>(".NUTS.JSON"); // project suffix (double ext)

        if (!m_pFs->isFileExistAndReadable(fileName))
        {
            return ErrorCode::notFound;
        }


        prj.clear();

        StringType fileNameUpper = umba::string_plus::toupper_copy(fileName);
        if (!umba::string_plus::ends_with(fileNameUpper, nutsJsonExt))
        {
            // Читаем проект из единственного .nut файла

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

                auto jConf = readGenericJsonFromUtfString(nutsJsonPrjText);

                auto filesNodeIter = jConf.find("files");
                if (filesNodeIter!=jConf.end())
                {
                    auto &jFiles = filesNodeIter.value();

                    for (nlohmann::json::iterator jFileIt=jFiles.begin(); jFileIt!=jFiles.end(); ++jFileIt)
                    {
                        auto str = jFileIt->get<std::string>();
                        if constexpr (sizeof(typename StringType::value_type)>1)
                        {
                            prj.nuts.emplace_back(m_pFs->normalizeFilename(m_pFs->appendPath(filePath, m_pFs->decodeText(str))));
                        }
                        else
                        {
                            prj.nuts.emplace_back(m_pFs->normalizeFilename(m_pFs->appendPath(filePath, str)));
                        }

                        if (!m_pFs->isFileExistAndReadable(prj.nuts.back()))
                        {
                            return ErrorCode::missingFiles;
                        }
                    }
                }

                prj.projectFileName = fileName;
                
            }
            catch(const std::exception &e)
            {
                // Заллогировать
                // e.what()
                MARTY_ASSMAN_ARG_USED(e);

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


        err = readNutProjectImpl(fullNameNutJson, prj);
        if ( err==ErrorCode::missingFiles  // не все файлы в проекте реально существуют
          || err==ErrorCode::unknownFormat // проект есть, но ошибка формата
           )
        {
            // Файл проекта есть, но там ошибка
            return err;
        }

        if (err!=ErrorCode::ok)
        {
            err = readNutProjectImpl(fullNameNut, prj);
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
        return readNutProjectImpl(fileName, prj);
    }

    virtual ErrorCode readNutProject(const std::wstring &fileName, NutProjectW &prj) const override
    {
        return readNutProjectImpl(fileName, prj);
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


} // namespace marty_assets_manager


// #include "warnings_restore.h"


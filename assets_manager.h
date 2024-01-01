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
#include "marty_simplesquirrel/json.h"

//
#include "umba/env.h"
#include "umba/simple_formatter.h"
#include "umba/gmesg.h"



namespace marty_assets_manager {


// Тут у нас нет понятия текущий каталог, путь начинается с корня - '/', если корень отсутствует явно, то считается, что все равно путь начинается с корня
struct AssetsManager : public IAssetsManager
{

protected:

    std::shared_ptr<marty_virtual_fs::IFileSystem> m_pFs        ;
    std::wstring                                   m_projectName;


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
    StringType decodeText(const std::string &str) const
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
    std::string encodeText(const StringType &str) const
    {
        if constexpr (sizeof(typename StringType::value_type)>1)
        {
            return m_pFs->encodeText(str);
        }
        else
        {
            return str;
        }
    }


    nlohmann::json readGenericJsonFromUtfString( const std::string &jsonStr
                                               , const std::string &fileName
                                               ) const
    {
        return marty_simplesquirrel::json_helpers::readGenericJsonFromUtfString(jsonStr, fileName);
    }

    nlohmann::json readGenericJsonFromUtfString( const std::string &jsonStr
                                               , const std::wstring &fileName
                                               ) const
    {
        return marty_simplesquirrel::json_helpers::readGenericJsonFromUtfString(jsonStr, m_pFs->encodeFilename(fileName));
    }


    template<typename StringType>
    NutType detectFileNutTypeImpl(const StringType &fname) const
    {
        StringType fnameUpper = umba::string_plus::toupper_copy(fname);


        static const StringType nutExt           = umba::string_plus::make_string<StringType>(".NUT") ; // single file suffix

        if (umba::string_plus::ends_with(fnameUpper, nutExt))
        {
            return NutType::nutFile;
        }


        static const StringType nutsJsnProjExt1  = umba::string_plus::make_string<StringType>(".NUTSJSNPROJ"); // project suffix
        static const StringType nutsJsnProjExt2  = umba::string_plus::make_string<StringType>(".NUTJSNPROJ"); // project suffix
        static const StringType nutsJsonExt      = umba::string_plus::make_string<StringType>(".NUTS.JSON"); // project suffix (double ext)

        static const StringType nutsYmlProjExt1  = umba::string_plus::make_string<StringType>(".NUTSYMLPROJ"); // project suffix
        static const StringType nutsYmlProjExt2  = umba::string_plus::make_string<StringType>(".NUTYMLPROJ"); // project suffix
        static const StringType nutsYamlExt      = umba::string_plus::make_string<StringType>(".NUTS.YAML"); // project suffix (double ext)

        if ( umba::string_plus::ends_with(fnameUpper, nutsJsnProjExt1)
          || umba::string_plus::ends_with(fnameUpper, nutsJsnProjExt2)
          || umba::string_plus::ends_with(fnameUpper, nutsJsonExt)
          || umba::string_plus::ends_with(fnameUpper, nutsYmlProjExt1)
          || umba::string_plus::ends_with(fnameUpper, nutsYmlProjExt2)
          || umba::string_plus::ends_with(fnameUpper, nutsYamlExt)
           )
        {
            return NutType::dotNutProject;
        }


        static const StringType appSelectorExtJ  = umba::string_plus::make_string<StringType>("DOTNUT.APP-SELECTOR.MANIFEST.JSON");
        static const StringType appSelectorExtY  = umba::string_plus::make_string<StringType>("DOTNUT.APP-SELECTOR.MANIFEST.YAML");

        if ( umba::string_plus::ends_with(fnameUpper, appSelectorExtJ)
          || umba::string_plus::ends_with(fnameUpper, appSelectorExtY)
           )
        {
            return NutType::dotNutAppSelector;
        }


        static const StringType manifestExt1     = umba::string_plus::make_string<StringType>(".DOTNUT-MANIFEST.JSON");
        static const StringType manifestExt2     = umba::string_plus::make_string<StringType>(".MANIFEST.JSON");
        static const StringType manifestExt3     = umba::string_plus::make_string<StringType>(".DOTNUT-MANIFEST.YAML");
        static const StringType manifestExt4     = umba::string_plus::make_string<StringType>(".MANIFEST.YAML");

        if ( umba::string_plus::ends_with(fnameUpper, manifestExt1)
          || umba::string_plus::ends_with(fnameUpper, manifestExt2)
          || umba::string_plus::ends_with(fnameUpper, manifestExt3)
          || umba::string_plus::ends_with(fnameUpper, manifestExt4)
           )
        {
            return NutType::dotNutManifect;
        }

        static const StringType jsonExt          = umba::string_plus::make_string<StringType>(".JSON"); // project suffix
        static const StringType yamlExt          = umba::string_plus::make_string<StringType>(".YAML"); // project suffix

        if ( umba::string_plus::ends_with(fnameUpper, jsonExt)
          || umba::string_plus::ends_with(fnameUpper, yamlExt)
           )
        {
            return NutType::dotNutProject;
        }

        return NutType::unknownNutType;

    }

    template<typename StringType>
    ErrorCode readNutProjectImpl( const StringType               &fileName
                                , NutProjectT<StringType>        &prj
                                , std::unordered_set<StringType> &loadedProjects
                                , std::unordered_set<StringType> &loadedNuts
                                ) const
    {
        if (!m_pFs->isFileExistAndReadable(fileName))
        {
            return ErrorCode::notFound;
        }


        StringType fileNameUpper = umba::string_plus::toupper_copy(fileName);

        bool isProjectFile = true;

        NutType nutType = detectFileNutTypeImpl(fileName);
        if (nutType!=NutType::dotNutProject)
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

                auto jConf = readGenericJsonFromUtfString(nutsJsonPrjText, fileName);

                auto filesNodeIter = jConf.find("files");
                if (filesNodeIter!=jConf.end())
                {
                    auto &jFiles = filesNodeIter.value();
                    if (!jFiles.is_array())
                    {
                        umba::lout << ".nuts.json: 'files' node must be an array\n";
                        umba::gmesg(fileName, ".nuts.json: 'files' node must be an array");
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
                            
                            if (!m_pFs->isFileExistAndReadable(prj.nuts.back()))
                            {
                                //umba::lout << "Missing file '" << m_pFs->encodeFilename(prj.nuts.back()) << "\n";
                                umba::lout << "Missing file '" << m_pFs->encodeText(prj.nuts.back()) << "'\n";
                                umba::gmesg("Missing file '" + m_pFs->encodeText(prj.nuts.back()) + "'");
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
                //umba::lout << "Failed to read nut project '" << m_pFs->encodeFilename(fileName) << "': " << e.what() << "\n";
                umba::lout << "Failed to read nut project '" << m_pFs->encodeText(fileName) << "': " << e.what() << "\n";
                umba::lout.flush();
                umba::gmesg(fileName, std::string("Failed to read nut project: ") + e.what());

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
    ErrorCode readAppSelectorManifestImpl(NutAppSelectorManifestT<StringType> &appSel) const
    {
        static StringType nutAppSelectorManifest = umba::string_plus::make_string<StringType>("dotnut.app-selector.manifest.json");

        if (!m_pFs->isFileExistAndReadable(nutAppSelectorManifest))
        {
            return ErrorCode::notFound;
        }

        std::string nutsJsonPrjText;
        ErrorCode err = m_pFs->readTextFile(nutAppSelectorManifest, nutsJsonPrjText);
        if (err!=ErrorCode::ok)
        {
            return err;
        }

        try
        {
            auto jConf = readGenericJsonFromUtfString(nutsJsonPrjText, nutAppSelectorManifest);
    
            auto appListNodeIter = jConf.find("app-list");
            if (appListNodeIter==jConf.end())
            {
                umba::gmesg("dotnut.app-selector.manifest.json", "no 'app-list' node found");
                return ErrorCode::invalidFormat;
                //return ErrorCode::notFound;
            }
    
            auto &jAppListNode = appListNodeIter.value();
            if (!jAppListNode.is_array())
            {
                umba::gmesg("dotnut.app-selector.manifest.json", "'app-list' node is not an array");
                return ErrorCode::invalidFormat;
            }
    
            appSel.manifestItems.clear();
    
            for (nlohmann::json::iterator jAppListItemsIt=jAppListNode.begin(); jAppListItemsIt!=jAppListNode.end(); ++jAppListItemsIt)
            {
                // title/app-name
                auto jAppNameIter = jAppListItemsIt->find("app-name");
                if (jAppNameIter==jAppListItemsIt->end())
                {
                    continue;
                }
    
                try
                {
                    std::string strAppName           = jAppNameIter->get<std::string>();
                    StringType  strAppNameStringType = filenameFromText<StringType>(strAppName);
                    if (strAppNameStringType.empty())
                    {
                        continue;
                    }
    
                    std::string strAppTitle;
                    auto jAppTitleIter = jAppListItemsIt->find("title");
                    if (jAppTitleIter!=jAppListItemsIt->end())
                    {
                        strAppTitle = jAppTitleIter->get<std::string>();
                    }
    
                    StringType strAppTitleStringType = filenameFromText<StringType>(strAppTitle);
                    if (strAppTitleStringType.empty())
                    {
                        strAppTitleStringType = strAppNameStringType;
                    }
    
                    appSel.manifestItems.emplace_back(NutAppSelectorManifestItemT<StringType>{strAppTitleStringType, strAppNameStringType});
                }
                catch(const std::exception &e)
                {
                    MARTY_ASSMAN_ARG_USED(e);
                }
                catch(...)
                {
                }
                
            }
    
        }
        catch(const std::exception &e)
        {
            umba::lout << "Failed to read 'dotnut.app-selector.manifest.json': " << e.what() << "\n";
            umba::lout.flush();
            //umba::gmesg("dotnut.app-selector.manifest.json", e.what());
            umba::gmesg(e.what());
            return ErrorCode::invalidFormat;
        }
        catch(...)
        {
            umba::lout << "Failed to read 'dotnut.app-selector.manifest.json': " << "unknown error" << "\n";
            umba::lout.flush();
            umba::gmesg("dotnut.app-selector.manifest.json", "unknown error");
            //umba::gmesg(e.what());
            return ErrorCode::invalidFormat;
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

        StringType fullNameBase     = m_pFs->appendPath(umba::string_plus::make_string<StringType>("/nuts"), projectName);
        
        std::vector<StringType> projectFileNames;
        projectFileNames.emplace_back(m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nuts.json"  )));
        projectFileNames.emplace_back(m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nuts.jsn"   )));
        projectFileNames.emplace_back(m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nutsjsnproj")));
        projectFileNames.emplace_back(m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nutjsnproj" )));
        projectFileNames.emplace_back(m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nuts.yaml"  )));
        projectFileNames.emplace_back(m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nuts.yml"   )));
        projectFileNames.emplace_back(m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nutsymlproj")));
        projectFileNames.emplace_back(m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nutymlproj" )));
        projectFileNames.emplace_back(m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nut"        )));

        //StringType fullNameNut      = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nut"        ));

        // // .nutsjsnproj .nutjsnproj .nuts.json
        // StringType fullNameNutJson1 = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nutsjsnproj"));
        // StringType fullNameNutJson2 = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nutjsnproj" ));
        // StringType fullNameNutJson3 = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nuts.json"  ));
        // StringType fullNameNutJson4 = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nuts.jsn"   ));
        // StringType fullNameNutYaml1 = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nutsymlproj"));
        // StringType fullNameNutYaml2 = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nutymlproj" ));
        // StringType fullNameNutYaml3 = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nuts.yaml"  ));
        // StringType fullNameNutYaml4 = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nuts.yml"   ));
        // StringType fullNameNut      = m_pFs->appendExt(fullNameBase, umba::string_plus::make_string<StringType>("nut"        ));

        std::unordered_set<StringType> loadedProjects;
        std::unordered_set<StringType> loadedNuts    ;

        for(const auto &prjFileName : projectFileNames)
        {
            err = readNutProjectImpl(prjFileName, prj, loadedProjects, loadedNuts);
            if (err==ErrorCode::ok)
            {
                break;
            }

            if ( err==ErrorCode::missingFiles  // не все файлы в проекте реально существуют
              || err==ErrorCode::invalidFormat // проект есть, но ошибка формата/синтаксиса файла JSON/YAML
              || err==ErrorCode::unknownFormat // проект есть, но ошибка формата, не JSON/YAML
               )
            {
                // Файл проекта есть, но там ошибка - отдельный nut файл не пытаемся читать - у нас есть файл проекта, просто там косяк
                return err;
            }

        }

        // if (err!=ErrorCode::ok)
        // {
        //     err = readNutProjectImpl(fullNameNut, prj, loadedProjects, loadedNuts);
        // }

        if (err!=ErrorCode::ok)
        {
            return err;
        }

        return readNutProjectFilesImpl(prj);

    }

    // void updateNutManifestGraphics(nlohmann::json j)
    // nlohmann::json

    

    // nlohmann::json::iterator findJsonAnyChild(nlohmann::json &j, const std::string &n1) const
    // {
    //     return j.find(n1.c_str());
    // }
    //  
    // nlohmann::json::iterator findJsonAnyChild(nlohmann::json &j, const std::string &n1, const std::string &n2) const
    // {
    //     auto iter = j.find(n1.c_str());
    //     if (iter==j.end())
    //         iter = j.find(n2.c_str());
    //     return iter;
    // }
    
    bool getEnvironmentVariable(const std::string &name, std::string &val) const
    {
        return umba::env::getVar(name, val);
    }

    bool getAllEnvironmentVariables(std::vector<std::pair<std::string,std::string> > &lst) const
    {
        return umba::env::getEnvVarsList(lst);
    }

    bool getEnvironmentVariable(const std::wstring &name, std::wstring &val) const
    {
        #if defined(WIN32) || defined(_WIN32)

            return umba::env::getVar(name, val);

        #else

            std::string strVal;
            if (!umba::env::getVar(encodeText(name), strVal))
            {
                return false;
            }

            val = decodeText<std::wstring>(strVal);

            return true;

        #endif
        
    }

    bool getAllEnvironmentVariables(std::vector<std::pair<std::wstring,std::wstring> > &lst) const
    {
        #if defined(WIN32) || defined(_WIN32)

            return umba::env::getEnvVarsList(lst);

        #else

            std::vector<std::pair<std::string,std::string> > strLst;
            if (!umba::env::getEnvVarsList(strLst))
            {
                return false;
            }

            lst.clear();

            for(const auto &p: strLst)
            {
                lst.emplace_back(std::make_pair(decodeText<std::wstring>(p.first),decodeText<std::wstring>(p.second)))
            }

            return true;

        #endif
        
    }


    template<typename StringType>
    ErrorCode updateNutManifestImpl(const StringType &fileName, NutManifestT<StringType> &manifest) const
    {
        using marty_simplesquirrel::json_helpers::findJsonAnyChild;

        //MARTY_ASSMAN_ARG_USED(fileName);
        MARTY_ASSMAN_ARG_USED(manifest);

        std::string maifestText;
        ErrorCode err = m_pFs->readTextFile(fileName, maifestText);
        if (err!=ErrorCode::ok)
        {
            return err;
        }

        try
        {
            manifest.manifestFileName = fileName;

            auto jManifest = readGenericJsonFromUtfString(maifestText, fileName);

            auto 
            // jiter = jManifest.find("appGroup");
            jiter = findJsonAnyChild(jManifest, "appGroup", "app-group");
            if (jiter!=jManifest.end())
            {
                std::string strAppGroup = jiter->get<std::string>();
                manifest.appGroup = filenameFromText<StringType>(strAppGroup);
            }

            // jiter = jManifest.find("graphicsMode");
            jiter = findJsonAnyChild(jManifest, "graphicsMode", "graphics-mode");
            if (jiter!=jManifest.end())
            {
                std::string strMode = jiter->get<std::string>();
                auto newVal = enum_deserialize_NutManifestGraphicsMode(strMode, NutManifestGraphicsMode::invalid);
                if (newVal!=NutManifestGraphicsMode::invalid)
                {
                    manifest.manifestGraphicsMode = newVal;
                }
            }

            //jiter = jManifest.find("window");
            jiter = findJsonAnyChild(jManifest, "window");
            if (jiter!=jManifest.end())
            {
                if (!jiter->is_object())
                {
                    throw std::runtime_error("'window' node is not an object");
                }

                auto jWindow = jiter.value();
                //jiter = jWindow.find("title");
                jiter = findJsonAnyChild(jWindow, "title");
                if (jiter!=jWindow.end())
                {
                    std::string str = jiter->get<std::string>();
                    manifest.window.title = decodeText<StringType>(str);
                }
                
            }

            //jiter = jManifest.find("hotkeys");
            jiter = findJsonAnyChild(jManifest, "hotkeys");
            if (jiter!=jManifest.end())
            {
                if (!jiter->is_object())
                {
                    throw std::runtime_error("'hotkeys' node is not an object");
                }

                auto jHotkeys = jiter.value();
                
                //jiter = jHotkeys.find("allowReloadScript");
                jiter = findJsonAnyChild(jHotkeys, "allowReloadScript", "allow-reload-script");
                if (jiter!=jHotkeys.end())
                {
                    manifest.hotkeysManifest.allowReloadScript = jiter->get<bool>();
                }

                //jiter = jHotkeys.find("allowFullscreen");
                jiter = findJsonAnyChild(jHotkeys, "allowFullscreen", "allow-fullscreen");
                if (jiter!=jHotkeys.end())
                {
                    manifest.hotkeysManifest.allowFullscreen = jiter->get<bool>();
                }
                
            }

            //jiter = jManifest.find("startup");
            jiter = findJsonAnyChild(jManifest, "startup");
            if (jiter!=jManifest.end())
            {
                if (!jiter->is_object())
                {
                    throw std::runtime_error("'startup' node is not an object");
                }

                auto jStartup = jiter.value();
                //jiter = jStartup.find("runFullscreen");
                jiter = findJsonAnyChild(jStartup, "runFullscreen", "run-fullscreen");
                if (jiter!=jStartup.end())
                {
                    manifest.startupManifest.runFullscreen = jiter->get<bool>();
                }
                
            }

    // bool getAllEnvironmentVariables(std::vector<std::pair<std::wstring,std::wstring> > &lst) const
    // bool getEnvironmentVariable(const std::string &name, std::string &val) const

            jiter = findJsonAnyChild(jManifest, "importEnvironmentVariables", "import-environment-variables");
            if (jiter!=jManifest.end())
            {
                if (jiter->is_boolean())
                {
                    std::vector<std::pair<StringType,StringType> > lst;
                    getAllEnvironmentVariables(lst);
                    for(const auto &p: lst)
                    {
                        manifest.envVars[p.first] = p.second;
                    }
                }
                else if (jiter->is_array())
                {
                    auto jClearVariables = jiter.value();
                    for (nlohmann::json::iterator jClrVarIt=jClearVariables.begin(); jClrVarIt!=jClearVariables.end(); ++jClrVarIt)
                    {
                        auto &jClrVar = jClrVarIt.value();
                        auto strVal = jClrVar.get<std::string>();
                        auto varName = decodeText<StringType>(strVal);
                        StringType val;
                        if (getEnvironmentVariable(varName, val))
                        {
                            manifest.envVars[varName] = val;
                        }
                    }
                }
                else
                {
                    throw std::runtime_error("'clearVariables' node is not an array nor boolean");
                }
            }


            jiter = findJsonAnyChild(jManifest, "clearVariables", "clear-variables");
            if (jiter!=jManifest.end())
            {
                if (jiter->is_boolean())
                {
                    manifest.envVars.clear();
                }
                else if (jiter->is_array())
                {
                    auto jClearVariables = jiter.value();
                    for (nlohmann::json::iterator jClrVarIt=jClearVariables.begin(); jClrVarIt!=jClearVariables.end(); ++jClrVarIt)
                    {
                        auto &jClrVar = jClrVarIt.value();
                        auto strVal = jClrVar.get<std::string>();
                        manifest.envVars.erase(decodeText<StringType>(strVal));
                    }
                }
                else
                {
                    throw std::runtime_error("'clearVariables' node is not an array nor boolean");
                }
            }


            jiter = jManifest.find("variables");
            if (jiter!=jManifest.end())
            {
                if (!jiter->is_object())
                {
                    throw std::runtime_error("'variables' node is not an object");
                }

                auto jVariables = jiter.value();
                for (auto& el : jVariables.items())
                {
                    std::string strKey = el.key();
                    std::string strVal = el.value();

                    manifest.envVars[decodeText<StringType>(strKey)] = decodeText<StringType>(strVal);
                }

            }

            //std::unordered_map<StringType, StringType>      envVars           ;

            //jiter = jManifest.find("filesystem");
            jiter = findJsonAnyChild(jManifest, "filesystem");
            if (jiter!=jManifest.end())
            {
                if (!jiter->is_object())
                {
                    throw std::runtime_error("'filesystem' node is not an object");
                }

                auto jFilesystem = jiter.value();

                //jiter = jFilesystem.find("mountLocalFilesystem");
                jiter = findJsonAnyChild(jFilesystem, "mountLocalFilesystem", "mount-local-filesystem");
                if (jiter!=jFilesystem.end())
                {
                    manifest.filesystemManifest.mountLocalFilesystem = jiter->get<bool>();
                }


                //jiter = jFilesystem.find("mountHome");
                jiter = findJsonAnyChild(jFilesystem, "mountHome", "mount-home");
                if (jiter!=jFilesystem.end())
                {
                    manifest.filesystemManifest.mountHome = jiter->get<bool>();
                }

                //jiter = jFilesystem.find("homeMountPointName");
                jiter = findJsonAnyChild(jFilesystem, "homeMountPointName", "home-mount-point-name");
                if (jiter!=jFilesystem.end())
                {
                    auto str = jiter->get<std::string>();
                    manifest.filesystemManifest.homeMountPointName = decodeText<StringType>(str);
                }

                //jiter = jFilesystem.find("homeMountTarget");
                jiter = findJsonAnyChild(jFilesystem, "homeMountTarget", "home-mount-target");
                if (jiter!=jFilesystem.end())
                {
                    auto str = jiter->get<std::string>();
                    manifest.filesystemManifest.homeMountTarget = decodeText<StringType>(str);
                }


                //jiter = jFilesystem.find("mountTemp");
                jiter = findJsonAnyChild(jFilesystem, "mountTemp", "mount-temp");
                if (jiter!=jFilesystem.end())
                {
                    manifest.filesystemManifest.mountTemp = jiter->get<bool>();
                }

                //jiter = jFilesystem.find("tempMountPointName");
                jiter = findJsonAnyChild(jFilesystem, "tempMountPointName", "temp-mount-point-name");
                if (jiter!=jFilesystem.end())
                {
                    auto str = jiter->get<std::string>();
                    manifest.filesystemManifest.tempMountPointName = decodeText<StringType>(str);
                }

                //jiter = jFilesystem.find("tempMountTarget");
                jiter = findJsonAnyChild(jFilesystem, "tempMountTarget", "temp-mount-target");
                if (jiter!=jFilesystem.end())
                {
                    auto str = jiter->get<std::string>();
                    manifest.filesystemManifest.tempMountTarget = decodeText<StringType>(str);
                }


                //jiter = jFilesystem.find("mountLogs");
                jiter = findJsonAnyChild(jFilesystem, "mountLogs", "mount-logs");
                if (jiter!=jFilesystem.end())
                {
                    manifest.filesystemManifest.mountLogs = jiter->get<bool>();
                }

                //jiter = jFilesystem.find("logsMountPointName");
                jiter = findJsonAnyChild(jFilesystem, "logsMountPointName", "logs-mount-point-name");
                if (jiter!=jFilesystem.end())
                {
                    auto str = jiter->get<std::string>();
                    manifest.filesystemManifest.logsMountPointName = decodeText<StringType>(str);
                }

                //jiter = jFilesystem.find("logsMountTarget");
                jiter = findJsonAnyChild(jFilesystem, "logsMountTarget", "logs-mount-target");
                if (jiter!=jFilesystem.end())
                {
                    auto str = jiter->get<std::string>();
                    manifest.filesystemManifest.logsMountTarget = decodeText<StringType>(str);
                }


                //jiter = jFilesystem.find("clearExistingMountPoints");
                jiter = findJsonAnyChild(jFilesystem, "clearExistingMountPoints", "clear-existing-mount-points");
                if (jiter!=jFilesystem.end())
                {
                    bool bClear = jiter->get<bool>();
                    if (bClear)
                    {
                        manifest.filesystemManifest.customMountPoints.clear();
                    }

                }

                //jiter = jFilesystem.find("mountPoints");
                jiter = findJsonAnyChild(jFilesystem, "mountPoints", "mount-points");
                if (jiter!=jFilesystem.end())
                {
                    //if (!jiter->is_object())
                    if (!jiter->is_array())
                    {
                        //throw std::runtime_error("'filesystem/mountPoints' node is not an object");
                        throw std::runtime_error("'filesystem/mountPoints' node is not an array");
                    }

                    auto jMountPoints = jiter.value();
                    for (nlohmann::json::iterator jMp=jMountPoints.begin(); jMp!=jMountPoints.end(); ++jMp)
                    {
                        NutFilesystemManifestMountPointInfoT<StringType> mpi;

                        jiter = jMp->find("name");
                        if (jiter==jMp->end())
                        {
                            throw std::runtime_error("'filesystem/mountPoints': mount point must contain 'name' string");
                        }
                        else
                        {
                            auto str = jiter->get<std::string>();
                            mpi.mountPointName = decodeText<StringType>(str);
                        }

                        jiter = jMp->find("target");
                        if (jiter==jMp->end())
                        {
                            throw std::runtime_error("'filesystem/mountPoints': mount point must contain 'target' string");
                        }
                        else
                        {
                            auto str = jiter->get<std::string>();
                            mpi.mountPointTargetPath = decodeText<StringType>(str);
                        }

                        manifest.filesystemManifest.customMountPoints.emplace_back(mpi);

                    }

                }

            // auto &jAppListNode = appListNodeIter.value();
            // if (!jAppListNode.is_array())
            // {
            //     umba::gmesg("dotnut.app-selector.manifest.json", "'app-list' node is not an array");
            //     return ErrorCode::invalidFormat;
            // }
            //  
            // appSel.manifestItems.clear();
            //  
            // for (nlohmann::json::iterator jAppListItemsIt=jAppListNode.begin(); jAppListItemsIt!=jAppListNode.end(); ++jAppListItemsIt)
            // {
            //     // title/app-name
            //     auto jAppNameIter = jAppListItemsIt->find("app-name");
            //     if (jAppNameIter==jAppListItemsIt->end())
            //     {
            //         continue;
            //     }
            //  
            //     try
            //     {
            //         std::string strAppName           = jAppNameIter->get<std::string>();
            //         StringType  strAppNameStringType = filenameFromText<StringType>(strAppName);
            //         if (strAppNameStringType.empty())
            //         {
            //             continue;
            //         }
            //  
            //         std::string strAppTitle;
            //         auto jAppTitleIter = jAppListItemsIt->find("title");
            //         if (jAppTitleIter!=jAppListItemsIt->end())
            //         {
            //             strAppTitle = jAppTitleIter->get<std::string>();
            //         }


                //manifest.filesystemManifest
                //NutFilesystemManifestT<StringType>   filesystemManifest;

                // template<typename StringType>
                // struct NutFilesystemManifestT
                // {
                //     bool       mountLocalFilesystem = true;
                //  
                //     bool       mountHome            = true;
                //     StringType homeMountPointName   = umba::string_plus::make_string<StringType>("~Home");
                //  
                //     bool       mountTemp            = true;
                //     StringType tempMountPointName   = umba::string_plus::make_string<StringType>("$Temp");
                //  
                //     bool       mountLogs            = true;
                //     StringType logsMountPointName   = umba::string_plus::make_string<StringType>(".Logs");
                //  
                //     std::vector< NutFilesystemManifestMountPointInfoT<StringType> > customMountPoints;
                //  
                // }; // struct NutFilesystemManifestT

                // template<typename StringType>
                // struct NutFilesystemManifestMountPointInfoT
                // {
                //     StringType    mountPointName;
                //     StringType    mountPointTargetPath;  // Мы сами можем определить, является ли таргет каталогом или файлом
                //  
                //     // static
                //     // NutFilesystemManifestMountPointInfoT<StringType> fromJsonNode(nlohmann::json &j)
                //     // {
                //     //     NutFilesystemManifestMountPointInfoT<StringType>
                //     // }
                // };


            }

#if 0
            ///
            auto appListNodeIter = jConf.find("app-list");
            if (appListNodeIter==jConf.end())
            {
                return ErrorCode::notFound;
            }
    
            auto &jAppListNode = appListNodeIter.value();
            if (!jAppListNode.is_array())
            {
                return ErrorCode::invalidFormat;
            }
    
            appSel.manifestItems.clear();
    
            for (nlohmann::json::iterator jAppListItemsIt=jAppListNode.begin(); jAppListItemsIt!=jAppListNode.end(); ++jAppListItemsIt)
            {
                // title/app-name
                auto jAppNameIter = jAppListItemsIt->find("app-name");
                if (jAppNameIter==jAppListItemsIt->end())
                {
                    continue;
                }
    
                try
                {
                    std::string strAppName           = jAppNameIter->get<std::string>();
                    StringType  strAppNameStringType = filenameFromText<StringType>(strAppName);
                    if (strAppNameStringType.empty())
                    {
                        continue;
                    }


            ///

#endif



        }
        catch(const std::exception &e)
        {
            umba::lout << "Failed to read manifest file: " << e.what() << "\n";
            umba::lout.flush();
            umba::gmesg(fileName, std::string("Failed to read manifest file: ") + e.what());
            return ErrorCode::invalidFormat;
        }
        catch(...)
        {
            umba::lout << "Failed to read manifest file: " << "unknown error" << "\n";
            umba::lout.flush();
            umba::gmesg(fileName, std::string("Failed to read manifest file: ") + "unknown error");
            return ErrorCode::invalidFormat;
        }

        return ErrorCode::ok;
        
    }

    template<typename StringType>
    ErrorCode updateNutManifestImpl(NutManifestT<StringType> &manifest) const
    {
        MARTY_ASSMAN_ARG_USED(manifest);

        StringType appName;
        ErrorCode err = getProjectName(appName);
        if (err!=ErrorCode::ok)
        {
            return err;
        }

        StringType appManifestBase = m_pFs->appendPath(umba::string_plus::make_string<StringType>("/manifests"), appName);

        StringType jsonName = m_pFs->appendExt(appManifestBase, umba::string_plus::make_string<StringType>("dotnut-manifest.json"));
        StringType yamlName = m_pFs->appendExt(appManifestBase, umba::string_plus::make_string<StringType>("dotnut-manifest.yaml"));

        err = updateNutManifestImpl(jsonName, manifest);
        if (err!=ErrorCode::ok)
        {
            err = updateNutManifestImpl(yamlName, manifest);
        }

        // manifests/
        // test_simplesquirrel03.dotnut-manifest.json
        //m_pFs

        return err;
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


    virtual NutType detectFileNutType(const std::string  &fname) const override
    {
        return detectFileNutTypeImpl(fname);
    }

    virtual NutType detectFileNutType(const std::wstring &fname) const override
    {
        return detectFileNutTypeImpl(fname);
    }
    
    
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

    virtual ErrorCode readAppSelectorManifest(NutAppSelectorManifestA &appSel) const override
    {
        return readAppSelectorManifestImpl(appSel);
    }

    virtual ErrorCode readAppSelectorManifest(NutAppSelectorManifestW &appSel) const override
    {
        return readAppSelectorManifestImpl(appSel);
    }

    virtual ErrorCode updateNutManifest(const std::string  &fileName, NutManifestA &manifest) const override
    {
        return updateNutManifestImpl(fileName, manifest);
    }

    virtual ErrorCode updateNutManifest(const std::wstring &fileName, NutManifestW &manifest) const override
    {
        return updateNutManifestImpl(fileName, manifest);
    }

    virtual ErrorCode updateNutManifest(NutManifestA &manifest) const override
    {
        return updateNutManifestImpl(manifest);
    }

    virtual ErrorCode updateNutManifest(NutManifestW &manifest) const override
    {
        return updateNutManifestImpl(manifest);
    }


protected:

    template<typename FileNameStringType, typename TextStringType>
    ErrorCode readConfTextFileImpl(const FileNameStringType &fName, TextStringType &fText) const
    {
        FileNameStringType fullConfFileName
            = m_pFs->appendPath( umba::string_plus::make_string<FileNameStringType>("/conf")
                               , fName
                               );

        return m_pFs->readTextFile(fullConfFileName, fText);
    }

    template<typename FileNameStringType>
    ErrorCode readConfDataFileImpl(const FileNameStringType &fName, std::vector<std::uint8_t> &fData) const
    {
        FileNameStringType fullConfFileName
            = m_pFs->appendPath( umba::string_plus::make_string<FileNameStringType>("/conf")
                               , fName
                               );

        return m_pFs->readDataFile(fullConfFileName, fData);
    }


public:

    // Тут автоматически работают перекодировки текста
    virtual ErrorCode readConfTextFile(const std::string  &fName, std::string  &fText) const override
    {
        return readConfTextFileImpl(fName, fText);
    }

    virtual ErrorCode readConfTextFile(const std::string  &fName, std::wstring &fText) const override
    {
        return readConfTextFileImpl(fName, fText);
    }

    virtual ErrorCode readConfTextFile(const std::wstring &fName, std::string  &fText) const override
    {
        return readConfTextFileImpl(fName, fText);
    }

    virtual ErrorCode readConfTextFile(const std::wstring &fName, std::wstring &fText) const override
    {
        return readConfTextFileImpl(fName, fText);
    }

     
    // Reading binary files
    virtual ErrorCode readConfDataFile(const std::string  &fName, std::vector<std::uint8_t> &fData) const override
    {
        return readConfDataFileImpl(fName, fData);
    }

    virtual ErrorCode readConfDataFile(const std::wstring &fName, std::vector<std::uint8_t> &fData) const override
    {
        return readConfDataFileImpl(fName, fData);
    }

    // Возвращает текстовую строку, соответствующую коду ошибки
    virtual bool getErrorCodeString(ErrorCode e, std::string  &errStr) const override
    {
        return m_pFs->getErrorCodeString(e, errStr);
    }

    virtual bool getErrorCodeString(ErrorCode e, std::wstring &errStr) const override
    {
        return m_pFs->getErrorCodeString(e, errStr);
    }


    //! Возвращает путь
    virtual std::string  getPath(const std::string  &fullName) const override
    {
        return m_pFs->getPath(fullName);
    }

    virtual std::wstring getPath(const std::wstring &fullName) const override
    {
        return m_pFs->getPath(fullName);
    }

    //! Возвращает имя и расширение
    virtual std::string  getFileName(const std::string  &fullName) const override
    {
        return m_pFs->getFileName(fullName);
    }

    virtual std::wstring getFileName(const std::wstring &fullName) const override
    {
        return m_pFs->getFileName(fullName);
    }


    //! Возвращает путь и имя
    virtual std::string  getPathFile(const std::string  &fullName) const override
    {
        return m_pFs->getPathFile(fullName);
    }

    virtual std::wstring getPathFile(const std::wstring &fullName) const override
    {
        return m_pFs->getPathFile(fullName);
    }


    //! Возвращает расширение
    virtual std::string  getExt(const std::string  &fullName) const override
    {
        return m_pFs->getExt(fullName);
    }

    virtual std::wstring getExt(const std::wstring &fullName) const override
    {
        return m_pFs->getExt(fullName);
    }


    //! Возвращает имя файла без пути и расширения
    virtual std::string  getName(const std::string  &fullName) const override
    {
        return m_pFs->getName(fullName);
    }

    virtual std::wstring getName(const std::wstring &fullName) const override
    {
        return m_pFs->getName(fullName);
    }


    //! Конкатенация путей
    virtual std::string  appendPath(const std::string  &pathAppendTo, const std::string  &appendPath) const override
    {
        return m_pFs->appendPath(pathAppendTo, appendPath);
    }

    virtual std::wstring appendPath(const std::wstring &pathAppendTo, const std::wstring &appendPath) const override
    {
        return m_pFs->appendPath(pathAppendTo, appendPath);
    }


    //! Добавление расширения
    virtual std::string  appendExt(const std::string  &nameAppendTo, const std::string  &appendExt) const override
    {
        return m_pFs->appendPath(nameAppendTo, appendExt);
    }

    virtual std::wstring appendExt(const std::wstring &nameAppendTo, const std::wstring &appendExt) const override
    {
        return m_pFs->appendPath(nameAppendTo, appendExt);
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




template<typename StringType, typename TargetGetter> inline
ErrorCode configureAppFileSystemByManifestMountAppFixedPointHelper
                                          ( std::shared_ptr<marty_virtual_fs::IVirtualFs> pVfs
                                          , const StringType                              &manifestFile
                                          , StringType                                    mntName    
                                          , StringType                                    mntNameDefault
                                          , StringType                                    mntTarget  // если не задано, то targetGetter
                                          , const std::string                             &msgMountFailed
                                          , TargetGetter                                  targetGetter
                                          , const std::string                             &msgGetTargetFailed
                                          )
{
    if (mntName.empty())
    {
        mntName = mntNameDefault;
    }

    if (mntTarget.empty())
    {
        if (!targetGetter(mntTarget))
        {
            umba::gmesg(manifestFile, msgGetTargetFailed);
            return ErrorCode::genericError;
        }
    }

    // mntName   - есть
    // mntTarget - есть

    ErrorCode err = pVfs->addMountPoint(mntName, mntTarget);
    if (err!=ErrorCode::ok)
    {
        umba::gmesg(manifestFile, msgMountFailed);
    }

    return err;
}

template<typename StringType> inline
ErrorCode configureAppFileSystemByManifest( std::shared_ptr<marty_virtual_fs::IVirtualFs> pVfs
                                          , std::shared_ptr<marty_virtual_fs::IAppPaths>  pAppPaths
                                          , const StringType                             &manifestFile
                                          , const NutFilesystemManifestT<StringType>     &fsManifest
                                          )
{
    ErrorCode err = ErrorCode::ok;


    if (fsManifest.mountLocalFilesystem)
    {
        err = pVfs->createMachineFilesystemMountPoints();
        if (err!=ErrorCode::ok)
        {
            umba::gmesg(manifestFile, "'filesystem/mountLocalFilesystem': Failed to mount local filesystem");
            return err;
        }
    }

    using umba::string_plus::make_string;


    if (fsManifest.mountHome)
    {
        err = configureAppFileSystemByManifestMountAppFixedPointHelper
                      ( pVfs, manifestFile
                      , fsManifest.homeMountPointName                       // mntName
                      , make_string<StringType>("~Home") // mntNameDefault
                      , fsManifest.homeMountTarget                          // mntTarget
                      , "'filesystem/mountHome': Failed to mount app home folder"
                      , [&](StringType &mntTarget) { return pAppPaths->getAppHomePathEx(mntTarget, true); }
                      , "'filesystem/mountHome': Failed to get app home folder"
                      );

        if (err!=ErrorCode::ok)
            return err;
    }


    if (fsManifest.mountTemp)
    {
        err = configureAppFileSystemByManifestMountAppFixedPointHelper
                      ( pVfs, manifestFile
                      , fsManifest.tempMountPointName                       // mntName
                      , make_string<StringType>("$Temp") // mntNameDefault
                      , fsManifest.tempMountTarget                          // mntTarget
                      , "'filesystem/mountTemp': Failed to mount app temp folder"
                      , [&](StringType &mntTarget) { return pAppPaths->getAppTempPathEx(mntTarget, true); }
                      , "'filesystem/mountTemp': Failed to get app temp folder"
                      );

        if (err!=ErrorCode::ok)
            return err;
    }


    if (fsManifest.mountLogs)
    {
        err = configureAppFileSystemByManifestMountAppFixedPointHelper
                      ( pVfs, manifestFile
                      , fsManifest.logsMountPointName                       // mntName
                      , make_string<StringType>(".Logs") // mntNameDefault
                      , fsManifest.logsMountTarget                          // mntTarget
                      , "'filesystem/mountLogs': Failed to mount app logs folder"
                      , [&](StringType &mntTarget) { return pAppPaths->getAppLogsPathEx(mntTarget, true); }
                      , "'filesystem/mountLogs': Failed to get app logs folder"
                      );

        if (err!=ErrorCode::ok)
            return err;
    }


    for(const auto &mpi: fsManifest.customMountPoints)
    {
        marty_virtual_fs::FileTypeFlags  fileTypeFlags = marty_virtual_fs::FileTypeFlags::normalFile;
        if (umba::filesys::isPathDirectory(mpi.mountPointTargetPath))
        {
            fileTypeFlags = fileTypeFlags | marty_virtual_fs::FileTypeFlags::directory;
        }

        err = pVfs->addMountPointEx(mpi.mountPointName, mpi.mountPointTargetPath, fileTypeFlags);
        if (err!=ErrorCode::ok)
        {
            umba::gmesg( manifestFile
                       , make_string<StringType>("'filesystem/mountPoints': Failed to mount '")
                       + mpi.mountPointName 
                       + make_string<StringType>("' to '")
                       + mpi.mountPointTargetPath
                       + make_string<StringType>("'")
                       );
            return err;
        }
        
    }

    return err;

}



} // namespace marty_assets_manager


// #include "warnings_restore.h"


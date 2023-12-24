/*! \file
    \brief Assets Manager types
*/

#pragma once


#include <string>
#include <vector>

//
#include "marty_virtual_fs/i_filesystem.h"
//
#include "enums.h"
//
#include "umba/string_plus.h"

//
#include "nlohmann/json.hpp"
#include "marty_yaml_toml_json/json_utils.h"
#include "marty_yaml_toml_json/yaml_json.h"
#include "marty_cpp/marty_cpp_exceptions.h"



//----------------------------------------------------------------------------

//
//#include "warnings_disable.h"

// #include "marty_simplesquirrel/simplesquirrel.h"

// Или пока это тут не нужно?


//----------------------------------------------------------------------------
namespace marty_assets_manager {



//----------------------------------------------------------------------------
typedef marty_virtual_fs::ErrorCode    ErrorCode;



//----------------------------------------------------------------------------
template<typename StringType>
struct NutProjectT
{
    StringType                 projectFileName;

    std::vector<StringType>    nuts           ; // nut filenames
    std::vector<StringType>    nutsData       ;

    void clear()
    {
        projectFileName.clear();
        nuts           .clear();
        nutsData       .clear();
    }

};

//------------------------------
typedef NutProjectT<std::string>     NutProjectA;
typedef NutProjectT<std::wstring>    NutProjectW;

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename StringType>
struct NutFilesystemManifestMountPointInfoT
{
    StringType    mountPointName;
    StringType    mountPointTargetPath;  // Мы сами можем определить, является ли таргет каталогом или файлом

    // static
    // NutFilesystemManifestMountPointInfoT<StringType> fromJsonNode(nlohmann::json &j)
    // {
    //     NutFilesystemManifestMountPointInfoT<StringType>
    // }
};


//----------------------------------------------------------------------------
template<typename StringType>
struct NutFilesystemManifestT
{
    bool       mountLocalFilesystem = true;

    bool       mountHome            = true;
    StringType homeMountPointName   = umba::string_plus::make_string<StringType>("~Home");

    bool       mountTemp            = true;
    StringType tempMountPointName   = umba::string_plus::make_string<StringType>("$Temp");

    bool       mountLogs            = true;
    StringType logsMountPointName   = umba::string_plus::make_string<StringType>(".Logs");

    std::vector< NutFilesystemManifestMountPointInfoT<StringType> > customMountPoints;

}; // struct NutFilesystemManifestT

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct NutStartupManifest
{
    bool runFullscreen = false;

}; // struct NutStartupManifest

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct NutHotkeysManifest
{
    bool allowReloadScript = true;
    bool allowFullscreen   = true;

}; // struct NutStartupManifest

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StringType>
struct NutManifestT
{
    // IAppPathsCommon::setAppCommonHomeSubPath - Глобально на всё приложение для всех экземпляров приложения
    StringType                           appGroup          ;
    NutManifestGraphicsMode              manifestGraphicsMode  = NutManifestGraphicsMode::prefferSpeed;

    NutHotkeysManifest                   hotkeysManifest   ;
    NutStartupManifest                   startupManifest   ;
    NutFilesystemManifestT<StringType>   filesystemManifest;

};

//------------------------------
typedef NutManifestT<std::string>        NutManifestA;
typedef NutManifestT<std::wstring>       NutManifestW;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StringType>
struct NutAppSelectorManifestItemT
{
    StringType    appTitle;
    StringType    appName ;

}; // struct NutAppSelectorManifestItemT

//------------------------------
typedef NutAppSelectorManifestItemT<std::string>     NutAppSelectorManifestItemA;
typedef NutAppSelectorManifestItemT<std::wstring>    NutAppSelectorManifestItemW;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StringType>
struct NutAppSelectorManifestT
{
    std::vector< NutAppSelectorManifestItemT<StringType> >  manifestItems;

}; // struct NutAppSelectorManifestT

//------------------------------
typedef NutAppSelectorManifestT<std::string>     NutAppSelectorManifestA;
typedef NutAppSelectorManifestT<std::wstring>    NutAppSelectorManifestW;

//----------------------------------------------------------------------------




} // namespace marty_assets_manager







// #include "warnings_restore.h"


/*! \file
    \brief Assets Manager types
*/

#pragma once


#include <string>
#include <vector>

//
#include "marty_virtual_fs/i_filesystem.h"


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
struct NutManifestT
{
    // Что тут нужно

    // F11
    // F5

    // Что и как монтировать


};






} // namespace marty_assets_manager







// #include "warnings_restore.h"


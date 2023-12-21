/*! \file
    \brief Assets Manager types
*/

#pragma once


#include <string>
#include <vector>

//
//#include "warnings_disable.h"

// #include "marty_simplesquirrel/simplesquirrel.h"

// Или пока это тут не нужно?

namespace marty_assets_manager {



typedef marty_virtual_fs::ErrorCode    ErrorCode;



// Нужен проект со списком файлов

template<typename StringType>
struct NutProjectT
{
    StringType                 projectFileName;

    std::vector<StringType>    nuts           ;
    std::vector<StringType>    nutsData       ;

    void clear()
    {
        projectFileName.clear();
        nuts           .clear();
        nutsData       .clear();
    }

};


typedef NutProjectT<std::string>     NutProjectA;
typedef NutProjectT<std::wstring>    NutProjectW;


} // namespace marty_assets_manager


// #include "warnings_restore.h"


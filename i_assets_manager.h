/*! \file
    \brief Interface for obtaining assets
*/

#pragma once


#include <string>
#include <vector>

//
#include "types.h"

//
//#include "warnings_disable.h"

//
#include "marty_virtual_fs/i_filesystem.h"


namespace marty_assets_manager {



struct IAssetsManager
{
    virtual ~IAssetsManager() {}

    virtual ErrorCode setProjectName(const std::string  &projectName) = 0;
    virtual ErrorCode setProjectName(const std::wstring &projectName) = 0;

    virtual ErrorCode getProjectName(std::string  &projectName) const = 0;
    virtual ErrorCode getProjectName(std::wstring &projectName) const = 0;

    // Чтение проекта (из одного nut-файла или из файла проекта)
    virtual ErrorCode readNutProject(const std::string  &fileName, NutProjectA &prj) const = 0;
    virtual ErrorCode readNutProject(const std::wstring &fileName, NutProjectW &prj) const = 0;

    // Чтение файлов, входящий в проект.
    virtual ErrorCode readNutProjectFiles(NutProjectA &prj) const = 0;
    virtual ErrorCode readNutProjectFiles(NutProjectW &prj) const = 0;

    // Чтение проекта (из одного nut-файла или из файла проекта), а также всех nut-файлов
    virtual ErrorCode readNutProjectComplete(NutProjectA &prj) const = 0;
    virtual ErrorCode readNutProjectComplete(NutProjectW &prj) const = 0;


    // Чтение manifest'а


}; // struct IAssetsManager


} // namespace marty_assets_manager


// #include "warnings_restore.h"


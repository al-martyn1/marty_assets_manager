/*! \file
    \brief Interface for obtaining assets
*/

#pragma once


#include <string>
#include <vector>

//
#include "nlohmann/json.hpp"
//
#include "types.h"

//
//#include "warnings_disable.h"

//
#include "marty_virtual_fs/i_filesystem.h"
//
#include "enums.h"


namespace marty_assets_manager {



struct IAssetsManager
{
    virtual ~IAssetsManager() {}

    virtual NutType detectFileNutType(const std::string  &fname) const = 0;
    virtual NutType detectFileNutType(const std::wstring &fname) const = 0;

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

    virtual ErrorCode readAppSelectorManifest(NutAppSelectorManifestA &appSel) const = 0;
    virtual ErrorCode readAppSelectorManifest(NutAppSelectorManifestW &appSel) const = 0;

    // Идея в том, что есть манифест по дефолту, и все файлы манифеста только обновляют уже существующий манифест
    // Это нужно, чтобы пользователь мог написать свой манифест и обновить/заменить прошитый в аппу
    virtual ErrorCode updateNutManifest(const std::string  &fileName, NutManifestA &manifest) const = 0;
    virtual ErrorCode updateNutManifest(const std::wstring &fileName, NutManifestW &manifest) const = 0;

    // Обновляем манифест из дефолтного месторасположения
    virtual ErrorCode updateNutManifest(NutManifestA &manifest) const = 0;
    virtual ErrorCode updateNutManifest(NutManifestW &manifest) const = 0;


    // Возвращает текстовую строку, соответствующую коду ошибки
    virtual bool getErrorCodeString(ErrorCode e, std::string  &errStr) const = 0; // static
    virtual bool getErrorCodeString(ErrorCode e, std::wstring &errStr) const = 0; // static

    //! Возвращает путь
    virtual std::string  getPath(const std::string  &fullName) const = 0; // static
    virtual std::wstring getPath(const std::wstring &fullName) const = 0; // static

    //! Возвращает имя и расширение
    virtual std::string  getFileName(const std::string  &fullName) const = 0; // static
    virtual std::wstring getFileName(const std::wstring &fullName) const = 0; // static

    //! Возвращает путь и имя
    virtual std::string  getPathFile(const std::string  &fullName) const = 0; // static
    virtual std::wstring getPathFile(const std::wstring &fullName) const = 0; // static

    //! Возвращает расширение
    virtual std::string  getExt(const std::string  &fullName) const = 0; // static
    virtual std::wstring getExt(const std::wstring &fullName) const = 0; // static

    //! Возвращает имя файла без пути и расширения
    virtual std::string  getName(const std::string  &fullName) const = 0; // static
    virtual std::wstring getName(const std::wstring &fullName) const = 0; // static

    //! Конкатенация путей
    virtual std::string  appendPath(const std::string  &pathAppendTo, const std::string  &appendPath) const = 0; // static
    virtual std::wstring appendPath(const std::wstring &pathAppendTo, const std::wstring &appendPath) const = 0; // static

    //! Добавление расширения
    virtual std::string  appendExt(const std::string  &nameAppendTo, const std::string  &appendExt) const = 0; // static
    virtual std::wstring appendExt(const std::wstring &nameAppendTo, const std::wstring &appendExt) const = 0; // static


    // Тут автоматически работают перекодировки текста
    virtual ErrorCode readConfTextFile(const std::string  &fName, std::string  &fText) const = 0;
    virtual ErrorCode readConfTextFile(const std::string  &fName, std::wstring &fText) const = 0;
    virtual ErrorCode readConfTextFile(const std::wstring &fName, std::string  &fText) const = 0;
    virtual ErrorCode readConfTextFile(const std::wstring &fName, std::wstring &fText) const = 0;

    // Reading binary files
    virtual ErrorCode readConfDataFile(const std::string  &fName, std::vector<std::uint8_t> &fData) const = 0;
    virtual ErrorCode readConfDataFile(const std::wstring &fName, std::vector<std::uint8_t> &fData) const = 0;

    virtual ErrorCode readConfJson(const std::string  &fName, nlohmann::json &j) const = 0;
    virtual ErrorCode readConfJson(const std::wstring &fName, nlohmann::json &j) const = 0;

    virtual ErrorCode readAssetsDataFile(const std::string  &fName, std::vector<std::uint8_t> &fData) const = 0;
    virtual ErrorCode readAssetsDataFile(const std::wstring &fName, std::vector<std::uint8_t> &fData) const = 0;

    virtual ErrorCode readIconData(const std::string  &iconName, std::vector<std::uint8_t> &iconData) const = 0;
    virtual ErrorCode readIconData(const std::wstring &iconName, std::vector<std::uint8_t> &iconData) const = 0;

    virtual ErrorCode readAppIconData(std::vector<std::uint8_t> &iconData) const = 0;


}; // struct IAssetsManager


} // namespace marty_assets_manager


// #include "warnings_restore.h"


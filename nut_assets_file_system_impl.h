/*! \file
    \brief Prepare IFileSystem for asset loader
*/

#pragma once

#include "marty_virtual_fs/app_paths_impl.h"
#include "marty_virtual_fs/filesystem_impl.h"

//
#include "marty_virtual_fs/i_app_paths.h"
#include "marty_virtual_fs/i_app_paths_common.h"
#include "marty_virtual_fs/i_virtual_fs.h"

//
#include "umba/filename.h"
#include "umba/filesys.h"

//
#include <memory>

namespace marty_assets {


// Для настройки VFS как файловой системы для ассетов нам нужно знать корень апликухи в файловой системе
// Это bool IAppPaths::getAppRootPath(...)
// Нужен IAppPaths

// Чтобы настроить IFileSystem, нам нужен связанный с ней IVirtualFs
// Нужен IVirtualFs

//----------------------------------------------------------------------------
inline
void configureNutAssetsFilesystem(marty_virtual_fs::IAppPaths *pAppPaths, marty_virtual_fs::IVirtualFs *pVirtualFs)
{
    pVirtualFs->clearMounts();

    std::wstring appRootPath;

    pAppPaths->getAppRootPath(appRootPath);
    // ошибку игнорим - всё равно нихрена не сделать. Только если самим вернуть ошибку
    // bool getAppRootPath(std::string  &p) const = 0;

    pVirtualFs->addMountPoint(L"nuts"           , pVirtualFs->appendPath(appRootPath, L"nuts"        ));
    pVirtualFs->addMountPoint(L"assets"         , pVirtualFs->appendPath(appRootPath, L"assets"      ));
    pVirtualFs->addMountPoint(L"translations"   , pVirtualFs->appendPath(appRootPath, L"translations"));

    std::wstring appName;
    pAppPaths->getAppName(appName);

    std::wstring manifestFileName = umba::filename::appendExt(appName, umba::string_plus::make_string<std::wstring>(".dotnut-manifest.json"));
    std::wstring manifestFullName = umba::filename::appendPath(appRootPath, manifestFileName);

    if (umba::filesys::isFileReadable(manifestFullName))
    {
        pVirtualFs->addMountPointEx(manifestFileName, manifestFullName, marty_virtual_fs::FileTypeFlags::normalFile);
    }

}

//----------------------------------------------------------------------------
inline
std::shared_ptr<marty_virtual_fs::IFileSystem> makeNutAssetsFilesystemSharedPtr()
{
    auto pFsImpl = std::make_shared<marty_virtual_fs::FileSystemImpl>();

    //auto pVfs    = std::static_pointer_cast<marty_virtual_fs::VirtualFsImpl>(pFsImpl);
    auto pVfs    = std::static_pointer_cast<marty_virtual_fs::IVirtualFs>(pFsImpl);

    auto pAppPathsImpl   = std::make_shared<marty_virtual_fs::AppPathsImpl>();
    auto pAppPaths       = std::static_pointer_cast<marty_virtual_fs::IAppPaths>(pAppPathsImpl);

    configureNutAssetsFilesystem(pAppPaths.get(), pVfs.get());

    return std::static_pointer_cast<marty_virtual_fs::IFileSystem>(pFsImpl);
}

//----------------------------------------------------------------------------
inline
marty_virtual_fs::IFileSystem* makeNutAssetsFilesystemPtr()
{
    auto pFsImpl = new marty_virtual_fs::FileSystemImpl();

    //auto pVfs    = std::static_pointer_cast<marty_virtual_fs::VirtualFsImpl>(pFsImpl);
    auto pVfs    = static_cast<marty_virtual_fs::IVirtualFs*>(pFsImpl);

    auto pAppPathsImpl   = new marty_virtual_fs::AppPathsImpl();
    auto pAppPaths       = static_cast<marty_virtual_fs::IAppPaths*>(pAppPathsImpl);

    configureNutAssetsFilesystem(pAppPaths, pVfs);

    return static_cast<marty_virtual_fs::IFileSystem*>(pFsImpl);
}


//auto pFsImpl = std::make_shared<marty_virtual_fs::FileSystemImpl>();

} // namespace marty_assets


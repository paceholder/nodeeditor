#include "PluginsManager.hpp"

#include "NodeDelegateModelRegistry.hpp"

#include <algorithm>
#include <utility>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QPluginLoader>

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

namespace QtNodes {

PluginsManager *PluginsManager::_instance = nullptr;

PluginsManager::PluginsManager()
{
    if (!_register)
        _register = std::make_shared<NodeDelegateModelRegistry>();
}

PluginsManager::~PluginsManager()
{
    unloadPlugins();

    if (PluginsManager::instance()) {
        delete PluginsManager::instance();
        PluginsManager::_instance = nullptr;
    }
}

PluginsManager *PluginsManager::instance()
{
    if (_instance == nullptr)
        _instance = new PluginsManager();
    return _instance;
}

std::shared_ptr<NodeDelegateModelRegistry> PluginsManager::registry()
{
    return _register;
}

/**
 * @brief Recursively loads all plugins with the specified suffix according to the folder path.
 * If no suffix is specified then the choice is up to the OS. For example, Windows OS selects `*.dll`
 * 
 * ```
 * │  plugins_load
 * │  QtNodes.dll
 * │
 * └─plugins
 *     │
 *     └─text
 *          plugin_text.node
 *          text_dependent.dll
 * ```
 * @TODO: Currently only tested and passed under windows, is there a solution for Qt for all three platforms?
 * 1. `plugins_Load` can successfully load `plugin_text.node`
 * 2. After changing the folder name `text` it still loads successfully
 * 
 * @param folderPath 
 * @param nameFilters 
 */
void PluginsManager::loadPlugins(const QString &folderPath, const QStringList &nameFilters)
{
    QDir pluginsDir;
    if (!pluginsDir.exists(folderPath)) {
        // Created if folderPath does not exist
        pluginsDir.mkpath(folderPath);
    }
    pluginsDir.cd(folderPath);

    auto IsLibrary = [](const QFileInfo f, const QStringList &nameFilters) {
        if (!f.isFile())
            return false;

        if (nameFilters.isEmpty())
            return QLibrary::isLibrary(f.absoluteFilePath());

        for (auto s : nameFilters) {
            if (s.endsWith(f.suffix(), Qt::CaseInsensitive))
                return true;
        }
        return false;
    };

    QDirIterator it(pluginsDir.path(),
                    QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
    while (it.hasNext()) {
        it.next();
        QFileInfo f = it.fileInfo();
        if (f.isDir()) {
            loadPlugins(it.filePath(), nameFilters);
        } else if (f.isFile() && IsLibrary(f, nameFilters)) {
#if defined(Q_OS_WIN)
#ifdef UNICODE
            SetDllDirectory(folderPath.toStdWString().c_str());
#else
            SetDllDirectory(folderPath.toStdString().c_str());
#endif // !UNICODE
#endif
            loadPluginFromPath(it.filePath());

#if defined(Q_OS_WIN)
            SetDllDirectory(NULL);
#endif
        }
    }
}

void PluginsManager::unloadPlugins()
{
    for (auto l : _loaders) {
        l.second->unload();
        delete l.second;
    }
    _loaders.clear();
}

PluginInterface *PluginsManager::loadPluginFromPath(const QString &filePath)
{
    QPluginLoader *loader = new QPluginLoader(filePath);

    if (loader->isLoaded()) {
        PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());

        QPluginLoader *l = _loaders.find(plugin->name())->second;
        plugin = qobject_cast<PluginInterface *>(l->instance());

        loader->unload();
        delete loader;

        return plugin;
    }

    PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
    if (plugin) {
        _loaders[plugin->name()] = loader;

        return plugin;
    } else {
        qWarning() << loader->errorString();

        delete loader;
    }

    return nullptr;
}

std::vector<PluginInterface *> PluginsManager::loadPluginFromPaths(const QStringList filePaths,
                                                                   const QStringList &nameFilters)
{
    std::vector<PluginInterface *> vecPlugins;
    vecPlugins.clear();

    auto IsLibrary = [](const QFileInfo f, const QStringList &nameFilters) {
        if (!f.isFile())
            return false;

        if (nameFilters.isEmpty())
            return QLibrary::isLibrary(f.absoluteFilePath());

        for (auto nf : nameFilters) {
            if (nf.endsWith(f.suffix(), Qt::CaseInsensitive))
                return true;
        }
        return false;
    };

    for (auto path : filePaths) {
        QFileInfo f(path);
        if (IsLibrary(f, nameFilters))
            vecPlugins.push_back(loadPluginFromPath(path));
    }
    return vecPlugins;
}

bool PluginsManager::unloadPluginFromPath(const QString &filePath)
{
    for (auto l : _loaders) {
        if (l.second->fileName() == filePath) {
            if (l.second->unload() == false) {
                return false;
            }
            delete l.second;
            _loaders.erase(l.first);
            return true;
        }
    }
    return false;
}

bool PluginsManager::unloadPluginFromName(const QString &pluginName)
{
    auto loaderIter = _loaders.find(pluginName);
    if (loaderIter != _loaders.end()) {
        if (loaderIter->second->unload() == false) {
            return false;
        }
        delete loaderIter->second;
        _loaders.erase(loaderIter->first);
        return true;
    }
    return false;
}

} // namespace QtNodes

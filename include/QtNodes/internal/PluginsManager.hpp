#pragma once

#include "Export.hpp"
#include "PluginInterface.hpp"

#include <unordered_map>
#include <vector>
#include <QObject>
#include <QPluginLoader>

namespace QtNodes {

class NodeDelegateModelRegistry;

class NODE_EDITOR_PUBLIC PluginsManager
{
    PluginsManager();

    virtual ~PluginsManager();

public:
    static PluginsManager *instance();

    std::shared_ptr<NodeDelegateModelRegistry> registry();

    void loadPlugins(const QString &folderPath = "./plugins",
                     const QStringList &nameFilters = QStringList());

    void unloadPlugins();

    /**
     * @brief Load the plug-in from the full file path
     * 
     * @param filePath "C:/plugin_text.dll"
     * @return PluginInterface* 
     */
    PluginInterface *loadPluginFromPath(const QString &filePath);

    std::vector<PluginInterface *> loadPluginFromPaths(const QStringList filePaths,
                                                       const QStringList &nameFilters);

    /**
     * @brief Unload the plugin from the full file path
     * 
     * @param filePath "C:/plugin_text.dll"
     * @return bool 
     */
    bool unloadPluginFromPath(const QString &filePath);

    /**
     * @brief Uninstall a plugin by its name, not its file name
     * 
     * @param pluginName "pluginText"
     * @return bool 
     */
    bool unloadPluginFromName(const QString &pluginName);

    inline std::unordered_map<QString, QPluginLoader *> loaders() { return _loaders; };

private:
    static PluginsManager *_instance;

    std::unordered_map<QString, QPluginLoader *> _loaders; // plugin name

    std::shared_ptr<NodeDelegateModelRegistry> _register;
};

} // namespace QtNodes
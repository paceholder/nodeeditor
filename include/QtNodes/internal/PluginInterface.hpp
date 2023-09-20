#pragma once

#include <QtPlugin>

namespace QtNodes {

class NodeDelegateModelRegistry;

class PluginInterface
{
public:
    virtual ~PluginInterface() = default;

    virtual QString name() const = 0;

    virtual void registerDataModels(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> &reg) = 0;
};

} // namespace QtNodes

Q_DECLARE_INTERFACE(QtNodes::PluginInterface, "QtNodes.PluginInterface/1.0")

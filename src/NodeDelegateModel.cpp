#include "NodeDelegateModel.hpp"

#include "StyleCollection.hpp"

namespace QtNodes {

NodeDelegateModel::NodeDelegateModel()
    : _nodeStyle(StyleCollection::nodeStyle())
{
    // Derived classes can initialize specific style here
}

NodeDelegateModel::NodeDelegateModel(const QString &name,
                                     const QString &caption,
                                     const QString &category,
                                     unsigned int inCount,
                                     unsigned int outCount)
    : _caption(caption)
    , _name(name)
    , _category(category)
    , _inCount(inCount)
    , _outCount(outCount)
{}

QJsonObject NodeDelegateModel::save() const
{
    QJsonObject modelJson;

    modelJson["model-name"] = name();

    return modelJson;
}

void NodeDelegateModel::load(QJsonObject const &)
{
    //
}

ConnectionPolicy NodeDelegateModel::portConnectionPolicy(PortType portType, PortIndex) const
{
    auto result = ConnectionPolicy::One;
    switch (portType) {
    case PortType::In:
        result = ConnectionPolicy::Many;
        break;
    case PortType::Out:
        result = ConnectionPolicy::Many;
        break;
    case PortType::None:
        break;
    }

    return result;
}

NodeStyle const &NodeDelegateModel::nodeStyle() const
{
    return _nodeStyle;
}

void NodeDelegateModel::setNodeStyle(NodeStyle const &style)
{
    _nodeStyle = style;
}

unsigned int NodeDelegateModel::nPorts(PortType portType) const
 {
    if (portType == PortType::In) {
        return _inCount;
    } else if (portType == PortType::Out) {
        return _outCount;
    }
    return 0;
}

QString NodeDelegateModel::caption() const {
    return _caption;
}

QString NodeDelegateModel::name() const
{
    return _name;
}

QString NodeDelegateModel::category() const
{
    return _category;
}

} // namespace QtNodes

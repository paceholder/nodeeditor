#include "NodeDelegateModel.hpp"

#include "StyleCollection.hpp"

namespace QtNodes {

NodeDelegateModel::NodeDelegateModel()
    : _nodeStyle(StyleCollection::nodeStyle())
{
    // Derived classes can initialize specific style here
}

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

void NodeDelegateModel::setValidationState(const NodeValidationState &validationState)
{
    _nodeValidationState = validationState;
}

ConnectionPolicy NodeDelegateModel::portConnectionPolicy(PortType portType, PortIndex) const
{
    auto result = ConnectionPolicy::One;
    switch (portType) {
    case PortType::In:
        result = ConnectionPolicy::One;
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

QPixmap NodeDelegateModel::processingStatusIcon() const
{
    int resolution = _nodeStyle.processingIconStyle._resolution;
    switch (_processingStatus) {
    case NodeProcessingStatus::NoStatus:
        return {};
    case NodeProcessingStatus::Updated:
        return _nodeStyle.statusUpdated.pixmap(resolution);
    case NodeProcessingStatus::Processing:
        return _nodeStyle.statusProcessing.pixmap(resolution);
    case NodeProcessingStatus::Pending:
        return _nodeStyle.statusPending.pixmap(resolution);
    case NodeProcessingStatus::Empty:
        return _nodeStyle.statusEmpty.pixmap(resolution);
    case NodeProcessingStatus::Failed:
        return _nodeStyle.statusInvalid.pixmap(resolution);
    case NodeProcessingStatus::Partial:
        return _nodeStyle.statusPartial.pixmap(resolution);
    }

    return {};
}

void NodeDelegateModel::setStatusIcon(NodeProcessingStatus status, const QPixmap &pixmap)
{
    switch (status) {
    case NodeProcessingStatus::NoStatus:
        break;
    case NodeProcessingStatus::Updated:
        _nodeStyle.statusUpdated = QIcon(pixmap);
        break;
    case NodeProcessingStatus::Processing:
        _nodeStyle.statusProcessing = QIcon(pixmap);
        break;
    case NodeProcessingStatus::Pending:
        _nodeStyle.statusPending = QIcon(pixmap);
        break;
    case NodeProcessingStatus::Empty:
        _nodeStyle.statusEmpty = QIcon(pixmap);
        break;
    case NodeProcessingStatus::Failed:
        _nodeStyle.statusInvalid = QIcon(pixmap);
        break;
    case NodeProcessingStatus::Partial:
        _nodeStyle.statusPartial = QIcon(pixmap);
        break;
    }
}

void NodeDelegateModel::setStatusIconStyle(const ProcessingIconStyle &style)
{
    _nodeStyle.processingIconStyle = style;
}

void NodeDelegateModel::setNodeProcessingStatus(NodeProcessingStatus status)
{
    _processingStatus = status;
}

void NodeDelegateModel::setBackgroundColor(QColor const &color)
{
    _nodeStyle.setBackgroundColor(color);
}

} // namespace QtNodes

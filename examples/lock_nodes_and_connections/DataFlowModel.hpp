#include <QtNodes/DataFlowGraphModel>

using QtNodes::ConnectionId;
using QtNodes::DataFlowGraphModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeFlag;
using QtNodes::NodeFlags;
using QtNodes::NodeId;

class DataFlowModel : public DataFlowGraphModel
{
public:
    DataFlowModel(std::shared_ptr<NodeDelegateModelRegistry> registry)
        : DataFlowGraphModel(std::move(registry))
        , _detachPossible{true}
        , _nodesLocked{false}
    {}

    bool detachPossible(ConnectionId const) const override { return _detachPossible; }

    void setDetachPossible(bool d = true) { _detachPossible = d; }

    //----

    NodeFlags nodeFlags(NodeId nodeId) const override
    {
        auto basicFlags = DataFlowGraphModel::nodeFlags(nodeId);

        if (_nodesLocked)
            basicFlags |= NodeFlag::Locked;

        return basicFlags;
    }

    void setNodesLocked(bool b = true)
    {
        _nodesLocked = b;

        for (NodeId nodeId : allNodeIds()) {
            Q_EMIT nodeFlagsUpdated(nodeId);
        }
    }

private:
    bool _detachPossible;

    bool _nodesLocked;
};

#pragma once

#include <QtNodes/Definitions>

#include <set>

/// Adds NodeFlag::Locked to a graph model, per node.
/// Base is a graph model with a nodeFlags() method, such as TestGraphModel
/// or DataFlowGraphModel. Neither model can lock a node by itself.
template<class Base>
class LockableModel : public Base
{
public:
    using Base::Base;

    QtNodes::NodeFlags nodeFlags(QtNodes::NodeId nodeId) const override
    {
        auto flags = Base::nodeFlags(nodeId);
        if (_locked.count(nodeId) != 0)
            flags |= QtNodes::NodeFlag::Locked;
        return flags;
    }

    void setLocked(QtNodes::NodeId nodeId, bool locked)
    {
        if (locked)
            _locked.insert(nodeId);
        else
            _locked.erase(nodeId);
        Q_EMIT this->nodeFlagsUpdated(nodeId);
    }

private:
    std::set<QtNodes::NodeId> _locked;
};

#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include <vector>

#include "DataFlowGraphModel.hpp"
#include "Export.hpp"
#include "GroupGraphicsObject.hpp"
#include "NodeState.hpp"
//#include "memory.hpp"
#include "Definitions.hpp"
#include "NodeConnectionInteraction.hpp"

namespace QtNodes {

class DataFlowGraphModel;
class GroupGraphicsObject;
class NodeState;
class ConnectionId;
class NodeConnectionInteraction;
class NodeGraphicsObject;

/**
 * @brief The NodeGroup class defines a controller for node groups. It is
 * responsible for managing the group's data and the interactions with other
 * classes.
 */
class NODE_EDITOR_PUBLIC NodeGroup : public QObject
{
    Q_OBJECT

public:
    /**
   * @brief Constructor to create a node group with the given nodes.
   * @param nodes List of node pointers to be included in the group.
   * @param name Group's name. If none is given, it is automatically generated.
   * @param parent Parent object.
   */
    NodeGroup(std::vector<NodeGraphicsObject *> nodes,
              const QUuid &uid,
              QString name = QString(),
              QObject *parent = nullptr);

public:
    /**
   * @brief Prepares a byte array containing this group's data to  be saved in a
   * file.
   * @return A byte array containing this group's data (in JSON format).
   */
    QByteArray saveToFile() const;

    /**
   * @brief Returns this group's universal ID.
   */
    QUuid id() const;

    /**
   * @brief Returns a reference to this group's graphical object.
   */
    GroupGraphicsObject &groupGraphicsObject();

    /**
   * @brief Returns a const reference to this group's graphical object.
   */
    GroupGraphicsObject const &groupGraphicsObject() const;

    /**
   * @brief Returns the list of nodes that belong to this group.
   */
    std::vector<NodeGraphicsObject *> &childNodes();

    /**
   * @brief Returns a list of IDs of the nodes that belong to this group.
   */
    std::vector<NodeId> nodeIDs() const;

    /**
   * @brief Returns this group's name.
   */
    QString const &name() const;

    /**
   * @brief Associates a GroupGraphicsObject with this group.
   */
    void setGraphicsObject(std::unique_ptr<GroupGraphicsObject> &&graphics_object);

    /**
   * @brief Returns whether the group's list of nodes is empty.
   */
    bool empty() const;

    /**
   * @brief Returns the number of groups created during the program's execution.
   * Used when automatically naming groups.
   */
    static int groupCount();

public Q_SLOTS:
    /**
   * @brief Adds a node to this group.
   * @param node Pointer to the node to be added.
   */
    void addNode(QtNodes::NodeGraphicsObject *node);

    /**
   * @brief Removes a node from this group.
   * @param node Pointer to the node to be removed.
   */
    void removeNode(QtNodes::NodeGraphicsObject *node);

private:
    /**
   * @brief Group's name, just a label so the user can easily identify and
   * label groups. It is not a unique identifier for the group.
   */
    QString _name;

    // addressing
    /**
   * @brief Universal ID of this group. It is the only unique identifier of
   * the group.
   */
    QUuid _uid;

    // data
    /**
   * @brief List of pointers of nodes that belong to this group.
   */
    std::vector<NodeGraphicsObject *> _childNodes;

    // painting
    /**
   * @brief Pointer to the graphical object associated with this group.
   */
    std::unique_ptr<GroupGraphicsObject> _groupGraphicsObject;

    /**
   * @brief Static variable to count the number of instances of groups that
   * were created during execution. Used when automatically naming groups.
   */
    static int _groupCount;
};
} // namespace QtNodes

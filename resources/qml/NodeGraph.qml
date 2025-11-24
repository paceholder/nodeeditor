import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15
import QtNodes 1.0

Item {
    id: root
    property QuickGraphModel graphModel

    property var nodeItems: ({})
    property Component nodeContentDelegate // User provided content

    function registerNode(id, item) {
        nodeItems[id] = item
        // Trigger update for connections? 
        // Since nodeItems is a var, changes don't automatically trigger bindings unless we assign to a property.
        // But connections will look up nodeItems.
        nodeRegistryChanged()
    }
    
    signal nodeRegistryChanged()

    // Temporary drafting connection
    property point dragStart
    property point dragCurrent
    property bool isDragging: false

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: 5000
        contentHeight: 5000
        
        Item {
            id: canvas
            width: 5000
            height: 5000

            // Background
            Rectangle {
                anchors.fill: parent
                color: "#3c3c3c"
            }

            // Grid
            Shape {
                anchors.fill: parent
                ShapePath {
                    strokeWidth: 1
                    strokeColor: "#505050"
                    fillColor: "transparent"
                    
                    // Vertical lines
                    startX: 0; startY: 0
                    PathMultiline {
                        paths: {
                            var p = []
                            for (var i = 0; i < 5000; i += 20) {
                                p.push(Qt.point(i, 0))
                                p.push(Qt.point(i, 5000))
                            }
                            return p
                        }
                    }
                }
                ShapePath {
                    strokeWidth: 1
                    strokeColor: "#505050"
                    fillColor: "transparent"
                    
                    // Horizontal lines
                    startX: 0; startY: 0
                    PathMultiline {
                         paths: {
                            var p = []
                            for (var j = 0; j < 5000; j += 20) {
                                p.push(Qt.point(0, j))
                                p.push(Qt.point(5000, j))
                            }
                            return p
                        }
                    }
                }
            }

            // Connections
            Repeater {
                model: graphModel ? graphModel.connections : null
                delegate: Connection {
                    graph: root
                    modelData: model
                }
            }
            
            // Nodes
            Repeater {
                model: graphModel ? graphModel.nodes : null
                delegate: Node {
                    id: nodeDelegate
                    graph: root
                    
                    // Model Roles
                    nodeId: model.nodeId
                    nodeType: model.nodeType
                    initialX: model.position.x
                    initialY: model.position.y
                    caption: model.caption
                    inPorts: model.inPorts
                    outPorts: model.outPorts
                    delegateModel: model.model // The C++ QObject*
                    contentDelegate: root.nodeContentDelegate
                    
                    onXChanged: {
                         if (completed) graphModel.nodes.moveNode(nodeId, x, y)
                    }
                    onYChanged: {
                         if (completed) graphModel.nodes.moveNode(nodeId, x, y)
                    }
                    
                    Component.onCompleted: {
                        root.registerNode(nodeId, nodeDelegate)
                    }
                }
            }
            
            // Dragging Connection
            Shape {
                visible: root.isDragging
                ShapePath {
                    strokeWidth: 2
                    strokeColor: "orange"
                    fillColor: "transparent"
                    startX: root.dragStart.x
                    startY: root.dragStart.y
                    PathCubic {
                        x: root.dragCurrent.x
                        y: root.dragCurrent.y
                        control1X: root.dragStart.x + 50
                        control1Y: root.dragStart.y
                        control2X: root.dragCurrent.x - 50
                        control2Y: root.dragCurrent.y
                    }
                }
            }
        }
    }
}

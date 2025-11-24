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
        nodeRegistryChanged()
    }
    
    signal nodeRegistryChanged()

    // Zoom and Pan
    property real zoomLevel: 1.0
    property point panOffset: Qt.point(0, 0)
    
    // Temporary drafting connection
    property point dragStart
    property point dragCurrent
    property bool isDragging: false

    Rectangle {
        anchors.fill: parent
        color: "#2b2b2b"
        clip: true

        // Grid Shader
        ShaderEffect {
            anchors.fill: parent
            property real zoom: root.zoomLevel
            property point offset: root.panOffset
            property size size: Qt.size(width, height)
            
            fragmentShader: "
                varying highp vec2 qt_TexCoord0;
                uniform highp float zoom;
                uniform highp vec2 offset;
                uniform highp vec2 size;
                
                void main() {
                    lowp vec2 coord = (qt_TexCoord0 * size - offset) / zoom;
                    lowp vec2 grid = abs(fract(coord / 20.0 - 0.5) - 0.5) / fwidth(coord / 20.0);
                    lowp float line = min(grid.x, grid.y);
                    lowp float alpha = 1.0 - min(line, 1.0);
                    
                    // Major grid lines
                    lowp vec2 grid2 = abs(fract(coord / 100.0 - 0.5) - 0.5) / fwidth(coord / 100.0);
                    lowp float line2 = min(grid2.x, grid2.y);
                    lowp float alpha2 = 1.0 - min(line2, 1.0);
                    
                    gl_FragColor = vec4(0.6, 0.6, 0.6, max(alpha * 0.1, alpha2 * 0.3));
                }
            "
        }

        // Graph Content Area
        Item {
            id: canvas
            width: 5000 // Virtual size, but we rely on infinite panning logic visually
            height: 5000
            x: root.panOffset.x
            y: root.panOffset.y
            scale: root.zoomLevel
            transformOrigin: Item.TopLeft

            // Connections
            Repeater {
                model: graphModel ? graphModel.connections : null
                delegate: Connection {
                    graph: root
                    property int sourceNodeId: model.sourceNodeId
                    property int sourcePortIndex: model.sourcePortIndex
                    property int destNodeId: model.destNodeId
                    property int destPortIndex: model.destPortIndex
                }
            }
            
            // Nodes
            Repeater {
                model: graphModel ? graphModel.nodes : null
                delegate: Node {
                    id: nodeDelegate
                    graph: root
                    
                    // Model Roles
                    property int nodeId: model.nodeId
                    property string nodeType: model.nodeType
                    property real initialX: model.position.x
                    property real initialY: model.position.y
                    property string caption: model.caption
                    property int inPorts: model.inPorts
                    property int outPorts: model.outPorts
                    property var delegateModel: model.delegateModel // The C++ QObject*
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

        // Input Handler for Pan/Zoom
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.MiddleButton | Qt.LeftButton
            property point lastPos

            onPressed: {
                lastPos = Qt.point(mouse.x, mouse.y)
            }

            onPositionChanged: {
                if (pressedButtons & Qt.MiddleButton || (pressedButtons & Qt.LeftButton && (mouse.modifiers & Qt.AltModifier))) {
                    var delta = Qt.point(mouse.x - lastPos.x, mouse.y - lastPos.y)
                    root.panOffset = Qt.point(root.panOffset.x + delta.x, root.panOffset.y + delta.y)
                    lastPos = Qt.point(mouse.x, mouse.y)
                }
            }

            onWheel: {
                var zoomFactor = 1.1
                if (wheel.angleDelta.y < 0) {
                    zoomLevel /= zoomFactor
                } else {
                    zoomLevel *= zoomFactor
                }
                // Clamp zoom
                if (zoomLevel < 0.1) zoomLevel = 0.1
                if (zoomLevel > 5.0) zoomLevel = 5.0
            }
        }
    }
}

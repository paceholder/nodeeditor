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
            
            // In Qt6, we'd normally use .qsb files. 
            // But to keep it simple and cross-version compatible (Qt5/Qt6), 
            // let's revert to the standard Shape-based grid for now, 
            // as inline shaders are deprecated/removed in Qt6's RHI.
            // Or we can use a Canvas which is easier than Shapes for infinite grids.
            visible: false 
        }
        
        // Canvas Grid
        Canvas {
             id: gridCanvas
             anchors.fill: parent
             property real zoom: root.zoomLevel
             property point offset: root.panOffset
             
             onZoomChanged: requestPaint()
             onOffsetChanged: requestPaint()
             
             onPaint: {
                 var ctx = getContext("2d")
                 ctx.clearRect(0, 0, width, height)
                 
                 ctx.lineWidth = 1
                 
                 var gridSize = 20 * zoom
                 var majorGridSize = 100 * zoom
                 
                 var startX = (offset.x % gridSize)
                 var startY = (offset.y % gridSize)
                 
                 if (startX < 0) startX += gridSize
                 if (startY < 0) startY += gridSize
                 
                 // Minor lines
                 ctx.strokeStyle = "#353535"
                 ctx.beginPath()
                 
                 // Vertical lines
                 for (var x = startX; x < width; x += gridSize) {
                     ctx.moveTo(x, 0)
                     ctx.lineTo(x, height)
                 }
                 
                 // Horizontal lines
                 for (var y = startY; y < height; y += gridSize) {
                     ctx.moveTo(0, y)
                     ctx.lineTo(width, y)
                 }
                 
                 ctx.stroke()
                 
                 // Major lines
                 ctx.strokeStyle = "#151515"
                 ctx.beginPath()
                 var mStartX = (offset.x % majorGridSize)
                 var mStartY = (offset.y % majorGridSize)
                 if (mStartX < 0) mStartX += majorGridSize
                 if (mStartY < 0) mStartY += majorGridSize
                 
                 for (var mx = mStartX; mx < width; mx += majorGridSize) {
                     ctx.moveTo(mx, 0)
                     ctx.lineTo(mx, height)
                 }
                 for (var my = mStartY; my < height; my += majorGridSize) {
                     ctx.moveTo(0, my)
                     ctx.lineTo(width, my)
                 }
                 ctx.stroke()
             }
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

            onPressed: (mouse) => {
                lastPos = Qt.point(mouse.x, mouse.y)
            }

            onPositionChanged: (mouse) => {
                if (pressedButtons & Qt.MiddleButton || (pressedButtons & Qt.LeftButton && (mouse.modifiers & Qt.AltModifier))) {
                    var delta = Qt.point(mouse.x - lastPos.x, mouse.y - lastPos.y)
                    root.panOffset = Qt.point(root.panOffset.x + delta.x, root.panOffset.y + delta.y)
                    lastPos = Qt.point(mouse.x, mouse.y)
                }
            }

            onWheel: (wheel) => {
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

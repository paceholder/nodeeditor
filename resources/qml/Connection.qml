import QtQuick 2.15
import QtQuick.Shapes 1.15

Item {
    id: root
    property var graph
    property var style: graph ? graph.style : null
    
    property int sourceNodeId: -1
    property int sourcePortIndex: -1
    property int destNodeId: -1
    property int destPortIndex: -1
    
    property var sourceNode: graph.nodeItems[sourceNodeId]
    property var destNode: graph.nodeItems[destNodeId]
    
    property bool selected: graph.isConnectionSelected(sourceNodeId, sourcePortIndex, destNodeId, destPortIndex)
    property bool hovered: false
    
    property string portTypeId: graph.getPortTypeId(sourceNodeId, 1, sourcePortIndex)
    property color lineColor: graph.getPortColor(portTypeId)
    
    Connections {
        target: graph
        function onNodeRegistryChanged() {
            sourceNode = graph.nodeItems[sourceNodeId]
            destNode = graph.nodeItems[destNodeId]
        }
        function onConnectionSelectionChanged() {
            selected = graph.isConnectionSelected(sourceNodeId, sourcePortIndex, destNodeId, destPortIndex)
        }
    }

    Connections {
        target: sourceNode
        function onXChanged() { root.updatePositions() }
        function onYChanged() { root.updatePositions() }
    }
    Connections {
        target: destNode
        function onXChanged() { root.updatePositions() }
        function onYChanged() { root.updatePositions() }
    }

    property point startPos: Qt.point(0,0)
    property point endPos: Qt.point(0,0)
    
    function updatePositions() {
        if (sourceNode && sourceNode.completed) {
            startPos = sourceNode.getPortPos(1, sourcePortIndex)
        }
        if (destNode && destNode.completed) {
            endPos = destNode.getPortPos(0, destPortIndex)
        }
    }
    
    onSourceNodeChanged: updatePositions()
    onDestNodeChanged: updatePositions()
    
    Component.onCompleted: updatePositions()

    visible: sourceNode !== undefined && destNode !== undefined && sourceNode.completed && destNode.completed

    // Bounding box for hit detection
    property real minX: Math.min(startPos.x, endPos.x) - 20
    property real minY: Math.min(startPos.y, endPos.y) - 20
    property real maxX: Math.max(startPos.x, endPos.x) + 20
    property real maxY: Math.max(startPos.y, endPos.y) + 20
    
    // Hit detection MouseArea covering the bounding box
    MouseArea {
        x: root.minX
        y: root.minY
        width: root.maxX - root.minX
        height: root.maxY - root.minY
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton
        propagateComposedEvents: true
        
        property bool isOverCurve: false
        
        onPositionChanged: (mouse) => {
            var canvasX = mouse.x + root.minX
            var canvasY = mouse.y + root.minY
            isOverCurve = root.distanceToCurve(canvasX, canvasY) < 10
            root.hovered = isOverCurve
        }
        
        onExited: {
            isOverCurve = false
            root.hovered = false
        }
        
        onPressed: (mouse) => {
            var canvasX = mouse.x + root.minX
            var canvasY = mouse.y + root.minY
            if (root.distanceToCurve(canvasX, canvasY) >= 10) {
                mouse.accepted = false
            }
        }
        
        onClicked: (mouse) => {
            var canvasX = mouse.x + root.minX
            var canvasY = mouse.y + root.minY
            if (root.distanceToCurve(canvasX, canvasY) < 10) {
                graph.forceActiveFocus()
                var additive = (mouse.modifiers & Qt.ControlModifier)
                graph.selectConnection(sourceNodeId, sourcePortIndex, destNodeId, destPortIndex, additive)
            } else {
                mouse.accepted = false
            }
        }
        
        cursorShape: isOverCurve ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
    
    function distanceToCurve(px, py) {
        var minDist = 999999
        var cp1x = startPos.x + Math.abs(endPos.x - startPos.x) * 0.5
        var cp1y = startPos.y
        var cp2x = endPos.x - Math.abs(endPos.x - startPos.x) * 0.5
        var cp2y = endPos.y
        
        for (var t = 0; t <= 1; t += 0.02) {
            var bx = bezierPoint(startPos.x, cp1x, cp2x, endPos.x, t)
            var by = bezierPoint(startPos.y, cp1y, cp2y, endPos.y, t)
            var dist = Math.sqrt((px - bx) * (px - bx) + (py - by) * (py - by))
            if (dist < minDist) minDist = dist
        }
        return minDist
    }
    
    function bezierPoint(p0, p1, p2, p3, t) {
        var u = 1 - t
        return u*u*u*p0 + 3*u*u*t*p1 + 3*u*t*t*p2 + t*t*t*p3
    }

    // Selection outline (behind the main line)
    Shape {
        anchors.fill: parent
        visible: root.selected
        
        ShapePath {
            strokeWidth: style.connectionSelectionOutlineWidth
            strokeColor: style.connectionSelectionOutline
            fillColor: "transparent"
            
            startX: root.startPos.x
            startY: root.startPos.y
            
            PathCubic {
                x: root.endPos.x
                y: root.endPos.y
                control1X: root.startPos.x + Math.abs(root.endPos.x - root.startPos.x) * 0.5
                control1Y: root.startPos.y
                control2X: root.endPos.x - Math.abs(root.endPos.x - root.startPos.x) * 0.5
                control2Y: root.endPos.y
            }
        }
    }

    // Visual connection line
    Shape {
        anchors.fill: parent
        
        ShapePath {
            strokeWidth: root.hovered ? style.connectionHoverWidth : style.connectionWidth
            strokeColor: root.hovered ? Qt.lighter(root.lineColor, 1.3) : root.lineColor
            fillColor: "transparent"
            
            startX: root.startPos.x
            startY: root.startPos.y
            
            PathCubic {
                x: root.endPos.x
                y: root.endPos.y
                control1X: root.startPos.x + Math.abs(root.endPos.x - root.startPos.x) * 0.5
                control1Y: root.startPos.y
                control2X: root.endPos.x - Math.abs(root.endPos.x - root.startPos.x) * 0.5
                control2Y: root.endPos.y
            }
        }
    }
}

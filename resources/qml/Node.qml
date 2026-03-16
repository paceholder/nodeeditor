import QtQuick 2.15

Item {
    id: root
    property var graph
    property int nodeId
    property string nodeType
    property string caption
    property int inPorts
    property int outPorts
    property var delegateModel
    property Component contentDelegate

    property real initialX
    property real initialY

    property bool completed: false
    property bool selected: {
        graph.selectionVersion
        return graph.isNodeSelected(nodeId)
    }

    property var style: graph ? graph.style : null

    property color nodeColor: style ? style.getNodeColor(nodeType) : "#2d2d2d"
    property string nodeIcon: style ? style.getNodeIcon(nodeType) : ""
    property color headerColor: style ? style.lightenColor(nodeColor, 0.12) : nodeColor
    property color bodyColor: nodeColor
    property color portColumnColor: style ? style.darkenColor(nodeColor, 0.10) : nodeColor

    property real headerHeight: style ? style.nodeHeaderHeight : 35
    property real portSz: style ? style.portSize : 10
    property real portSpacing: style ? style.nodePortSpacing : 10
    property real nodeRadius: style ? style.nodeRadius : 5

    x: initialX
    y: initialY

    width: style ? style.nodeMinWidth : 150
    height: {
        var portCount = Math.max(inPorts, outPorts)
        var portsH = portCount * (portSz + portSpacing)
        return Math.max(headerHeight + portsH + 10, 60)
    }

    Component.onCompleted: completed = true

    // Trigger canvas repaint on relevant changes
    onNodeColorChanged: nodeCanvas.requestPaint()
    onInPortsChanged: nodeCanvas.requestPaint()
    onOutPortsChanged: nodeCanvas.requestPaint()
    onWidthChanged: nodeCanvas.requestPaint()
    onHeightChanged: nodeCanvas.requestPaint()
    onSelectedChanged: nodeCanvas.requestPaint()

    // =================================================================
    // Canvas: draws body with real transparent cutouts at port positions
    // =================================================================
    Canvas {
        id: nodeCanvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.clearRect(0, 0, width, height)

            var w = root.width
            var h = root.height
            var r = nodeRadius
            var hh = headerHeight
            var cutR = portSz * 0.75
            var colW = portSz + 4  // port column width

            // --- Background with port cutouts (evenodd) ---
            ctx.beginPath()

            // Outer rounded rectangle
            ctx.moveTo(r, 0)
            ctx.lineTo(w - r, 0)
            ctx.arcTo(w, 0, w, r, r)
            ctx.lineTo(w, h - r)
            ctx.arcTo(w, h, w - r, h, r)
            ctx.lineTo(r, h)
            ctx.arcTo(0, h, 0, h - r, r)
            ctx.lineTo(0, r)
            ctx.arcTo(0, 0, r, 0, r)
            ctx.closePath()

            // Input port cutouts
            for (var i = 0; i < inPorts; i++) {
                var iy = hh + 5 + i * (portSz + portSpacing) + portSz / 2
                ctx.moveTo(cutR, iy)
                ctx.arc(0, iy, cutR, 0, 2 * Math.PI, false)
            }

            // Output port cutouts
            for (var j = 0; j < outPorts; j++) {
                var oy = hh + 5 + j * (portSz + portSpacing) + portSz / 2
                ctx.moveTo(w + cutR, oy)
                ctx.arc(w, oy, cutR, 0, 2 * Math.PI, false)
            }

            // Fill port column color (darker, behind everything)
            ctx.fillStyle = root.portColumnColor.toString()
            ctx.fill("evenodd")

            // --- Body center (main color, between port columns) ---
            ctx.beginPath()
            var bodyLeft = inPorts > 0 ? colW : 0
            var bodyRight = outPorts > 0 ? w - colW : w
            var bw = bodyRight - bodyLeft
            var bh = h - hh
            var blr = inPorts > 0 ? 0 : r  // bottom-left radius
            var brr = outPorts > 0 ? 0 : r  // bottom-right radius

            ctx.moveTo(bodyLeft, hh)
            ctx.lineTo(bodyRight, hh)
            ctx.lineTo(bodyRight, h - brr)
            if (brr > 0) ctx.arcTo(bodyRight, h, bodyRight - brr, h, brr)
            ctx.lineTo(bodyLeft + blr, h)
            if (blr > 0) ctx.arcTo(bodyLeft, h, bodyLeft, h - blr, blr)
            ctx.lineTo(bodyLeft, hh)
            ctx.closePath()
            ctx.fillStyle = root.bodyColor.toString()
            ctx.fill()

            // --- Header fill ---
            ctx.beginPath()
            ctx.moveTo(r, 0)
            ctx.lineTo(w - r, 0)
            ctx.arcTo(w, 0, w, r, r)
            ctx.lineTo(w, hh)
            ctx.lineTo(0, hh)
            ctx.lineTo(0, r)
            ctx.arcTo(0, 0, r, 0, r)
            ctx.closePath()
            ctx.fillStyle = root.headerColor.toString()
            ctx.fill()

            // --- Divider line ---
            ctx.beginPath()
            ctx.moveTo(0, hh)
            ctx.lineTo(w, hh)
            ctx.strokeStyle = "rgba(0,0,0,0.3)"
            ctx.lineWidth = 1
            ctx.stroke()

            // --- Node border (with same cutouts) ---
            ctx.beginPath()

            // Top edge
            ctx.moveTo(r, 0)
            ctx.lineTo(w - r, 0)
            ctx.arcTo(w, 0, w, r, r)

            // Right edge with output port gaps
            if (outPorts > 0) {
                for (var ro = 0; ro < outPorts; ro++) {
                    var roy = hh + 5 + ro * (portSz + portSpacing) + portSz / 2
                    // Line to top of cutout
                    ctx.lineTo(w, roy - cutR)
                    // Arc around the cutout (go around the outside)
                    ctx.moveTo(w, roy + cutR)
                }
            }

            // Continue right edge to bottom
            ctx.lineTo(w, h - r)
            ctx.arcTo(w, h, w - r, h, r)

            // Bottom edge
            ctx.lineTo(r, h)
            ctx.arcTo(0, h, 0, h - r, r)

            // Left edge with input port gaps
            if (inPorts > 0) {
                // Go from bottom up
                var lastY = h - r
                for (var ri = inPorts - 1; ri >= 0; ri--) {
                    var riy = hh + 5 + ri * (portSz + portSpacing) + portSz / 2
                    ctx.lineTo(0, riy + cutR)
                    ctx.moveTo(0, riy - cutR)
                }
            }

            // Continue left edge to top
            ctx.lineTo(0, r)
            ctx.arcTo(0, 0, r, 0, r)

            if (root.selected) {
                ctx.strokeStyle = style ? style.nodeSelectedBorder.toString() : "#4a9eff"
                ctx.lineWidth = 2
            } else {
                ctx.strokeStyle = "rgba(0,0,0,0.5)"
                ctx.lineWidth = 0.5
            }
            ctx.stroke()
        }
    }

    // =================================================================
    // Header icon + caption (on top of canvas)
    // =================================================================
    Image {
        id: iconImg
        x: 8
        y: (headerHeight - 16) / 2
        width: 16
        height: 16
        source: nodeIcon
        sourceSize: Qt.size(32, 32)
        visible: nodeIcon !== ""
    }

    Text {
        x: iconImg.visible ? iconImg.x + iconImg.width + 6 : 12
        y: (headerHeight - height) / 2
        width: root.width - x - 8
        text: caption
        color: Qt.rgba(0, 0, 0, 0.7)
        font.bold: true
        font.pixelSize: style ? style.nodeCaptionFontSize : 12
        font.family: "Poppins"
        elide: Text.ElideRight
    }

    // =================================================================
    // Content delegate
    // =================================================================
    Loader {
        id: contentLoader
        y: headerHeight + 2
        x: 10
        width: parent.width - 20
        height: parent.height - headerHeight - 15
        sourceComponent: contentDelegate
        onLoaded: {
            if (item) {
                item.delegateModel = Qt.binding(function(){ return root.delegateModel })
                item.nodeType = Qt.binding(function(){ return root.nodeType })
            }
        }
        Connections {
            target: root
            function onDelegateModelChanged() { if (contentLoader.item) contentLoader.item.delegateModel = root.delegateModel }
            function onNodeTypeChanged() { if (contentLoader.item) contentLoader.item.nodeType = root.nodeType }
        }
    }

    // =================================================================
    // Input Ports
    // =================================================================
    Column {
        id: inPortsColumn
        z: 10
        x: -portSz / 2 + 1
        y: headerHeight + 5
        spacing: portSpacing

        Repeater {
            id: inRepeater
            model: inPorts
            delegate: Item {
                id: inPortItem
                width: portSz
                height: portSz
                property string portTypeId: graph.getPortTypeId(root.nodeId, 0, index)
                property bool isCompatible: !graph.isDragging ||
                    (graph.activeConnectionStart && graph.activeConnectionStart.portType === 1 &&
                     graph.draftConnectionTypeId === portTypeId)
                property bool isDimmed: graph.isDragging && !isCompatible
                property bool isConnected: { graph.connectionVersion; return graph.graphModel.portConnected(root.nodeId, 0, index) }

                Rectangle {
                    id: inPortCircle
                    width: inPortItem.isConnected ? portSz : 4
                    height: width
                    radius: width / 2
                    anchors.centerIn: parent
                    antialiasing: true
                    color: inPortItem.isConnected ? graph.getPortColor(inPortItem.portTypeId) : "transparent"
                    opacity: inPortItem.isDimmed ? (style ? style.portDimmedOpacity : 0.3) : 1.0
                    border.color: inPortItem.isCompatible && graph.isDragging ? "#ffffff" : graph.getPortColor(inPortItem.portTypeId)
                    border.width: inPortItem.isCompatible && graph.isDragging ? 2 : 1
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    x: portSz * 1.5 + 3
                    text: graph.getPortCaption(root.nodeId, 0, index) || (inPorts === 1 ? "Input" : String.fromCharCode(65 + index))
                    font.pixelSize: 10
                    font.family: "Poppins"
                    color: Qt.rgba(0, 0, 0, 0.7)
                }

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: -4
                    hoverEnabled: true
                    preventStealing: true
                    onEntered: { if (!graph.isDragging) { inPortCircle.scale = 1.2; graph.setActivePort({nodeId: root.nodeId, portType: 0, portIndex: index}) } }
                    onExited: { if (!graph.isDragging) { inPortCircle.scale = 1.0; graph.setActivePort(null) } }
                    property bool isActive: { var ap = graph.activePort; return ap && ap.nodeId === root.nodeId && ap.portType === 0 && ap.portIndex === index }
                    onIsActiveChanged: inPortCircle.scale = isActive ? 1.4 : 1.0
                    onPressed: (mouse) => {
                        var existing = graph.graphModel.getConnectionAtInput(root.nodeId, index)
                        var mousePos = inPortItem.mapToItem(graph.canvas, inPortItem.width/2, inPortItem.height/2)
                        if (existing.valid) {
                            graph.graphModel.removeConnection(existing.outNodeId, existing.outPortIndex, root.nodeId, index)
                            graph.connectionVersion++
                            var sourceNode = graph.nodeItems[existing.outNodeId]
                            var sourcePos = sourceNode.getPortPos(1, existing.outPortIndex)
                            graph.startDraftConnection(existing.outNodeId, 1, existing.outPortIndex, sourcePos)
                            graph.updateDraftConnection(mousePos)
                        } else {
                            var pos = inPortItem.mapToItem(graph.canvas, inPortItem.width/2, inPortItem.height/2)
                            graph.startDraftConnection(root.nodeId, 0, index, pos)
                        }
                    }
                    onPositionChanged: (mouse) => { graph.updateDraftConnection(mapToItem(graph.canvas, mouse.x, mouse.y)) }
                    onReleased: graph.endDraftConnection()
                }
            }
        }
    }

    // =================================================================
    // Output Ports
    // =================================================================
    Column {
        id: outPortsColumn
        z: 10
        x: root.width - portSz / 2 - 1
        y: headerHeight + 5
        spacing: portSpacing

        Repeater {
            id: outRepeater
            model: outPorts
            delegate: Item {
                id: outPortItem
                width: portSz
                height: portSz
                property string portTypeId: graph.getPortTypeId(root.nodeId, 1, index)
                property bool isCompatible: !graph.isDragging ||
                    (graph.activeConnectionStart && graph.activeConnectionStart.portType === 0 &&
                     graph.draftConnectionTypeId === portTypeId)
                property bool isDimmed: graph.isDragging && !isCompatible
                property bool isConnected: { graph.connectionVersion; return graph.graphModel.portConnected(root.nodeId, 1, index) }

                Rectangle {
                    id: outPortCircle
                    width: outPortItem.isConnected ? portSz : 4
                    height: width
                    radius: width / 2
                    anchors.centerIn: parent
                    antialiasing: true
                    color: outPortItem.isConnected ? graph.getPortColor(outPortItem.portTypeId) : "transparent"
                    opacity: outPortItem.isDimmed ? (style ? style.portDimmedOpacity : 0.3) : 1.0
                    border.color: outPortItem.isCompatible && graph.isDragging ? "#ffffff" : graph.getPortColor(outPortItem.portTypeId)
                    border.width: outPortItem.isCompatible && graph.isDragging ? 2 : 1
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    x: -portSz / 2 - 3 - contentWidth
                    text: graph.getPortCaption(root.nodeId, 1, index) || (outPorts === 1 ? "Output" : "Out " + index)
                    font.pixelSize: 10
                    font.family: "Poppins"
                    color: Qt.rgba(0, 0, 0, 0.7)
                }

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: -4
                    hoverEnabled: true
                    preventStealing: true
                    onEntered: { if (!graph.isDragging) { outPortCircle.scale = 1.2; graph.setActivePort({nodeId: root.nodeId, portType: 1, portIndex: index}) } }
                    onExited: { if (!graph.isDragging) { outPortCircle.scale = 1.0; graph.setActivePort(null) } }
                    property bool isActive: { var ap = graph.activePort; return ap && ap.nodeId === root.nodeId && ap.portType === 1 && ap.portIndex === index }
                    onIsActiveChanged: outPortCircle.scale = isActive ? 1.4 : 1.0
                    onPressed: (mouse) => {
                        var pos = outPortItem.mapToItem(graph.canvas, outPortItem.width/2, outPortItem.height/2)
                        graph.startDraftConnection(root.nodeId, 1, index, pos)
                    }
                    onPositionChanged: (mouse) => { graph.updateDraftConnection(mapToItem(graph.canvas, mouse.x, mouse.y)) }
                    onReleased: graph.endDraftConnection()
                }
            }
        }
    }

    // =================================================================
    // Interaction handlers
    // =================================================================
    TapHandler {
        onTapped: (eventPoint, button) => {
            graph.forceActiveFocus()
            var additive = eventPoint && eventPoint.event ? (eventPoint.event.modifiers & Qt.ControlModifier) : false
            if (additive) graph.toggleNodeSelection(nodeId)
            else graph.selectNode(nodeId, false)
        }
        onDoubleTapped: (eventPoint, button) => {
            if (graph && graph.nodeDoubleClicked) graph.nodeDoubleClicked(nodeId, nodeType, delegateModel, caption)
        }
    }

    PointHandler {
        acceptedButtons: Qt.LeftButton
        onActiveChanged: {
            if (active) {
                graph.forceActiveFocus()
                var additive = (point.modifiers & Qt.ControlModifier)
                if (additive) graph.toggleNodeSelection(nodeId)
                else if (!root.selected) graph.selectNode(nodeId, false)
            }
        }
    }

    DragHandler {
        id: dragHandler
        target: root
        property point lastPos: Qt.point(0, 0)
        property bool isDraggingGroup: false
        onActiveChanged: {
            if (active) {
                graph.bringToFront(root)
                lastPos = Qt.point(root.x, root.y)
                isDraggingGroup = root.selected && Object.keys(graph.selectedNodeIds).length > 1
            }
        }
        onTranslationChanged: {
            if (isDraggingGroup) {
                var dx = root.x - lastPos.x
                var dy = root.y - lastPos.y
                var ids = graph.getSelectedNodeIds()
                for (var i = 0; i < ids.length; i++) {
                    if (ids[i] !== nodeId) {
                        var n = graph.nodeItems[ids[i]]
                        if (n) { n.x += dx; n.y += dy }
                    }
                }
                lastPos = Qt.point(root.x, root.y)
            }
        }
    }

    // =================================================================
    // Port utility functions
    // =================================================================
    function getPortInfoAt(x, y) {
        var inPos = inPortsColumn.mapFromItem(root, x, y)
        var inChild = inPortsColumn.childAt(inPos.x, inPos.y)
        if (inChild) {
            for (var i = 0; i < inRepeater.count; ++i) {
                if (inRepeater.itemAt(i) === inChild) return {nodeId: root.nodeId, portType: 0, portIndex: i}
            }
        }
        var outPos = outPortsColumn.mapFromItem(root, x, y)
        var outChild = outPortsColumn.childAt(outPos.x, outPos.y)
        if (outChild) {
            for (var j = 0; j < outRepeater.count; ++j) {
                if (outRepeater.itemAt(j) === outChild) return {nodeId: root.nodeId, portType: 1, portIndex: j}
            }
        }
        return null
    }

    function getPortPos(type, index) {
        var repeater = (type === 0) ? inRepeater : outRepeater
        var column = (type === 0) ? inPortsColumn : outPortsColumn
        var portItem = repeater.itemAt(index)
        if (portItem) {
            return root.mapToItem(root.parent,
                column.x + portItem.x + portItem.width/2,
                column.y + portItem.y + portItem.height/2)
        }
        return Qt.point(x, y)
    }
}

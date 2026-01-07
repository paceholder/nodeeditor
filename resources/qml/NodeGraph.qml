import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15
import QtNodes 1.0

Item {
    id: root
    property QuickGraphModel graphModel
    property alias canvas: canvas

    property var nodeItems: ({})
    property Component nodeContentDelegate // User provided content
    
    // Style - can be overridden by user
    property NodeGraphStyle style: NodeGraphStyle {}
    
    function getPortColor(typeId) {
        return style.getPortColor(typeId)
    }
    
    function getPortTypeId(nodeId, portType, portIndex) {
        if (!graphModel) return "default"
        return graphModel.getPortDataTypeId(nodeId, portType, portIndex) || "default"
    }

    function registerNode(id, item) {
        nodeItems[id] = item
        nodeRegistryChanged()
    }
    
    signal nodeRegistryChanged()
    signal nodeDoubleClicked(int nodeId, string nodeType, var delegateModel)

    // Zoom and Pan
    property real zoomLevel: 1.0
    property point panOffset: Qt.point(0, 0)
    
    // Port dragging
    property var activePort: null
    
    // Z-order management
    property int topZ: 1
    function bringToFront(nodeItem) {
        topZ++
        nodeItem.z = topZ
    }
    
    // Selection management
    property var selectedNodeIds: ({})
    property int selectionVersion: 0
    
    signal selectionChanged()
    
    function isNodeSelected(nodeId) {
        return selectedNodeIds.hasOwnProperty(nodeId)
    }
    
    function selectNode(nodeId, additive) {
        if (!additive) {
            selectedNodeIds = {}
            clearConnectionSelection()
        }
        if (!selectedNodeIds.hasOwnProperty(nodeId)) {
            selectedNodeIds[nodeId] = true
            selectionVersion++
            selectionChanged()
        }
    }
    
    function deselectNode(nodeId) {
        if (selectedNodeIds.hasOwnProperty(nodeId)) {
            delete selectedNodeIds[nodeId]
            selectionVersion++
            selectionChanged()
        }
    }
    
    function toggleNodeSelection(nodeId) {
        if (selectedNodeIds.hasOwnProperty(nodeId)) {
            delete selectedNodeIds[nodeId]
        } else {
            selectedNodeIds[nodeId] = true
        }
        selectionVersion++
        selectionChanged()
    }
    
    function clearSelection() {
        selectedNodeIds = {}
        selectionVersion++
        selectionChanged()
        clearConnectionSelection()
    }
    
    function selectNodesInRect(rect) {
        for (var id in nodeItems) {
            var node = nodeItems[id]
            if (node) {
                var nodeRect = Qt.rect(node.x, node.y, node.width, node.height)
                if (rectsIntersect(rect, nodeRect)) {
                    selectedNodeIds[id] = true
                }
            }
        }
        selectionVersion++
        selectionChanged()
    }
    
    function selectConnectionsInRect(rect) {
        if (!graphModel || !graphModel.connections) return
        
        var connModel = graphModel.connections
        for (var i = 0; i < connModel.rowCount(); i++) {
            var idx = connModel.index(i, 0)
            var srcNodeId = connModel.data(idx, 258) // SourceNodeIdRole
            var srcPortIdx = connModel.data(idx, 259) // SourcePortIndexRole
            var dstNodeId = connModel.data(idx, 260) // DestNodeIdRole
            var dstPortIdx = connModel.data(idx, 261) // DestPortIndexRole
            
            var srcNode = nodeItems[srcNodeId]
            var dstNode = nodeItems[dstNodeId]
            
            if (srcNode && dstNode && srcNode.completed && dstNode.completed) {
                var startPos = srcNode.getPortPos(1, srcPortIdx)
                var endPos = dstNode.getPortPos(0, dstPortIdx)
                
                if (curveIntersectsRect(startPos, endPos, rect)) {
                    selectedConnections.push({
                        outNodeId: srcNodeId,
                        outPortIndex: srcPortIdx,
                        inNodeId: dstNodeId,
                        inPortIndex: dstPortIdx
                    })
                }
            }
        }
        connectionSelectionChanged()
    }
    
    function curveIntersectsRect(startPos, endPos, rect) {
        var cp1x = startPos.x + Math.abs(endPos.x - startPos.x) * 0.5
        var cp1y = startPos.y
        var cp2x = endPos.x - Math.abs(endPos.x - startPos.x) * 0.5
        var cp2y = endPos.y
        
        for (var t = 0; t <= 1; t += 0.05) {
            var u = 1 - t
            var bx = u*u*u*startPos.x + 3*u*u*t*cp1x + 3*u*t*t*cp2x + t*t*t*endPos.x
            var by = u*u*u*startPos.y + 3*u*u*t*cp1y + 3*u*t*t*cp2y + t*t*t*endPos.y
            
            if (bx >= rect.x && bx <= rect.x + rect.width &&
                by >= rect.y && by <= rect.y + rect.height) {
                return true
            }
        }
        return false
    }
    
    function rectsIntersect(r1, r2) {
        return !(r2.x > r1.x + r1.width ||
                 r2.x + r2.width < r1.x ||
                 r2.y > r1.y + r1.height ||
                 r2.y + r2.height < r1.y)
    }
    
    function getSelectedNodeIds() {
        return Object.keys(selectedNodeIds).map(function(id) { return parseInt(id) })
    }
    
    // Connection selection management
    property var selectedConnections: []
    
    signal connectionSelectionChanged()
    
    function isConnectionSelected(outNodeId, outPortIndex, inNodeId, inPortIndex) {
        for (var i = 0; i < selectedConnections.length; i++) {
            var c = selectedConnections[i]
            if (c.outNodeId === outNodeId && c.outPortIndex === outPortIndex &&
                c.inNodeId === inNodeId && c.inPortIndex === inPortIndex) {
                return true
            }
        }
        return false
    }
    
    function selectConnection(outNodeId, outPortIndex, inNodeId, inPortIndex, additive) {
        if (!additive) {
            selectedConnections = []
            clearSelection()
        }
        selectedConnections.push({
            outNodeId: outNodeId,
            outPortIndex: outPortIndex,
            inNodeId: inNodeId,
            inPortIndex: inPortIndex
        })
        connectionSelectionChanged()
    }
    
    function clearConnectionSelection() {
        selectedConnections = []
        connectionSelectionChanged()
    }
    
    function deleteSelectedConnections() {
        for (var i = 0; i < selectedConnections.length; i++) {
            var c = selectedConnections[i]
            graphModel.removeConnection(c.outNodeId, c.outPortIndex, c.inNodeId, c.inPortIndex)
        }
        selectedConnections = []
        connectionSelectionChanged()
    }
    
    function deleteSelectedNodes() {
        var ids = getSelectedNodeIds()
        for (var i = 0; i < ids.length; i++) {
            graphModel.removeNode(ids[i])
            delete nodeItems[ids[i]]
        }
        clearSelection()
    }
    
    function deleteSelected() {
        deleteSelectedConnections()
        deleteSelectedNodes()
    }
    
    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Delete || event.key === Qt.Key_Backspace || event.key === Qt.Key_X) {
            deleteSelected()
            event.accepted = true
        } else if (event.key === Qt.Key_Z && (event.modifiers & Qt.ControlModifier)) {
            if (event.modifiers & Qt.ShiftModifier) {
                if (graphModel) graphModel.redo()
            } else {
                if (graphModel) graphModel.undo()
            }
            event.accepted = true
        } else if (event.key === Qt.Key_Y && (event.modifiers & Qt.ControlModifier)) {
            if (graphModel) graphModel.redo()
            event.accepted = true
        }
    }
    
    focus: true
    
    // Marquee selection
    property bool isMarqueeSelecting: false
    property point marqueeStart: Qt.point(0, 0)
    property point marqueeEnd: Qt.point(0, 0)
    
    // Temporary drafting connection
    property point dragStart: Qt.point(0, 0)
    property point dragCurrent: Qt.point(0, 0)
    property bool isDragging: false
    
    function startDraftConnection(nodeId, portType, portIndex, pos) {
        dragStart = pos
        dragCurrent = pos
        isDragging = true
        activeConnectionStart = {nodeId: nodeId, portType: portType, portIndex: portIndex}
        draftConnectionTypeId = getPortTypeId(nodeId, portType, portIndex)
    }
    
    property string draftConnectionTypeId: ""
    
    property var activeConnectionStart: null
    
    function updateDraftConnection(pos) {
        dragCurrent = pos
        
        // Hit testing for potential target port
        // Use geometry-based search instead of childAt to avoid z-ordering issues with the drag line itself
        var targetNode = null
        
        for (var id in nodeItems) {
            var node = nodeItems[id]
            // nodeItems is a map, check if node is valid
            if (node && node.visible) {
                // Map canvas pos to node local
                var localPos = node.mapFromItem(canvas, pos.x, pos.y)
                if (node.contains(Qt.point(localPos.x, localPos.y))) {
                    targetNode = node
                    break
                }
            }
        }
        
        if (targetNode && typeof targetNode.getPortInfoAt === 'function') {
            var nodeLocalPos = canvas.mapToItem(targetNode, pos.x, pos.y)
            var portInfo = targetNode.getPortInfoAt(nodeLocalPos.x, nodeLocalPos.y)
            setActivePort(portInfo)
        } else {
            setActivePort(null)
        }
    }
    
    function endDraftConnection() {
        isDragging = false
        draftConnectionTypeId = ""
        if (activePort && activeConnectionStart) {
            var start = activeConnectionStart
            var end = activePort
            
            var outNodeId, outPortIndex, inNodeId, inPortIndex
            
            // Determine Out -> In direction
            if (start.portType === 1 && end.portType === 0) {
                outNodeId = start.nodeId
                outPortIndex = start.portIndex
                inNodeId = end.nodeId
                inPortIndex = end.portIndex
            } else if (start.portType === 0 && end.portType === 1) {
                outNodeId = end.nodeId
                outPortIndex = end.portIndex
                inNodeId = start.nodeId
                inPortIndex = start.portIndex
            } else {
                activeConnectionStart = null
                return
            }
            
            // Only create connection if types are compatible
            if (graphModel.connectionPossible(outNodeId, outPortIndex, inNodeId, inPortIndex)) {
                graphModel.addConnection(outNodeId, outPortIndex, inNodeId, inPortIndex)
            }
        }
        activeConnectionStart = null
    }
    
    function setActivePort(portInfo) {
        activePort = portInfo
    }

    Rectangle {
        anchors.fill: parent
        color: style.canvasBackground
        clip: true

        // Input Handler for Pan/Zoom/Selection
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.MiddleButton | Qt.LeftButton
            property point lastPos: Qt.point(0, 0)

            onPressed: (mouse) => {
                root.forceActiveFocus()
                lastPos = Qt.point(mouse.x, mouse.y)
                
                // Left click without Alt starts marquee selection
                if (mouse.button === Qt.LeftButton && !(mouse.modifiers & Qt.AltModifier)) {
                    // Convert screen position to canvas coordinates
                    var canvasPos = Qt.point(
                        (mouse.x - root.panOffset.x) / root.zoomLevel,
                        (mouse.y - root.panOffset.y) / root.zoomLevel
                    )
                    root.marqueeStart = canvasPos
                    root.marqueeEnd = canvasPos
                    root.isMarqueeSelecting = true
                    
                    // Clear selection unless Ctrl is held
                    if (!(mouse.modifiers & Qt.ControlModifier)) {
                        root.clearSelection()
                    }
                }
            }

            onPositionChanged: (mouse) => {
                if (pressedButtons & Qt.MiddleButton || (pressedButtons & Qt.LeftButton && (mouse.modifiers & Qt.AltModifier))) {
                    var delta = Qt.point(mouse.x - lastPos.x, mouse.y - lastPos.y)
                    root.panOffset = Qt.point(root.panOffset.x + delta.x, root.panOffset.y + delta.y)
                    lastPos = Qt.point(mouse.x, mouse.y)
                } else if (root.isMarqueeSelecting) {
                    var canvasPos = Qt.point(
                        (mouse.x - root.panOffset.x) / root.zoomLevel,
                        (mouse.y - root.panOffset.y) / root.zoomLevel
                    )
                    root.marqueeEnd = canvasPos
                }
            }
            
            onReleased: (mouse) => {
                if (root.isMarqueeSelecting) {
                    // Select nodes and connections in marquee rect
                    var x = Math.min(root.marqueeStart.x, root.marqueeEnd.x)
                    var y = Math.min(root.marqueeStart.y, root.marqueeEnd.y)
                    var w = Math.abs(root.marqueeEnd.x - root.marqueeStart.x)
                    var h = Math.abs(root.marqueeEnd.y - root.marqueeStart.y)
                    
                    if (w > 5 || h > 5) {
                        var rect = Qt.rect(x, y, w, h)
                        root.selectNodesInRect(rect)
                        root.selectConnectionsInRect(rect)
                    }
                    root.isMarqueeSelecting = false
                }
            }

            onWheel: (wheel) => {
                var zoomFactor = 1.1
                var oldZoom = zoomLevel
                var newZoom = (wheel.angleDelta.y < 0) ? oldZoom / zoomFactor : oldZoom * zoomFactor
                
                // Clamp
                newZoom = Math.max(0.1, Math.min(5.0, newZoom))
                
                // Mouse position on screen
                var mouseX = wheel.x
                var mouseY = wheel.y
                
                // Position in canvas (world coordinates)
                var canvasX = (mouseX - panOffset.x) / oldZoom
                var canvasY = (mouseY - panOffset.y) / oldZoom
                
                // Adjust pan to keep point fixed under mouse
                panOffset = Qt.point(
                    mouseX - canvasX * newZoom,
                    mouseY - canvasY * newZoom
                )
                
                zoomLevel = newZoom
            }
        }

        // Grid Shader
        /*
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
        */
        
        // Canvas Grid
        Canvas {
             id: gridCanvas
             anchors.fill: parent
             property real zoom: root.zoomLevel
             property point offset: root.panOffset
             property color minorColor: style.gridMinorLine
             property color majorColor: style.gridMajorLine
             property real minorSpacing: style.gridMinorSpacing
             property real majorSpacing: style.gridMajorSpacing
             
             onZoomChanged: requestPaint()
             onOffsetChanged: requestPaint()
             onMinorColorChanged: requestPaint()
             onMajorColorChanged: requestPaint()
             
             onPaint: {
                 var ctx = getContext("2d")
                 ctx.clearRect(0, 0, width, height)
                 
                 ctx.lineWidth = 1
                 
                 var gridSize = minorSpacing * zoom
                 var majorGridSize = majorSpacing * zoom
                 
                 var startX = (offset.x % gridSize)
                 var startY = (offset.y % gridSize)
                 
                 if (startX < 0) startX += gridSize
                 if (startY < 0) startY += gridSize
                 
                 // Minor lines
                 ctx.strokeStyle = minorColor
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
                 ctx.strokeStyle = majorColor
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
            sourceNodeId: model.sourceNodeId
            sourcePortIndex: model.sourcePortIndex
            destNodeId: model.destNodeId
            destPortIndex: model.destPortIndex
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
                    delegateModel: model.delegateModel // The C++ QObject*
                    contentDelegate: root.nodeContentDelegate
                    
                    onXChanged: {
                         if (completed && Math.abs(x - initialX) > 0.1) graphModel.nodes.moveNode(nodeId, x, y)
                    }
                    onYChanged: {
                         if (completed && Math.abs(y - initialY) > 0.1) graphModel.nodes.moveNode(nodeId, x, y)
                    }
                    
                    Component.onCompleted: {
                        console.log("Node created. ID:", nodeId, "Caption:", caption, "In:", inPorts, "Out:", outPorts)
                        root.registerNode(nodeId, nodeDelegate)
                    }
                }
            }
            
    // Dragging Connection
    Shape {
        visible: root.isDragging
        ShapePath {
            strokeWidth: style.draftConnectionWidth
            strokeColor: style.draftConnectionColor
            fillColor: "transparent"
            startX: root.dragStart.x
            startY: root.dragStart.y
            PathCubic {
                x: root.dragCurrent.x
                y: root.dragCurrent.y
                control1X: root.dragStart.x + Math.abs(root.dragCurrent.x - root.dragStart.x) * 0.5
                control1Y: root.dragStart.y
                control2X: root.dragCurrent.x - Math.abs(root.dragCurrent.x - root.dragStart.x) * 0.5
                control2Y: root.dragCurrent.y
            }
        }
    }
    
    // Marquee Selection Rectangle
    Rectangle {
        visible: root.isMarqueeSelecting
        x: Math.min(root.marqueeStart.x, root.marqueeEnd.x)
        y: Math.min(root.marqueeStart.y, root.marqueeEnd.y)
        width: Math.abs(root.marqueeEnd.x - root.marqueeStart.x)
        height: Math.abs(root.marqueeEnd.y - root.marqueeStart.y)
        color: style.selectionRectFill
        border.color: style.selectionRectBorder
        border.width: style.selectionRectBorderWidth
    }
    }
    }
    }

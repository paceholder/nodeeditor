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

    function registerNode(id, item) {
        nodeItems[id] = item
        nodeRegistryChanged()
    }
    
    signal nodeRegistryChanged()

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
    
    function rectsIntersect(r1, r2) {
        return !(r2.x > r1.x + r1.width ||
                 r2.x + r2.width < r1.x ||
                 r2.y > r1.y + r1.height ||
                 r2.y + r2.height < r1.y)
    }
    
    function getSelectedNodeIds() {
        return Object.keys(selectedNodeIds).map(function(id) { return parseInt(id) })
    }
    
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
    }
    
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
        if (activePort && activeConnectionStart) {
            // Check if connecting Out -> In or In -> Out
            var start = activeConnectionStart
            var end = activePort
            
            // We only allow Out -> In connection creation in this simple logic
            // If drag started from Out (1) and ended at In (0)
            if (start.portType === 1 && end.portType === 0) {
                graphModel.addConnection(start.nodeId, start.portIndex, end.nodeId, end.portIndex)
            }
            // If drag started from In (0) and ended at Out (1) - usually we drag from source to dest
            else if (start.portType === 0 && end.portType === 1) {
                graphModel.addConnection(end.nodeId, end.portIndex, start.nodeId, start.portIndex)
            }
        }
        activeConnectionStart = null
    }
    
    function setActivePort(portInfo) {
        activePort = portInfo
    }

    Rectangle {
        anchors.fill: parent
        color: "#2b2b2b"
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
                    // Select nodes in marquee rect
                    var x = Math.min(root.marqueeStart.x, root.marqueeEnd.x)
                    var y = Math.min(root.marqueeStart.y, root.marqueeEnd.y)
                    var w = Math.abs(root.marqueeEnd.x - root.marqueeStart.x)
                    var h = Math.abs(root.marqueeEnd.y - root.marqueeStart.y)
                    
                    if (w > 5 || h > 5) {
                        root.selectNodesInRect(Qt.rect(x, y, w, h))
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
            strokeWidth: 2
            strokeColor: "orange"
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
        color: "#224a9eff"
        border.color: "#4a9eff"
        border.width: 1
    }
    }
    }
    }

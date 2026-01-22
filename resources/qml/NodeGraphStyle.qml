import QtQuick 2.15

QtObject {
    // Canvas
    property color canvasBackground: "#2b2b2b"
    property color gridMinorLine: "#353535"
    property color gridMajorLine: "#151515"
    property real gridMinorSpacing: 20
    property real gridMajorSpacing: 100
    
    // Node
    property color nodeBackground: "#2d2d2d"
    property color nodeBorder: "black"
    property color nodeSelectedBorder: "#4a9eff"
    property real nodeBorderWidth: 2
    property real nodeSelectedBorderWidth: 3
    property real nodeRadius: 5
    property color nodeCaptionColor: "#eeeeee"
    property int nodeCaptionFontSize: 12
    property bool nodeCaptionBold: true
    property real nodeMinWidth: 150
    property real nodePortSpacing: 10
    property real nodeHeaderHeight: 35
    property color nodeContentColor: "#ffffff"
    
    // Ports
    property real portSize: 12
    property real portBorderWidth: 1
    property color portBorderColor: "black"
    property color portHighlightBorder: "#ffffff"
    property real portHighlightBorderWidth: 2
    property real portHoverScale: 1.2
    property real portActiveScale: 1.4
    property real portDimmedOpacity: 0.3
    
    // Port type colors
    property var portTypeColors: ({
        "decimal": "#4CAF50",
        "integer": "#2196F3",
        "string": "#FF9800",
        "boolean": "#9C27B0",
        "default": "#9E9E9E"
    })
    
    // Connection
    property real connectionWidth: 3
    property real connectionHoverWidth: 3.5
    property real connectionSelectedWidth: 4
    property color connectionSelectionOutline: "#4a9eff"
    property real connectionSelectionOutlineWidth: 7
    
    // Draft connection
    property real draftConnectionWidth: 2
    property color draftConnectionColor: "orange"
    
    // Selection
    property color selectionRectFill: "#224a9eff"
    property color selectionRectBorder: "#4a9eff"
    property real selectionRectBorderWidth: 1
    
    // Helper function
    function getPortColor(typeId) {
        return portTypeColors[typeId] || portTypeColors["default"]
    }
}

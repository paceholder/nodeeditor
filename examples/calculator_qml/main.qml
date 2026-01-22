import QtQuick 2.0
import QtQml
import QtQuick.Window
import bk

Window {
    width: 1500
    height: 1000
    visible: true
    Text {
        id: myLabel
        width: 200
        height: 200
        text: "Label to show that zooming with mouse is broken\nif node editor is not a (0, 0) position."
    }
    NodeEditor {
        id: qmlwrap
        width: 800
        height: 800
        anchors.top: myLabel.bottom
        anchors.left: myLabel.right
    }
}

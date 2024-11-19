import QtQuick
import QtQuick.Window
import QtCharts
import bk

Window {
    width: 1500
    height: 1000
    visible: true

    Button {
        id: button
        width: 200
        height: 200
        anchors.top: myChart.bottom
        text: qsTr("Click Me!")
        onClicked: {
            backend.setObject(button);
        }
    }
    WidgetView {
        id: qmlwrap
        width: 800
        height: 800
        anchors.top: myChart.bottom
        anchors.left: myChart.right
    }
}

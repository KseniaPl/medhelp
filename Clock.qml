import QtQuick 2.4

Item {
    id:clocktext
    property int hours
    property int minutes
    property int seconds
    // можно использовать для цветовой гаммы
    property bool night: false

    function timeChanged() {
        var date = new Date;
        hours = date.getHours()
        night = ( hours < 7 || hours > 19 )
        minutes = date.getMinutes()
        seconds = date.getSeconds()
    }

    function sizeChanged() {
        cityLabel.font.pixelSize = Math.round(parent.width / 7);
    }

    Timer {
        interval: 100; running: true; repeat: true;
        onTriggered: clocktext.timeChanged()
    }

    Text
    {
        id: cityLabel
        anchors.fill: parent
        color: "white"
        font.family: font_clock.name
        font.bold: true;
        font.italic: true;
        font.pixelSize: {
            // 8symbols in time. -> 16 in a row. 16xSize=parent.width
            Math.round(parent.width / 7)
        }

        style: Text.Raised;
        styleColor: "black"
        text: hours.toString() + ":"  +( minutes < 10 ? "0" : "") + minutes.toString() + ":" + (seconds < 10 ? "0" : "") + seconds.toString();
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

}

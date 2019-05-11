import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    id: backgroundItem
    width: 768
    height: 768
    transformOrigin: Item.Center
    clip: false

    Rectangle {
        id: rectangle1
        width: backgroundItem.width - 5
        height: backgroundItem.height - 5
        color: "#000000"
        transformOrigin: Item.Center
    }
    Grid {
        columns: Math.sqrt((rectangle1.width*rectangle1.height)/9)
    }

}

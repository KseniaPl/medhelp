import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import Settings 1.0
import QtMultimedia 5.4

Item {
    property int countdown: 5
    property int maxcountdown: 5
    id: itemBase
    Settings {
        id: settings
    }
    Rectangle {
        id: rectangle2
        width: parent.width
        height: parent.height
        color: "#000000"
        transformOrigin: Item.Center

        MouseArea {
            anchors.fill: parent
            onClicked: {
//                rectangle2.opacity = 0;
//                clear();
                //   stackView2.push(Qt.resolvedUrl("main.qml"))
            }
            Behavior on opacity { NumberAnimation { duration: 300 } }
        }
    }

    Text {
        id: textSettings
        color: "#ffffff"
        font.pixelSize: 24
        font { family: "Segoe UI" }
        Behavior on opacity { NumberAnimation { duration: 700; } }
        text: "Настройки"
        anchors {
            horizontalCenter: rectangle2.horizontalCenter;
            top: rectangle2.top;
            margins: rectangle2.height/12
        }
    }

    Column {
        id: radioButtons_Calibration
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: rectangle2.width/20
        anchors.rightMargin: spacing
        anchors.topMargin: rectangle2.height/7
        spacing: 10
        Text {
            color: "#d7d7d7"
            font.pixelSize: 20
            font { family: "Segoe UI" }
            text: "Тип калибровки"
        }
        ExclusiveGroup { id: tabPositionGroup }
        RadioButton {
            id: radiobuttonmotion
            checked:
            {
                if (settings.getCalibrationType() === 0)
                    return true;
                return false;
            }
            exclusiveGroup: tabPositionGroup
            style: RadioButtonStyle {
                label: Text {
                    color: "white"
                    font { family: "Segoe UI" }
                    text: "Движение шарика"
                    font.pixelSize: 18;
                }
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 4
                    border.color: control.activeFocus ? "darkblue" : "gray"
                    border.width: 1
                    color: "#ffffff"
                    Rectangle {
                        visible: control.checked
                        color: "#1dff00"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    settings.setCalibrationType(0);
                    radiobuttonmotion.checked = true;
                }
            }

        }
        RadioButton {
            id: radiobuttonEmersion
            checked:
            {
                if (settings.getCalibrationType() === 1)
                    return true;
                return false;
            }
            exclusiveGroup: tabPositionGroup
            style: RadioButtonStyle {
                label: Text {
                    color: "white"
                    font { family: "Segoe UI" }
                    text: "Появление шарика"
                    font.pixelSize: 18
                }
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 4
                    border.color: control.activeFocus ? "darkblue" : "gray"
                    border.width: 1
                    color: "#ffffff"
                    Rectangle {
                        visible: control.checked
                        color: "#1dff00"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    settings.setCalibrationType(1);
                    radiobuttonEmersion.checked = true;
                }
            }

        }
    }


    Column {
        id: radioButtons_FlipImage
        anchors.top: radioButtons_Calibration.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: rectangle2.width/20
        anchors.rightMargin: spacing
        anchors.topMargin: 30
        spacing: 10
        Text {
            color: "#d7d7d7"
            font.pixelSize: 20
            font { family: "Segoe UI" }
            text: "Поворот кадра"
        }
        ExclusiveGroup { id: radioButtons_FlipImageGroup }

        RadioButton {
            id: radioButton_0
            checked:
            {
                if (settings.getFlipImage() === 0)
                    return true;
                return false;
            }
            exclusiveGroup: radioButtons_FlipImageGroup
            style: RadioButtonStyle {
                label: Text {
                    color: "white"
                    font { family: "Segoe UI" }
                    text: "Без поворота"
                    font.pixelSize: 18;
                }
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 4
                    border.color: control.activeFocus ? "darkblue" : "gray"
                    border.width: 1
                    color: "#ffffff"
                    Rectangle {
                        visible: control.checked
                        color: "#ff0000"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    settings.setFlipImage(0);
                    radioButton_0.checked = true;
                }
            }
        }

        RadioButton {
            id: radioButton_90
            checked:
            {
                if (settings.getFlipImage() === 1)
                    return true;
                return false;
            }
            exclusiveGroup: radioButtons_FlipImageGroup
            style: RadioButtonStyle {
                label: Text {
                    color: "white"
                    font { family: "Segoe UI" }
                    text: "90 градусов"
                    font.pixelSize: 18;
                }
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 4
                    border.color: control.activeFocus ? "darkblue" : "gray"
                    border.width: 1
                    color: "#ffffff"
                    Rectangle {
                        visible: control.checked
                        color: "#ff0000"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    settings.setFlipImage(1);
                    radioButton_90.checked = true;
                }
            }
        }
        RadioButton {
            id: radioButton_180
            checked:
            {
                if (settings.getFlipImage() === 2)
                    return true;
                return false;
            }
            exclusiveGroup: radioButtons_FlipImageGroup
            style: RadioButtonStyle {
                label: Text {
                    color: "white"
                    font { family: "Segoe UI" }
                    text: "180 градусов"
                    font.pixelSize: 18
                }
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 4
                    border.color: control.activeFocus ? "darkblue" : "gray"
                    border.width: 1
                    color: "#ffffff"
                    Rectangle {
                        visible: control.checked
                        color: "#ff0000"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    settings.setFlipImage(2);
                    radioButton_180.checked = true;
                }
            }
        }

        RadioButton {
            id: radioButton_m90
            checked:
            {
                if (settings.getFlipImage() === 3)
                    return true;
                return false;
            }
            exclusiveGroup: radioButtons_FlipImageGroup
            style: RadioButtonStyle {
                label: Text {
                    color: "white"
                    font { family: "Segoe UI" }
                    text: "-90 градусов"
                    font.pixelSize: 18
                }
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 4
                    border.color: control.activeFocus ? "darkblue" : "gray"
                    border.width: 1
                    color: "#ffffff"
                    Rectangle {
                        visible: control.checked
                        color: "#ff0000"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    settings.setFlipImage(3);
                    radioButton_m90.checked = true;
                }
            }
        }
    }

    Column {
        id: other_settings
        anchors.top: radioButtons_FlipImage.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: rectangle2.width/20
        anchors.rightMargin: spacing
        anchors.topMargin: 30
        spacing: 10

        Text {
            color: "#d7d7d7"
            font.pixelSize: 20
            font { family: "Segoe UI" }
            text: "Другие настройки"
        }

        CheckBox {
            id: сheckBox_showImage
            checked: settings.getShowImage()

            //exclusiveGroup: tabPositionGroup
            style: CheckBoxStyle {
                label: Text {
                    color: "white"
                    font { family: "Segoe UI" }
                    text: "Показать изображение"
                    font.pixelSize: 18
                }
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 4
                    border.color: control.activeFocus ? "darkblue" : "gray"
                    border.width: 1
                    color: "#ffffff"
                    Rectangle {
                        visible: control.checked
                        color: "#005dff"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    if (сheckBox_showImage.checked)
                    {
                        settings.setShowImage(false);
                        сheckBox_showImage.checked = false;
                    }
                    else
                    {
                        settings.setShowImage(true);
                        сheckBox_showImage.checked = true;
                    }
                }
            }
        }
        CheckBox {
            id: сheckBox_flip_mirrorReflection
            checked: settings.getFlip_mirrorReflection()

            //exclusiveGroup: tabPositionGroup
            style: CheckBoxStyle {
                label: Text {
                    color: "white"
                    font { family: "Segoe UI" }
                    text: "Зеркальное отражение камеры"
                    font.pixelSize: 18
                }
                indicator: Rectangle {
                    implicitWidth: 16
                    implicitHeight: 16
                    radius: 4
                    border.color: control.activeFocus ? "darkblue" : "gray"
                    border.width: 1
                    color: "#ffffff"
                    Rectangle {
                        visible: control.checked
                        color: "#005dff"
                        border.color: "#333"
                        radius: 1
                        anchors.margins: 4
                        anchors.fill: parent
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    if (сheckBox_flip_mirrorReflection.checked)
                    {
                        settings.setFlip_mirrorReflection(false);
                        сheckBox_flip_mirrorReflection.checked = false;
                    }
                    else
                    {
                        settings.setFlip_mirrorReflection(true);
                        сheckBox_flip_mirrorReflection.checked = true;
                    }
                }
            }
        }
    }

//    Item
//    {
//        id: check_camera
//        anchors.top: other_settings.bottom
//        anchors.left: parent.left
//        anchors.right: parent.right
//        anchors.leftMargin: rectangle2.width/20
//        anchors.rightMargin: spacing
//        anchors.topMargin: 30

//        ComboBox {
//            id: comboBox1
//            x: 258
//            y: 54
//            model: myModel
//            editable: true

//        }
//    }

//    Item
//    {
//        visible: false
//        Button {
//            //        id: button_CameraTest
//            //        anchors.top: сheckBox_showImage.bottom
//            //        anchors.left: parent.left
//            //        anchors.leftMargin: rectangle2.width/20
//            //        anchors.topMargin: 30

//            style: ButtonStyle {
//                label: Text {
//                    color: "white"
//                    text: "Тест камеры"
//                    font { family: "Segoe UI" }
//                    font.pixelSize: 12;
//                }
//                background: Rectangle {
//                    implicitWidth: 100
//                    implicitHeight: 25
//                    border.width: control.activeFocus ? 2 : 1
//                    border.color: "#888"
//                    radius: 4
//                    gradient: Gradient {
//                        GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
//                        GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
//                    }
//                }
//            }
//            MouseArea {
//                anchors.fill: parent
//                onClicked:
//                {
//                    if (button_CameraTest.style.text === "Тест камеры")
//                    {
//                        button_CameraTest.text = "Закончить тест";
//                    }
//                    else
//                    {
//                        button_CameraTest.text = "Тест камеры";
//                    }
//                }
//            }
//        }
//    }

    Camera {
        id: camera
    }

    Column {
        id: cameraListColumn
        anchors.top: other_settings.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: rectangle2.width/20
        anchors.topMargin: 30

        Text {
            id: cameraList
            color: "#d7d7d7"
            font.pixelSize: 20
            font { family: "Segoe UI" }
            text: "Список доступных камер:"
        }

        ListView {
            spacing: 10
            anchors
            {
                top: cameraList.bottom
                topMargin: 10
            }

            model: QtMultimedia.availableCameras
            delegate: Text {
                color: "white"
                font { family: "Segoe UI" }
                font.pixelSize: 18
                text: "- " +modelData.displayName// + modelData.deviceId

                MouseArea {
                    anchors.fill: parent
                    onClicked:
                    {
                        camera.deviceId = modelData.deviceId
                        selectedCameraChangedSignal(modelData.deviceId);
                    }
                }
            }
        }
    }

    Item
    {
        width: parent.width/5;
        height: parent.height/5;
        anchors {
            right: rectangle2.right
            rightMargin: 50
            top: textSettings.bottom
            topMargin: 20
            //margins: 50
        }

        VideoOutput {
            anchors.fill: parent
            source: camera
            orientation:
            {
                if (radioButton_0.checked)
                    return 0;
                if (radioButton_90.checked)
                    return 90;
                else if (radioButton_180.checked)
                    return 180;
                else if (radioButton_m90.checked)
                    return 270;
            }
        }
    }

    Text {
        id: textExitWithSaving
        color: "#ffffff"
        font.pixelSize: 24
        font { family: "Segoe UI" }
        Behavior on opacity { NumberAnimation { duration: 700; } }
        text: "Сохранить "
        anchors {
            left: parent.left
            leftMargin: rectangle2.width/20
            bottom: rectangle2.bottom;
            bottomMargin: rectangle2.height/20
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                settings.saveSettings("settings.xml");
                rectangle2.opacity = 0;
                clear();
            }
            Behavior on opacity { NumberAnimation { duration: 300 } }
        }
    }

    Text {
        id: textExitWithoutSaving
        color: "#ffffff"
        font.pixelSize: 24
        font { family: "Segoe UI" }
        Behavior on opacity { NumberAnimation { duration: 700; } }
        text: " Отмена"
        anchors {
            left: textExitWithSaving.right
            leftMargin: rectangle2.width/20
            bottom: rectangle2.bottom;
            bottomMargin: rectangle2.height/20
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                rectangle2.opacity = 0;
                clear();
            }
            Behavior on opacity { NumberAnimation { duration: 300 } }
        }
    }

}

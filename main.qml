import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.1
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Qt.labs.folderlistmodel 1.0
import QtQml.Models 2.1
import Settings 1.0
import "menu"


ApplicationWindow {
    signal mysignal();
    id: mainWindow;
    onClosing:
    {
        //fullStopSignal();
        Qt.quit();
    }
    objectName: "mainWindow"
    title: qsTr("Med Helper")
    width: 800;
    height: 800;
    visible: true;

    property int exitCountdown: 2
    property bool menu_shown: false

    property bool calibrationDone: false

    //signals to c++ (to baselogic)
    signal widthChngSignal(int value)
    signal heightChngSignal(int value)
    onWidthChanged: widthChngSignal(width)
    onHeightChanged: heightChngSignal(height)

    signal fullStopSignal()
    signal workProcessSignal()

    signal startCameraInitialisationSignal()
    signal startCalibrationSignal()
    signal stopCalibrationSignal()

    property bool ifInitCamera: false
    property bool ifCameraError: false

    //сигналы первого типа калибровки
    signal directoryChangedSignal(int direction)

    //сигналы для второго типа калибровки - появление шарика
    //signal startAnalyseEmersionRoundCalibSignal()
    signal flagEmersionRoundCalibSignal()
    signal flagStopEmersionRoundCalibSignal()
    signal getMonitorCoordCalibSignal(int x, int y)

    //изменена выбранная камера
    signal selectedCameraChangedSignal(string value)

    //сигнал начала записи для настройки детектора моргания
    signal startEyeBlinkDetectorSignal()
    signal stopEyeBlinkDetectorSignal()

    //тест на базе BioID
    signal testBioIDSignal()

    property bool flagcheckCalibrovka: false

    FontLoader {
        id: font_clock
        source: "Font_clock.ttf"
    }

    //класс Settings, импортированный из с++
    Settings {
        id: settings
    }

    Item {
        Item {
            focus: true
            Keys.onPressed: {
                if (event.key === Qt.Key_Left) {
                    console.log("Btn Left presssed");
                }
            }
        }


        id: backgroundItem
        width: parent.width
        height: parent.height
        transformOrigin: Item.Center
        clip: false
        objectName: "backgroundItem"
        opacity: stackView.depth > 1 ? 0 : 1
        Behavior on opacity
        {
            NumberAnimation { duration: 300 }
        }

        Rectangle {
            id: rectangle1
            width: backgroundItem.width
            height: backgroundItem.height
            //color: "#000000"
            transformOrigin: Item.Center
            objectName: "rectangle1"
            MouseArea {
                anchors.fill: parent
                onClicked:  if(menu_shown) { menu_shown = false; onMenu(); }
            }
        }

        Image {
            id: menuImage
            source: "Menu.bmp"
            height: rectangle1.width/16
            width: height
            //scale: 0.3
            opacity : 0.7;
            MouseArea {
                anchors.fill: parent
                anchors.margins: -10
                hoverEnabled: true
                onEntered: menuImage.opacity = 1;
                onExited: menuImage.opacity = 0.7;
                onClicked:  {
                    menu_shown = true
                    onMenu(); }
            }
            Behavior on opacity
            {
                NumberAnimation { duration: 100 }
            }
        }
        Grid {
            id: maingrid
            // columns:  ectangle1.width/Math.sqrt((rectangle1.width*rectangle1.height)/countButtons)
            // rows: columns
            //columns:Math.sqrt(countButtons)
            //rows:Math.sqrt(countButtons)
            spacing: 3
            anchors.centerIn: rectangle1
            //anchors.top: menuImage.height + 5
            //anchors.centerIn: parent

            Column {
                spacing: 3
                Image {
                    id: btn_phone
                    objectName: "btn_phone"
                    source:  "images/Telephone - Ringing-02.bmp"
                    width: getWinthImages (4)
                    height: width
                    opacity : 0.7;
                    Behavior on opacity
                    {
                        NumberAnimation { duration: 100 }
                    }
                    MouseArea {

                        anchors.fill: parent
                        anchors.margins: -10
                        hoverEnabled: true
                        onEntered: btn_phone.opacity = 1;
                        onExited: btn_phone.opacity = 0.7;
                        //onClicked: callback(fileName)
                        onClicked: {
                            //showFullScreen();
                            if(menu_shown) { menu_shown = false; onMenu(); }

                            console.log("Nomber of Columns: " + maingrid.columns)
                        }
                    }
                    function callback(fileName)
                    {
                        //information(0, "Message", "It's my message");
                        //window.Quit();
                        //showFullScreen();
                    }
                    function press()
                    {
                        btn_phone.opacity = 1;
                    }
                    function unpress()
                    {
                        btn_phone.opacity = 0.7;
                    }
                }

                Image {
                    id: btn_lamp
                    objectName: "btn_lamp"
                    source:  "images/Night-Lamp.bmp"
                    width: getWinthImages (4)
                    height: width
                    opacity : 0.7;
                    Behavior on opacity
                    {
                        NumberAnimation { duration: 100 }
                    }
                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -10
                        hoverEnabled: true
                        onEntered: btn_lamp.opacity = 1;
                        onExited: btn_lamp.opacity = 0.7;

                        //onClicked: callback(fileName)
                        onClicked: {
                            console.log("!!!! qml. btn_lamp w is "+ btn_lamp.width)
                            //showFullScreen();
                            if(menu_shown) { menu_shown = false; onMenu(); }
                            console.log("Call signal...")
                            console.log("Nomber of Columns: " + maingrid.columns)
                        }
                    }
                    function callback(fileName)
                    {
                        //information(0, "Message", "It's my message");
                        //window.Quit();
                        //showFullScreen();
                    }
                    function press()
                    {
                        btn_lamp.opacity = 1;
                    }
                    function unpress()
                    {
                        btn_lamp.opacity = 0.7;
                    }
                }

                Image {
                    id: btn_speaker
                    objectName: "btn_speaker"
                    source:  "images/Speaker-Volume.bmp"
                    width: getWinthImages (4)
                    height: width
                    opacity : 0.7;
                    Behavior on opacity
                    {
                        NumberAnimation { duration: 100 }
                    }
                    MouseArea {

                        anchors.fill: parent
                        anchors.margins: -10
                        hoverEnabled: true
                        onEntered: btn_speaker.opacity = 1;
                        onExited: btn_speaker.opacity = 0.7;
                        //onClicked: callback(fileName)
                        onClicked: {
                            if(menu_shown) { menu_shown = false; onMenu(); }
                            //showFullScreen();
                            console.log("Nomber of Columns: " + maingrid.columns)
                        }
                    }
                    function callback(fileName)
                    {
                        //information(0, "Message", "It's my message");
                        //window.Quit();
                        //showFullScreen();
                    }
                    function press()
                    {
                        btn_speaker.opacity = 1;
                    }
                    function unpress()
                    {
                        btn_speaker.opacity = 0.7;
                    }
                }

            }

            Column {
                spacing: 3
                Image {
                    id: btn_doctor
                    objectName: "btn_doctor"
                    source:  "images/Doctor.bmp"
                    width: btn_phone.width*2 + 3
                    height: width
                    opacity : 0.7;
                    Behavior on opacity
                    {
                        NumberAnimation { duration: 100 }
                    }
                    MouseArea {

                        anchors.fill: parent
                        anchors.margins: -10
                        hoverEnabled: true
                        onEntered: btn_doctor.opacity = 1;
                        onExited: btn_doctor.opacity = 0.7;
                        //onClicked: callback(fileName)
                        onClicked: {
                            //showFullScreen();
                            if(menu_shown) { menu_shown = false; onMenu(); }
                            console.log("Nomber of Columns: " + maingrid.columns)
                        }
                    }
                    function callback(fileName)
                    {
                        //information(0, "Message", "It's my message");
                        //window.Quit();
                        //showFullScreen();
                    }
                    function press()
                    {
                        btn_doctor.opacity = 1;
                    }
                    function unpress()
                    {
                        btn_doctor.opacity = 0.7;
                    }
                }
                Row {
                    spacing: 3
                    Image {
                        id: btn_soup
                        objectName: "btn_soup"
                        source:  "images/Soup-01.bmp"
                        width: getWinthImages (4)
                        height: width
                        opacity : 0.7;
                        Behavior on opacity
                        {
                            NumberAnimation { duration: 100 }
                        }
                        MouseArea {

                            anchors.fill: parent
                            anchors.margins: -10
                            hoverEnabled: true
                            onEntered: btn_soup.opacity = 1;
                            onExited: btn_soup.opacity = 0.7;
                            //onClicked: callback(fileName)
                            onClicked: {
                                //showFullScreen();
                                if(menu_shown) { menu_shown = false; onMenu(); }
                                console.log("Nomber of Columns: " + maingrid.columns)
                            }
                        }
                        function callback(fileName)
                        {
                            //information(0, "Message", "It's my message");
                            //window.Quit();
                            //showFullScreen();
                        }
                        function press()
                        {
                            btn_soup.opacity = 1;
                        }
                        function unpress()
                        {
                            btn_soup.opacity = 0.7;
                        }
                    }

                    Image {
                        id: btn_drink
                        objectName: "btn_drink"
                        source:  "images/Man-Drinking-02.bmp"
                        width: getWinthImages (4)
                        height: width
                        opacity : 0.7;
                        Behavior on opacity
                        {
                            NumberAnimation { duration: 100 }
                        }
                        MouseArea {

                            anchors.fill: parent
                            anchors.margins: -10
                            hoverEnabled: true
                            onEntered: btn_drink.opacity = 1;
                            onExited: btn_drink.opacity = 0.7;
                            //onClicked: callback(fileName)
                            onClicked: {
                                //showFullScreen();
                                if(menu_shown) { menu_shown = false; onMenu(); }
                                console.log("Nomber of Columns: " + maingrid.columns)
                            }
                        }
                        function callback(fileName)
                        {
                            //information(0, "Message", "It's my message");
                            //window.Quit();
                            //showFullScreen();
                        }
                        function press()
                        {
                            btn_drink.opacity = 1;
                        }
                        function unpress()
                        {
                            btn_drink.opacity = 0.7;
                        }
                    }
                }
            }

            Column {
                spacing: 3
                Rectangle {
                    id: rectangle_clock
                    objectName: "rectangle_clock"
                    width: getWinthImages (4)
                    height: width
                    color: "#220000"

                    onWidthChanged: clocktext2.sizeChanged()

                    Clock
                    {
                        id: clocktext2
                        objectName: "clocktext2"
                        anchors.fill: parent
                    }
                }


                Image {
                    id: btn_music
                    objectName: "btn_music"
                    source:  "images/Semi-Quaver-01.bmp"
                    width: getWinthImages (4)
                    height: width
                    opacity : 0.7;
                    Behavior on opacity
                    {
                        NumberAnimation { duration: 100 }
                    }
                    MouseArea {

                        anchors.fill: parent
                        anchors.margins: -10
                        hoverEnabled: true
                        onEntered: btn_music.opacity = 1;
                        onExited: btn_music.opacity = 0.7;

                        //onClicked: callback(fileName)
                        onClicked: {
                            if(menu_shown) { menu_shown = false; onMenu(); }
                            console.log("Nomber of Columns: " + maingrid.columns)
                        }
                    }
                    function callback(fileName)
                    {
                        //information(0, "Message", "It's my message");
                        //window.Quit();
                        //showFullScreen();
                    }
                    function press()
                    {
                        btn_music.opacity = 1;
                    }
                    function unpress()
                    {
                        btn_music.opacity = 0.7;
                    }
                }



                Image {
                    id: btn_monitor
                    objectName: "btn_monitor"
                    source:  "images/Monitor-01.bmp"
                    width: getWinthImages (4)
                    height: width
                    opacity : 0.7;
                    Behavior on opacity
                    {
                        NumberAnimation { duration: 100 }
                    }
                    MouseArea {

                        anchors.fill: parent
                        anchors.margins: -10
                        hoverEnabled: true
                        onEntered: btn_monitor.opacity = 1;
                        onExited: btn_monitor.opacity = 0.7;
                        //onClicked: callback(fileName)
                        onClicked: {
                            //showFullScreen();
                            if(menu_shown) { menu_shown = false; onMenu(); }
                            console.log("Nomber of Columns: " + maingrid.columns)
                        }
                    }
                    function callback(fileName)
                    {
                        //information(0, "Message", "It's my message");
                        //window.Quit();
                        //showFullScreen();
                    }
                    function press()
                    {
                        btn_monitor.opacity = 1;
                    }
                    function unpress()
                    {
                        btn_monitor.opacity = 0.7;
                    }
                }
            }
        }

        Rectangle {
            id: circleMonitorProjection
            width: 30;
            height: 30;
            radius: width*0.5
            color: "#03ffd5"
            x:  -width
            y:  -width
        }
    }
    function getWinthImages (countButtons) {
        var w, h;
        w = (rectangle1.width - menuImage.width*1.5)/countButtons - 3 *(countButtons-1)
        h = rectangle1.height/(countButtons-1) - 3 *(countButtons-2)
        if(w > h) return h
        else return w
    }

    /* МЕНЮ */

    Rectangle {
        id: chadowId
        anchors.fill: parent
        color: "#000000";
        opacity: 0
        Behavior on opacity { NumberAnimation { duration: 300 } }
    }

    Rectangle {

        id: menu_view_
        color: "#26017b";
        //opacity: menu_shown ? 1 : 0
        Column  {
            width: 134
            height: 188
            anchors {
                //horizontalCenter: menu_view_.horizontalCenter;
                top: menu_view_.top;
                topMargin: menu_view_.height/12;
                left: menu_view_.left;
                leftMargin: menu_view_.width/8;
            }
            spacing: 25

            Text {
                id: textFullScreen
                color: "#ffffff"
                font { family: "Segoe UI " }
                text: Window.FullScreen === mainWindow.visibility ? "В режиме окна" : "На весь экран"
                MouseArea {
                    anchors.fill: parent
                    onEntered:
                        textFullScreen.color = "#d3d2d2";
                    onExited:
                        textFullScreen.color = "#ffffff";
                    hoverEnabled: true
                    onClicked: {
                        //if (Window.visibility === ) showFullScreen();
                        Window.FullScreen === mainWindow.visibility ? showNormal() : showFullScreen();
                    }
                }
                font.pixelSize: 20
            }

            Text {
                id: textSettings
                text: "Настройки"
                color: "#ffffff";
                font.pixelSize: 20;
                font { family: "Segoe UI " }
                MouseArea {
                    anchors.fill: parent
                    onEntered:
                        textSettings.color = "#d3d2d2";
                    onExited:
                        textSettings.color = "#ffffff";
                    hoverEnabled: true
                    onClicked:  {
                        if(menu_shown) { menu_shown = false; onMenu(); }
                        stackView.push(Qt.resolvedUrl("menu/QmlSettings.qml"))
                    }
                }
            }

            Text {
                id: textCalib
                text: "Калибровка"
                color: "#ffffff";
                font.pixelSize: 20;
                font { family: "Segoe UI" }
                MouseArea {
                    anchors.fill: parent
                    onEntered:
                        textCalib.color = "#d3d2d2";
                    onExited:
                        textCalib.color = "#ffffff";
                    hoverEnabled: true
                    onClicked: { // console.log("valuepage")
                        //stackView.depth = 2
                        //                        mainWindow.minimumHeight = mainWindow.height;
                        //                        mainWindow.minimumWidth = mainWindow.width;
                        //                        mainWindow.maximumHeight = mainWindow.height;
                        //                        mainWindow.maximumWidth = mainWindow.width;

                        ifInitCamera = false;
                        //fullStopSignal();
                        if(menu_shown) { menu_shown = false; onMenu(); }
                        startCameraInitialisationSignal()
                        //если в настройках выбрана калибровка с движением шарика, тогда открываем Calibration.qml
                        // иначе если появление шарика при калибровке - открываем CalibrationEmersion.qml
                        console.log("Calibration Type is " + settings.getCalibrationType());
                        if (settings.getCalibrationType() === 0)
                            stackView.push(Qt.resolvedUrl("menu/CalibrationMotion.qml"))
                        else if (settings.getCalibrationType() === 1)
                            stackView.push(Qt.resolvedUrl("menu/CalibrationEmersion.qml"))
                    }
                }
            }

            Text {
                id: textExit
                text: "Выход"
                color: "#ffffff";
                font.pixelSize: 20;
                font { family: "Segoe UI" }
                MouseArea {
                    anchors.fill: parent
                    onEntered:
                        textExit.color = "#d3d2d2";
                    onExited:
                        textExit.color = "#ffffff";
                    hoverEnabled: true
                    onClicked:
                    {
                        Qt.quit();
                        //fullStopSignal();
                        //onExitTimer.running = true;
                    }
                }
            }
            Text {
                id: textStartProcessing
                text: "-- test"
                color: "#ffffff";
                font.pixelSize: 20;
                font { family: "Segoe UI" }
                MouseArea {
                    anchors.fill: parent
                    onEntered:
                        textStartProcessing.color = "#d3d2d2";
                    onExited:
                        textStartProcessing.color = "#ffffff";
                    hoverEnabled: true
                    onClicked: {
                        ifInitCamera = false;
                        //fullStopSignal();
                        if(menu_shown) { menu_shown = false; onMenu(); }
                        cameraInitialisationFinishedSlot(true);
                        workProcessSignal();
                    }
                }
            }

            Text {
                id: textTestBioID
                visible: false
                text: "-- Test BioID"
                color: "#ffffff";
                font.pixelSize: 20;
                font { family: "Segoe UI" }
                MouseArea {
                    anchors.fill: parent
                    onEntered:
                        textTestBioID.color = "#d3d2d2";
                    onExited:
                        textTestBioID.color = "#ffffff";
                    hoverEnabled: true
                    onClicked: {
                        testProgressonBioID.opacity = 1;
                        ifInitCamera = false;
                        //fullStopSignal();
                        testBioIDSignal();
                    }
                }
            }
            ProgressBar {
                id: testProgressonBioID
                value: 22
                minimumValue: 0
                maximumValue: 1519
                opacity : 0
            }

            Timer {
                id: onExitTimer
                interval: 1000
                repeat: true
                running: false
                onTriggered: {
                    if (exitCountdown === 0)
                    {
                        Qt.quit();
                    }
                    exitCountdown--;
                }
            }
        }

        width: 300
        height: mainWindow.height
        transform: Translate {
            id: menuTranslate
            x: -300
            Behavior on x { NumberAnimation { duration: 400; easing.type: Easing.OutQuad } }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        // Implements back key navigation
        focus: true
        Keys.onReleased: if (event.key === Qt.Key_Back && stackView.depth > 1) {
                             stackView.pop();
                             event.accepted = true;
                         }
        delegate: StackViewDelegate {
            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 300
                }
            }
        }
    }


    function onMenu()
    {
        menuTranslate.x = menu_shown ? 0 : -300
        chadowId.opacity = menu_shown ? 0.4 :0
    }
    //Component.onCompleted: folderModel.folder = "images/"

    function myQmlFunction(msg) {
        //maingrid
        // imageButtons.onClicked();
        console.log("Got message:", msg)
        return "some return value"
    }

    function cameraInitialisationFinishedSlot(value)
    {
        if(value) {
            ifInitCamera = true
        }
        else {
            ifInitCamera = false
        }
        console.log("cameraInitialisationFinishedSlot called :) value = " + value)
    }
    function unpressAllButtons()
    {
        btn_phone.unpress();
        //rectangle_clock
        btn_music.unpress();
        btn_lamp.unpress();
        btn_doctor.unpress();
        btn_monitor.unpress();
        btn_speaker.unpress();
        btn_soup.unpress();
        btn_drink.unpress();
    }

    function pressButtonSlot(value)
    {
        //console.log("QML log. btn №" + value + "pressed");
        unpressAllButtons();
        switch (value){
        case 1:
            btn_phone.press();
            break;
        case 2:
            btn_lamp.press();
            break;
        case 3:
            btn_speaker.press();
            break;
        case 4:
            btn_doctor.press();
            break;
        case 5:
            btn_soup.press();
            break;
        case 6:
            btn_drink.press();
            break;
        case 7:
            //rectangle_clock
            break;
        case 8:
            btn_music.press();
            break;
        case 9:
            btn_monitor.press();
            break;
        }
    }

    function drawPointOnMonitorSlot(xCoord, yCoord)
    {
        circleMonitorProjection.x = xCoord - circleMonitorProjection.width/2;
        circleMonitorProjection.y = yCoord - circleMonitorProjection.height/2;
    }

    function progressChangedTestBioIDSlot(val)
    {

        textTestBioID.text = "-- Test BioID. " + val
        testProgressonBioID.value = val

        if(val == 1521){
            testProgressonBioID.opacity = 0
            textTestBioID.text = "-- Test BioID has completed.\n See the result.txt"
        }
    }

    function slotCheckBadCalibrovka(value)
    {
        calibrationDone = value;
        flagcheckCalibrovka = true;
    }
}

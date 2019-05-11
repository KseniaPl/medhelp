import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1

Item {
    property int countdown: 5
    property int maxcountdown: 5
    property int exitCountdown: 3
    id: itemBase
    Rectangle {
        id: rectangle2
        width: parent.width
        height: parent.height
        color: "#000000"
        transformOrigin: Item.Center
        // opacity:0
        // onFocusChanged: {
        //     opacity:1
        //}

        MouseArea {
            anchors.fill: parent
            onClicked: {
                //rectangle2.opacity = 0;
                //clear();
                //   stackView2.push(Qt.resolvedUrl("main.qml"))
            }
            Behavior on opacity { NumberAnimation { duration: 300 } }
        }
    }

    Text {
        id: textInit
        color: "#ffffff"
        font.pixelSize: 24;
        font { family: "Segoe UI" }
        opacity: 0
        Behavior on opacity { NumberAnimation { duration: 700; } }
        text: "Подключение камеры..."
        anchors {
            horizontalCenter: rectangle2.horizontalCenter; top: rectangle2.top;
            margins: rectangle2.height/6
        }
    }

    Column {
        id: columnId
        spacing: 20
        opacity: countdown < 2 || countdown == maxcountdown ? 0 : 1
        visible: opacity == 0 ? false : true
        Behavior on opacity { NumberAnimation { duration: 700; } }
        anchors {
            horizontalCenter: rectangle2.horizontalCenter; top: rectangle2.top;
            margins: rectangle2.height/6
        }
        Text {
            id: textCalib
            color: "#ffffff"
            font.pixelSize: 24;
            font { family: "Segoe UI" }

            text: "Калибровка начнется через . . ."
        }

        Text {
            anchors.horizontalCenter:  columnId.horizontalCenter
            id: textTimer
            color: "#ffffff"
            font.pixelSize: 44;
            font { family: "Segoe UI" }
            text: countdown  < maxcountdown && countdown  > 0 ? countdown : ""
        }
        Timer {
            id: countdownTimer
            interval: 1000
            repeat: true
            running: true;
            onTriggered: {
                if(ifInitCamera) {
                    textInit.opacity = 0
                    countdown--;
                    if(countdown == 1)
                        startCalibrationSignal()
                    if(countdown == 0)
                    {
                        running = false;
                        round_trajectoryTimer.running = true;
                    }
                }
                else
                    textInit.opacity = 1
                if(ifCameraError)
                {
                    ifCameraError = false;
                    textInit.text = "Камера не подключена!"
                    countdownTimer.running = false
                }
            }
        }
    }

    Rectangle {
        id: round_trajectory
        // отступ
        property int trajectory_margin: root.width < root.height ? root.width/50 : root.height/50
        x: trajectory_margin
        y: x
        //размер прямоугольника (красного маркера)
        width: root.width < root.height? root.width/20 : root.height/20
        height: width
        color: rectangle2.color

        //направление движения
        property int directory: 0

        //длительность (для расчета одинаковой скорости на всех участках)
        //(при выборе Behavior on x { NumberAnimation { duration: round_trajectory.dur; } } )
        property int dur: 0

        //небольшая плавность
        Behavior on x { SmoothedAnimation { velocity: 150; easing.type: Easing.Linear } }
        Behavior on y { SmoothedAnimation { velocity: 150; easing.type: Easing.Linear } }

        //рвномерно
        //Behavior on x { NumberAnimation { duration: round_trajectory.dur; } }
        //Behavior on y { NumberAnimation { duration: round_trajectory.dur; } }


        Timer {
            id: round_trajectoryTimer
            interval: 500
            repeat: true
            running: false
            onTriggered: {
                // движение сверху
                if(round_trajectory.directory === 0)
                {
                    directoryChangedSignal(round_trajectory.directory)
                    round_trajectory.calculateDuration(round_trajectory.x, root.width - (round_trajectory.trajectory_margin + round_trajectory.width));
                    round_trajectory.x = root.width - (round_trajectory.trajectory_margin + round_trajectory.width);
                    round_trajectory.directory = 1;
                }
                // справа
                else if (round_trajectory.directory === 1 && round_trajectory.x === root.width - (round_trajectory.trajectory_margin + round_trajectory.width))
                {
                    directoryChangedSignal(round_trajectory.directory)
                    round_trajectory.calculateDuration(round_trajectory.y, root.height - (round_trajectory.trajectory_margin + round_trajectory.width));
                    round_trajectory.y = root.height - (round_trajectory.trajectory_margin + round_trajectory.width);
                    round_trajectory.directory = 2;
                }
                // снизу
                else if (round_trajectory.directory === 2 && round_trajectory.y === root.height - (round_trajectory.trajectory_margin + round_trajectory.width))
                {
                    directoryChangedSignal(round_trajectory.directory)
                    round_trajectory.calculateDuration(round_trajectory.x, round_trajectory.trajectory_margin);
                    round_trajectory.x = round_trajectory.trajectory_margin;
                    round_trajectory.directory = 3;
                }
                // слева
                else if (round_trajectory.directory === 3 && round_trajectory.x === round_trajectory.trajectory_margin)
                {
                    directoryChangedSignal(round_trajectory.directory)
                    round_trajectory.calculateDuration(round_trajectory.y, round_trajectory.trajectory_margin);
                    round_trajectory.y = round_trajectory.trajectory_margin;
                    round_trajectory.directory = 4;
                }
                // конец
                else if (round_trajectory.directory === 4 && round_trajectory.y === round_trajectory.trajectory_margin)
                {
                    stopCalibrationSignal();

                    round_trajectory.directory = 0;
                    textCalib.text = "Калибровка завершена.";
                    columnId.visible = true;
                    columnId.opacity = 1;

                    //TODO:
                    //разрешить изменение размеров окна (послать сигнал в main.qml

                    //stop
                    round_trajectory.visible = false;
                    round_trajectoryTimer.running = false;
                    onExitTimer.running = true;
                }
            }
        }

        function calculateDuration(point1, point2)
        { dur = 5*Math.abs(point1 - point2); }

        Rectangle {
            id: round
            width: round_trajectory.width
            height: width
            radius: width
            color: "red"
            opacity: countdown < 4 ? 1 : 0
            anchors.centerIn: round_trajectory
            property bool sizeBig: true
            Timer {
                id: roundTimer
                interval: 500
                repeat: true
                running: true
                onTriggered: {
                    if(round.sizeBig) { round.width = round_trajectory.width; }
                    else { round.width = round_trajectory.width/1.1; }
                    round.sizeBig = !round.sizeBig;
                }
            }
            Behavior on  width { NumberAnimation { duration: 500; easing.type: Easing.OutQuad } }
            Behavior on  opacity { NumberAnimation { duration: 500;  } }
        }
    }

    Timer {
        id: onExitTimer
        interval: 1000
        repeat: true
        running: false
        onTriggered: {
            if (exitCountdown === 0)
            {
                onExitTimer.running = false;
                rectangle2.opacity = 0;
                clear();
            }
            exitCountdown--;
        }
    }
    Text {
        id: textExit
        color: "#ffffff"
        font.pixelSize: 18
        font { family: "Segoe UI"
                bold: true}
        opacity: 0.4
        //Behavior on opacity { NumberAnimation { duration: 700; } }
        text: "Прервать"
        anchors {
            left: parent.left
            leftMargin: rectangle2.width/10
            top: rectangle2.top;
            topMargin: rectangle2.height/10
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                rectangle2.opacity = 0;
                fullStopSignal();
                ifInitCamera = false;
                reset();
                clear();
            }
            //Behavior on opacity { NumberAnimation { duration: 300 } }
        }
    }

    function reset()
    {
        countdown = 5;
        maxcountdown = 5;
        exitCountdown = 3;
        textInit.opacity = 1;
        textInit.text = "Подключение камеры..."
        round_trajectory.directory = 0;
        textCalib.text = "Калибровка начнется через . . .";
        round_trajectory.visible = true;
        round_trajectoryTimer.running = false;
        onExitTimer.running = false;
        countdownTimer.running = true;
    }
}

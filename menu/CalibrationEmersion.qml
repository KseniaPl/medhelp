import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import Settings 1.0
import QtMultimedia 5.0

Item {
    Settings {
        id: settings
    }
    property int countdown: 5
    property int maxcountdown: 5
    property int exitCountdown: 3

    //количество колонок и столбцов, в которых юудет появляться шарик. должно быть одинаковым!
    property int collsRound: Math.sqrt(settings.getPointsCount()) // 2 или 3
    property int rowsRound: collsRound
    property int collsRoundCounter: collsRound - 1

    // число секунд, когда шарик находится в неподвижном состоянии
    property int rowsRoundCounter: 4
    property int rowsRoundSeconds: 4


    // счетчик калибровки
    property int roundCounter: collsRound * rowsRound


    id: itemBase
    Rectangle {
        id: rectangle2
        width: parent.width
        height: parent.height
        color: "#000000"
        transformOrigin: Item.Center

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
            horizontalAlignment : Text.AlignHCenter
            text: "Калибровка начнется через..."
        }

        Text {
            anchors.horizontalCenter:  columnId.horizontalCenter
            id: textTimer
            color: "#ffffff"
            font.pixelSize: 44;
            font { family: "Segoe UI" }
            text: countdown  < maxcountdown && countdown  > 0 ? countdown : ""
        }

        Rectangle {
            anchors.horizontalCenter:  columnId.horizontalCenter
            id: btnRestartCalib
            width: txtRestartCalib.width + 10
            height: txtRestartCalib.height + 5
            border.color: "white"
            color: "black"
            border.width: 2
            Text {
                id: txtRestartCalib
                text: "Начать заново"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 20
                color: "white"
                anchors.horizontalCenter:  btnRestartCalib.horizontalCenter
                font { family: "Segoe UI" }
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    textCalib.text = "Калибровка начнется через..."
                    maxcountdown = 5;
                    exitCountdown = 3;
                    collsRoundCounter = collsRound - 1;
                    rowsRoundCounter = 4;
                    rowsRoundSeconds = 4;
                    roundCounter = collsRound * rowsRound;
                    btnRestartCalib.opacity = 0;
                    countdownTimer.running = true;
                    round_trajectory.x = round_trajectory.trajectory_margin;
                    round_trajectory.y = round_trajectory.trajectory_margin;

                }
                onEntered: btnRestartCalib.border.color = "red"
                onExited: btnRestartCalib.border.color = "white"
                hoverEnabled: true
            }
            opacity: 0;
            Behavior on opacity { NumberAnimation { duration: 200 } }
        }

        Timer {
            id: countdownTimer
            interval: 1000
            repeat: true
            running: true;
            onTriggered: {
                if(ifInitCamera)
                {
                    textInit.opacity = 0
                    countdown--;
                    if(countdown == 1)
                    {
                        if(!calibrationDone) {
                            startCalibrationSignal();
                            columnId.opacity = 0;
                        }
                        else
                            stopEyeBlinkDetectorSignal();
                    }
                    if(countdown == 0)
                    {
                        running = false;
                        if(!calibrationDone)
                            round_trajectoryTimer.running = true;
                        else
                        {
                            playSound.play();
                            textExit.opacity = 0;
                            textCalib.text = "Калибровка пройдена";
                            textCalib.opacity = 1;
                            columnId.visible = true;
                            columnId.opacity = 1;
                            onExitTimer.running = true;
                        }
                    }

                    if(countdown == 4 && calibrationDone)
                        startEyeBlinkDetectorSignal();

                }
                else textInit.opacity = 1
                if(ifCameraError)
                {
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
        y: trajectory_margin
        //размер прямоугольника (красного маркера)
        width: root.width < root.height? root.width/20 : root.height/20
        height: width
        color: rectangle2.color
        //длительность (для расчета одинаковой скорости на всех участках)
        //(при выборе Behavior on x { NumberAnimation { duration: round_trajectory.dur; } } )
        property int dur: 0


        Timer {
            id: round_trajectoryTimer
            interval: 1000
            repeat: true
            running: false
            onTriggered: {
                //анимация шарика
                //три секунды шарик на месте
                //прикрутить куда нить сигнал flagEmersionRoundCalibSignal
                if(rowsRoundCounter == rowsRoundSeconds-1)
                {
                    //сигнал о начале записи координат зрачков для позиции шарика
                    flagEmersionRoundCalibSignal()
                    //сигнал записи координат шарика на мониторе
                    getMonitorCoordCalibSignal(round_trajectory.x + round_trajectory.width/2, round_trajectory.y + round_trajectory.height/2)
                }
                if(rowsRoundCounter == 0)
                {
                    //сигнал о том чтобы зрачки не записывались в массив
                    flagStopEmersionRoundCalibSignal()

                    if(collsRoundCounter == 0) {
                        collsRoundCounter = collsRound
                        round_trajectory.x = round_trajectory.trajectory_margin
                        round_trajectory.y += (itemBase.height - 2*round_trajectory.trajectory_margin - round_trajectory.height)/(rowsRound - 1);// -  2*round_trajectory.trajectory_margin
                    }
                    else
                        round_trajectory.x += (itemBase.width - 2*round_trajectory.trajectory_margin - round_trajectory.width)/(collsRound - 1);// -  4*round_trajectory.trajectory_margin
                    collsRoundCounter--
                    rowsRoundCounter = rowsRoundSeconds

                    roundCounter--
                }
                rowsRoundCounter--

                //конец анализа и записи
                if(roundCounter == 0) {
                    maxcountdown = 4;
                    //calibrationDone = true;
                    stopCalibrationSignal();
                    roundCounter--;
                }

                if(flagcheckCalibrovka) {
                    if(!calibrationDone) {
                        textCalib.text = "Калибровка прошла неудачно. ";
                        btnRestartCalib.opacity = 1;
                        countdown = 5;
                    }
                    else {
                        textCalib.text = "Отлично! Теперь плотно закройте глаза до тех пор,\n пока не прозвучит звуковой сигнал";
                        countdown = 7;
                        countdownTimer.running = true;
                    }

                    flagcheckCalibrovka = false;
                    columnId.visible = true;
                    columnId.opacity = 1;
                    round_trajectoryTimer.running = false;
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
                onTriggered:
                {
                    if(round.sizeBig) {
                        round.width = round_trajectory.width;
                    }
                    else {
                        round.width = round_trajectory.width/1.8;
                    }
                    round.sizeBig = !round.sizeBig;
                }
            }
            Behavior on  width { NumberAnimation { duration: 500; easing.type: Easing.OutQuad } }
            Behavior on  opacity { NumberAnimation { duration: 500;  } }

            Rectangle {
                id: roundSmall
                width: 3
                height: width
                radius: width
                color: "#000000"
                anchors.centerIn: round
            }
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
                clear();
                fullStopSignal();
            }
            //Behavior on opacity { NumberAnimation { duration: 300 } }
        }
    }

    Audio {
           id: playSound
           source: "qrc:/Sound.wav"
       }

}

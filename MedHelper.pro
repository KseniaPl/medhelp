#
# В переменных средах слудет добавить переменную OPENCV_QT
# Её значение - путь до папки с билдом opencv для qt (mingw).
# Например "D:/opencv/RELEASE"
# В данной папке должны быть следующие пути:
#   /bin/*.dll
#   /install/x64/mingw/lib/*.dll.a
#   /install/include/opencv
#   /install/include/opencv2
#
# В папке с данным .pro файлом должны находиться следющие папки:
#   \inc\dest
#   \inc\Eigen
#   \src\dest
#

TEMPLATE = app

QT += qml quick widgets
QT += declarative
QT += core
QT += multimedia
QT += xml

SOURCES += main.cpp \
    Logger.cpp \
    ControlManager.cpp \
    FileManager.cpp \
    BaseLogic.cpp \
    MainClass.cpp \
    Settings.cpp \
    EyeTracker/Analyser.cpp \
    EyeTracker/Calibration.cpp \
    EyeTracker/Camera.cpp \
    EyeTracker/Eyetracker.cpp \
    EyeTracker/PupilDetector.cpp \
    EyeTracker/bioidtest.cpp \
    EyeTracker/facefeatures.cpp \
    src/dest/core/image.cpp \
    src/dest/core/regressor.cpp \
    src/dest/core/shape.cpp \
    src/dest/core/tracker.cpp \
    src/dest/core/training_data.cpp \
    src/dest/core/tree.cpp \
    src/dest/face/face_detector.cpp \
    src/dest/util/draw.cpp \
    src/dest/util/glob.cpp \
    EyeTracker/kalmanfilters.cpp \
    EyeTracker/Blinkdetector.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    func.h \
    Logger.h \
    ControlManager.h \
    FileManager.h \
    BaseLogic.h \
    MainClass.h \
    Settings.h \
    EyeTracker/Analyser.h \
    EyeTracker/Calibration.h \
    EyeTracker/Camera.h \
    EyeTracker/eyetracker.h \
    EyeTracker/PupilDetector.h \
    EyeTracker/bioidtest.h \
    EyeTracker/facefeatures.h \
    inc/dest/core/config.h \
    inc/dest/core/image.h \
    inc/dest/core/regressor.h \
    inc/dest/core/shape.h \
    inc/dest/core/tracker.h \
    inc/dest/core/training_data.h \
    inc/dest/core/tree.h \
    inc/dest/face/face_detector.h \
    inc/dest/flatbuffers/flatbuffers.h \
    inc/dest/io/matrix_io.h \
    inc/dest/io/rect_io.h \
    inc/dest/util/convert.h \
    inc/dest/util/draw.h \
    inc/dest/util/glob.h \
    inc/dest/util/log.h \
    inc/dest/dest.h \
    inc/dest/tinydir/tinydir.h \
    EyeTracker/kalmanfilters.h \
    EyeTracker/Blinkdetector.h

#INCLUDEPATH += E:\Qt_projects\Medhelper_facetrack\inc
#INCLUDEPATH += E:\Qt_projects\Medhelper_facetrack\src
#INCLUDEPATH += D:/opencv/RELEASE/install/include
#LIBS += "D:/opencv/RELEASE/bin/*.dll"

INCLUDEPATH += $$PWD/inc
INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$(OPENCV_QT)/install/include
LIBS += "$$(OPENCV_QT)/install/x64/mingw/lib/*.a"

DISTFILES +=

CONFIG += c++11

OTHER_FILES += \
    Clock.qml \
    main.qml \
    main_old.qml \
    menu/CalibrationMotion.qml \
    menu/CalibrationEmersion.qml \
    menu/QmlSettings.qml

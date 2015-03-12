p#-------------------------------------------------
#
# Project created by QtCreator 2015-02-02T18:58:43
#
#-------------------------------------------------

QT       += core gui

# opencv
unix: CONFIG += link_pkgconfig static
unix: PKGCONFIG += opencv
win32: INCLUDEPATH += $(OPENCV_DIR)/include
win32: DEPENDPATH += $(OPENCV_DIR)/include
win32:CONFIG(release, debug|release):{
    LIBS += -L"$(OPENCV_DIR)/lib/" \
    -lopencv_core$(OPENCV_VER) \
    -lopencv_video$(OPENCV_VER) \
    -lopencv_highgui$(OPENCV_VER) \
    -lopencv_calib3d$(OPENCV_VER) \
    -lopencv_imgproc$(OPENCV_VER)
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PatternDetection
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagecontainer.cpp \
    detection.cpp

HEADERS  += mainwindow.hh \
    imagecontainer.hh \
    detection.hh

FORMS    += mainwindow.ui

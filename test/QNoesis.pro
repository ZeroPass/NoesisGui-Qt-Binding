#-------------------------------------------------
#
# Project created by QtCreator 2016-10-04T12:57:34
#
#-------------------------------------------------

QT       += core gui widgets opengl network

lessThan(QT_VERSION, 5.4): error("Qt version must be greater or equal to 5.4")

TARGET = QNoesis
TEMPLATE = app
CONFIG += c++14

isEmpty(NOESIS_SDK_DIR): error("NoesisGUI SDK path not specified")
INCLUDEPATH += "$${NOESIS_SDK_DIR}"
INCLUDEPATH += "$${NOESIS_SDK_DIR}/Include"
DEPENDPATH += "$${NOESIS_SDK_DIR}/Include"
INCLUDEPATH += "../src"


SOURCES += main.cpp   \
    mainwindow.cpp \
    ../src/qnoesiswidget.cpp \
    ../src/qnsrenderdevice.cpp \
    "$${NOESIS_SDK_DIR}/Src/Renderers/GL/GLRenderDevice.cpp" \
    ../src/qnoesiswindow.cpp

HEADERS  += \
    mainwindow.h \
    ../src/qnoesisapplication.h \
    ../src/qnoesiswidget.h \
    ../src/qnsrenderdevice.h \
    ../src/qnoesiswindow.h


win32: LIBS += -L"$${NOESIS_SDK_DIR}/Lib/windows_x86/" -lNoesis
else:win64:LIBS += -L"$${NOESIS_SDK_DIR}/Lib/windows_x86_64/" -lNoesis
else:macx {
    QMAKE_CXXFLAGS += -F$$(HOME)/Library/Frameworks
    QMAKE_LFLAGS   += -F$$(HOME)/Library/Frameworks
    LIBS += "$${NOESIS_SDK_DIR}/Bin/osx/Noesis.dylib"
    QMAKE_RPATHDIR += "$${NOESIS_SDK_DIR}/Bin/osx/"
    #LIBS += -framework OpenGL.framework
}
#else:ios: LIBS += -L"$${NOESIS_SDK_DIR}/Lib/ios/" -lNoesis




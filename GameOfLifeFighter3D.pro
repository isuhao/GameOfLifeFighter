LIBS += ../GameOfLifeFighter/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

#QMAKE_CXX = g++-5
#QMAKE_LINK = g++-5
#QMAKE_CC = gcc-5
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += \
    ../GameOfLifeFighter/Urho3D/include \
    ../GameOfLifeFighter/Urho3D/include/Urho3D/ThirdParty

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

include(GameOfLifeFighterConsole.pri)

SOURCES += \
    mastercontrol.cpp \
    inputmaster.cpp \
    golfcam.cpp \
    cell.cpp \
    cellmaster.cpp \
    cellring.cpp \
    urho3dhelper.cpp \
    luckey.cpp

HEADERS += \
    mastercontrol.h \
    inputmaster.h \
    golfcam.h \
    cell.h \
    cellmaster.h \
    cellring.h \
    urho3dhelper.h \
    luckey.h

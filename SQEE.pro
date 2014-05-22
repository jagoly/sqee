TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-window -lsfml-system

SOURCES += main.cpp \
    application.cpp \
    eventhandler.cpp \
    scene.cpp \
    simplehandlers.cpp

HEADERS += \
    application.hpp \
    scene.hpp \
    eventhandler.hpp


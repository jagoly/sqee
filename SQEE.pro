TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-window -lsfml-system

SOURCES += \
    main.cpp \
    simplehandlers.cpp \
    libsqee/application.cpp \
    libsqee/eventhandler.cpp \
    libsqee/scene.cpp

HEADERS += \
    simplehandlers.hpp \
    libsqee/application.hpp \
    libsqee/eventhandler.hpp \
    libsqee/libsqee.hpp \
    libsqee/scene.hpp

OTHER_FILES += \
    DroidSans.ttf


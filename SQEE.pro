TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-window -lsfml-system

SOURCES += \
    main.cpp \
    libsqee/app/application.cpp \
    libsqee/app/eventhandler.cpp \
    libsqee/scenes/scene.cpp \
    libsqee/basics/handlers.cpp

HEADERS += \
    libsqee/libsqee.hpp \
    libsqee/app/application.hpp \
    libsqee/app/eventhandler.hpp \
    libsqee/scenes/scene.hpp \
    libsqee/basics/handlers.hpp \
    libsqee/extra.hpp

OTHER_FILES += \
    DroidSans.ttf


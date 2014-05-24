TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-window -lsfml-system

SOURCES += \
    main.cpp \
    libsqee/app/application.cpp \
    libsqee/visuals/scene.cpp \
    libsqee/basics/handlers.cpp \
    libsqee/logic/entity.cpp \
    libsqee/app/handler.cpp \
    libsqee/tests/suite.cpp \
    libsqee/visuals/textureholder.cpp \
    libsqee/logic/stage.cpp

HEADERS += \
    libsqee/app/application.hpp \
    libsqee/visuals/scene.hpp \
    libsqee/basics/handlers.hpp \
    libsqee/extra.hpp \
    libsqee/logic/entity.hpp \
    libsqee/app/handler.hpp \
    libsqee/tests/suite.hpp \
    libsqee/base.hpp \
    libsqee/visuals/textureholder.hpp \
    libsqee/logic/stage.hpp

OTHER_FILES += \
    DroidSans.ttf


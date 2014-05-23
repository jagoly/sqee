TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-window -lsfml-system

SOURCES += \
    main.cpp \
    libsqee/app/application.cpp \
    libsqee/scenes/scene.cpp \
    libsqee/basics/handlers.cpp \
    libsqee/logic/entity.cpp \
    libsqee/logic/component.cpp \
    libsqee/app/handler.cpp

HEADERS += \
    libsqee/libsqee.hpp \
    libsqee/app/application.hpp \
    libsqee/scenes/scene.hpp \
    libsqee/basics/handlers.hpp \
    libsqee/extra.hpp \
    libsqee/logic/entity.hpp \
    libsqee/app/handler.hpp \
    libsqee/logic/component.hpp

OTHER_FILES += \
    DroidSans.ttf


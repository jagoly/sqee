TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lsfml-graphics -lsfml-window -lsfml-system -ljsoncpp

SOURCES += \
    main.cpp \
    libsqee/app/application.cpp \
    libsqee/visuals/scene.cpp \
    libsqee/basics/handlers.cpp \
    libsqee/logic/entity.cpp \
    libsqee/app/handler.cpp \
    libsqee/visuals/textureholder.cpp \
    libsqee/logic/stage.cpp \
    libsqee/tests/soko/stages.cpp \
    libsqee/tests/soko/scenes.cpp \
    libsqee/tests/soko/app.cpp \
    libsqee/tests/soko/helpers.cpp \
    libsqee/tests/soko/level.cpp

HEADERS += \
    libsqee/app/application.hpp \
    libsqee/visuals/scene.hpp \
    libsqee/basics/handlers.hpp \
    libsqee/extra.hpp \
    libsqee/logic/entity.hpp \
    libsqee/app/handler.hpp \
    libsqee/base.hpp \
    libsqee/visuals/textureholder.hpp \
    libsqee/logic/stage.hpp \
    libsqee/tests/soko.hpp \
    libsqee/tests/soko/stages.hpp \
    libsqee/tests/soko/scenes.hpp \
    libsqee/tests/soko/app.hpp \
    libsqee/tests/soko/helpers.hpp \
    libsqee/tests/soko/level.hpp

OTHER_FILES += \
    DroidSans.ttf \
    test_soko/DroidSans.ttf \
    test_soko/playerStill.png \
    test_soko/level1.json \
    test_soko/bg_blue.png \
    test_soko/bg_green.png \
    test_soko/goal.png \
    test_soko/hole.png \
    test_soko/rock.png \
    test_soko/player_still.png \
    test_soko/wall.png

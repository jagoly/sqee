#pragma once

#include <sqee/defs.hpp>
#include <sqee/misc/containers.hpp>

namespace sqt {

class Settings {
public:
    sq::SettingMap<int> smInt;
    sq::SettingMap<float> smFloat;
    sq::SettingMap<string> smString;

    void apply();
    void revert();

    bool check_update(const string& _object);

private:
    set<string> updatedSet;
};

struct VideoSettings : public Settings {
    VideoSettings();
};

struct SoundSettings : public Settings {
    SoundSettings();
};

struct InputSettings : public Settings {
    InputSettings();
};

VideoSettings& vidSet();

}

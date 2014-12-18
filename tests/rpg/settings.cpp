#include "settings.hpp"

using namespace sqt;

void Settings::apply() {
    smInt.apply();
    smFloat.apply();
    smString.apply();

    updatedSet.clear();
}

void Settings::revert() {
    smInt.revert();
    smFloat.revert();
    smString.revert();
}

bool Settings::check_update(const string& _object) {
    if (updatedSet.count(_object)) return false;
    else { updatedSet.emplace(_object); return true; }
}

VideoSettings::VideoSettings() {
    #ifdef SQEE_DEBUG
    cout << "INFO: Initializing Global \"sqt::VideoSettings\"" << endl;
    #endif

    smInt.add_setting("shadQuality", 2);
    smInt.add_setting("fxaaQuality", 2);


}

VideoSettings& sqt::vidSet() {
    static VideoSettings videoSettings;
    return videoSettings;
}

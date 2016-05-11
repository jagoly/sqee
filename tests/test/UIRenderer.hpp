#pragma once

#include <sqee/forward.hpp>
#include <sqee/builtins.hpp>

namespace jagolygui { class Widget; }

namespace sqt {

class UIRenderer {
public:
    UIRenderer(); ~UIRenderer();

    void render_gui(jagolygui::Widget* _root);

private:
    struct Implementation;
    unique_ptr<Implementation> impl;
};

}

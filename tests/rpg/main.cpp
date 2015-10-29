#include <vector>
#include <iostream>

#include <sqee/redist/lmccop.hpp>
#include <sqee/app/Resources.hpp>

#include "RpgApp.hpp"

op::ArgStatus checkop_TEST(const op::Option& _option, bool _msg) {
    if (_option.arg != NULL) return op::ArgStatus::ARG_OK;
    return op::ArgStatus::ARG_ILLEGAL;
}

enum Index { UNKNOWN, HELP, TEST };
const op::Descriptor usage[] = {
    {UNKNOWN, 0, "", "",     op::Arg::None, "USAGE: sqee_rpg [options]\n\nOptions:"},
    {HELP,    0, "", "help", op::Arg::None, " --help\n Show this message"},
    {TEST,    0, "", "test", &checkop_TEST, " --test path\n Load test from path"},
    {0,0,0,0,0,0}
};

int main(int argc, char* argv[]) {
    argc-=(argc>0); argv+=(argc>0);

    op::Stats stats(usage, argc, argv);
    std::vector<op::Option> options(stats.options_max);
    std::vector<op::Option> buffer(stats.buffer_max);
    op::Parser parse(usage, argc, argv, options.data(), buffer.data());

    if (options[HELP] || options[UNKNOWN] || parse.error()) {
        op::printUsage(std::cout, usage);
        return 1;
    }

    if (options[TEST]) {
        sq::static_path() = options[TEST].arg;
    } else sq::static_path() = "assets/";

    sqt::RpgApp app;
    app.eval_test_init();
    return app.run();
}

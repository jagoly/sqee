#include <vector>

#include <sqee/redist/lmccop.hpp>

#include "RpgApp.hpp"

enum Index { UNKNOWN, HELP };
const op::Descriptor usage[] = {
    {UNKNOWN, 0, "", "",     op::Arg::None, "USAGE: sqee_rpg [options]\n\nOptions:"},
    {HELP,    0, "", "help", op::Arg::None, " --help \t Show this message"},
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

    sqt::RpgApp app;
    return app.run();
}

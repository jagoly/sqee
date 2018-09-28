// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/app/ChaiEngine.hpp>

#include <chaiscript/chaiscript.hpp>

using namespace sq;

//============================================================================//

UniquePtr<ChaiEngine> sq::create_ChaiEngine()
{
    const Vector<String> modulePaths = {};
    const Vector<String> usePaths = {};
    const auto chaiOptions = chaiscript::default_options();

    return std::make_unique<ChaiEngine>(modulePaths, usePaths, chaiOptions);
}

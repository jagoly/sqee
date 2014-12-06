#include "resbank.hpp"

using namespace sqt;

ResBank::ResBank() {
    #ifdef SQEE_DEBUG
    cout << "INFO: Initializing Global \"sqt::ResBank\"" << endl;
    #endif

    const uchar t8080FF[3] = {0x80, 0x80, 0xFF};
    const uchar t000000[3] = {0x00, 0x00, 0x00};
    const uchar tFFFFFF[3] = {0xFF, 0xFF, 0xFF};

    //////////////////

    tex2D_8080FF.create(gl::TEXTURE_2D, gl::RGB, gl::RGB8);
    tex2D_8080FF.resize({1, 1, 1});
    tex2D_8080FF.buffer_memory(t8080FF);

    tex2D_000000.create(gl::TEXTURE_2D, gl::RGB, gl::RGB8);
    tex2D_000000.resize({1, 1, 1});
    tex2D_000000.buffer_memory(t000000);

    tex2D_FFFFFF.create(gl::TEXTURE_2D, gl::RGB, gl::RGB8);
    tex2D_FFFFFF.resize({1, 1, 1});
    tex2D_FFFFFF.buffer_memory(tFFFFFF);

    //////////////////

    tex2DA_8080FF.create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8);
    tex2DA_8080FF.resize({1, 1, 1});
    tex2DA_8080FF.buffer_memory(t8080FF);

    tex2DA_000000.create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8);
    tex2DA_000000.resize({1, 1, 1});
    tex2DA_000000.buffer_memory(t000000);

    tex2DA_FFFFFF.create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8);
    tex2DA_FFFFFF.resize({1, 1, 1});
    tex2DA_FFFFFF.buffer_memory(tFFFFFF);
}

ResBank& sqt::resBank() {
    static ResBank resBank;
    return resBank;
}

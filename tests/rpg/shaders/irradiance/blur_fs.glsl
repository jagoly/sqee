// GLSL Fragment Shader

layout(binding=0) uniform sampler2DArray texIrrd;

out vec3 fragColour;

uniform uint layer;

ivec2 n_conv_t(ivec2 _crd) { return ivec2(_crd.x + 128, 1); }
ivec2 n_conv_r(ivec2 _crd) { return _crd; }
ivec2 n_conv_b(ivec2 _crd) { return ivec2(_crd.x + 160, 31); }
ivec2 n_conv_l(ivec2 _crd) { return ivec2(127, _crd.y); }

ivec2 e_conv_t(ivec2 _crd) { return ivec2(31 + 96, _crd.x); }
ivec2 e_conv_r(ivec2 _crd) { return _crd; }
ivec2 e_conv_b(ivec2 _crd) { return ivec2(31 + 128, 32 - _crd.x); }
ivec2 e_conv_l(ivec2 _crd) { return _crd; }

ivec2 s_conv_t(ivec2 _crd) { return ivec2(32 - _crd.x + 64, 31); }
ivec2 s_conv_l(ivec2 _crd) { return _crd; }
ivec2 s_conv_b(ivec2 _crd) { return ivec2(32 - _crd.x + 96, 0); }
ivec2 s_conv_r(ivec2 _crd) { return _crd; }

ivec2 w_conv_t(ivec2 _crd) { return ivec2(0 + 0, 32 - _crd.x); }
ivec2 w_conv_r(ivec2 _crd) { return ivec2(0, _crd.y); }
ivec2 w_conv_b(ivec2 _crd) { return ivec2(0 + 32, _crd.x); }
ivec2 w_conv_l(ivec2 _crd) { return _crd; }


ivec2 conv_t(ivec2 _crd, uint _fInd) {
    if (_fInd == 0) return n_conv_t(_crd);
    if (_fInd == 1) return e_conv_t(_crd);
    if (_fInd == 2) return s_conv_t(_crd);
    if (_fInd == 3) return w_conv_t(_crd);
    if (_fInd == 4) return u_conv_t(_crd);
    if (_fInd == 5) return d_conv_t(_crd);
}

ivec2 conv_r(ivec2 _crd, uint _fInd) {
    if (_fInd == 0) return n_conv_r(_crd);
    if (_fInd == 1) return e_conv_r(_crd);
    if (_fInd == 2) return s_conv_r(_crd);
    if (_fInd == 3) return w_conv_r(_crd);
    if (_fInd == 4) return u_conv_r(_crd);
    if (_fInd == 5) return d_conv_r(_crd);
}

ivec2 conv_b(ivec2 _crd, uint _fInd) {
    if (_fInd == 0) return n_conv_b(_crd);
    if (_fInd == 1) return e_conv_b(_crd);
    if (_fInd == 2) return s_conv_b(_crd);
    if (_fInd == 3) return w_conv_b(_crd);
    if (_fInd == 4) return u_conv_b(_crd);
    if (_fInd == 5) return d_conv_b(_crd);
}

ivec2 conv_l(ivec2 _crd, uint _fInd) {
    if (_fInd == 0) return n_conv_l(_crd);
    if (_fInd == 1) return e_conv_l(_crd);
    if (_fInd == 2) return s_conv_l(_crd);
    if (_fInd == 3) return w_conv_l(_crd);
    if (_fInd == 4) return u_conv_l(_crd);
    if (_fInd == 5) return d_conv_l(_crd);
}

void main() {
    ivec2 fOrig = ivec2(gl_FragCoord.xy) / ivec2(32) * 32;
    ivec2 fCrd = ivec2(gl_FragCoord.xy) % ivec2(32);
    uint fInd = uint(gl_FragCoord.x) / 32u;

    vec3 value = texelFetch(texIrrd, ivec3(gl_FragCoord.xy, layer), 0).rgb;
    vec3 valNN, valNZ, valNP, valZN, valZP, valPN, valPZ, valPP;

    ivec2 crdNN = fCrd + ivec2(-1, -1);
    ivec2 crdNZ = fCrd + ivec2(-1,  0);
    ivec2 crdNP = fCrd + ivec2(-1, +1);
    ivec2 crdZN = fCrd + ivec2( 0, -1);
    ivec2 crdZP = fCrd + ivec2( 0, +1);
    ivec2 crdPN = fCrd + ivec2(+1, -1);
    ivec2 crdPZ = fCrd + ivec2(+1,  0);
    ivec2 crdPP = fCrd + ivec2(+1, +1);


    ////////////////////

    if (crdNN.x == -1 && crdNN.y == -1) {
        valNN = (valNZ + valZN) * 0.5f;
    } else {
        if (crdNN.x < 0) crdNN = conv_l(crdNN, fInd);
        else if (crdNN.y < 0) crdNN = conv_b(crdNN, fInd);
        valNN = texelFetch(texIrrd, ivec3(crdNN + fOrig, layer), 0).rgb;
    }

    if (crdNZ.x == -1) crdNZ = conv_l(crdNZ, fInd);
    valNZ = texelFetch(texIrrd, ivec3(crdNZ + fOrig, layer), 0).rgb;

    if (crdNP.x == -1 && crdNP.y == 32) {
        valNP = (valNZ + valZP) * 0.5f;
    } else {
        if (crdNP.x == -1) crdNP = conv_l(crdNP, fInd);
        else if (crdNP.y == 32) crdNP = conv_t(crdNP, fInd);
        valNP = texelFetch(texIrrd, ivec3(crdNP + fOrig, layer), 0).rgb;
    }

    if (crdZN.y == -1) crdZN = conv_b(crdZN, fInd);
    valZN = texelFetch(texIrrd, ivec3(crdZN + fOrig, layer), 0).rgb;

    ////////////////////

    if (crdZP.y == 32) crdZP = conv_t(crdZP, fInd);
    valZP = texelFetch(texIrrd, ivec3(crdZP + fOrig, layer), 0).rgb;

    if (crdPN.x == 32 && crdPN.y == -1) {
        valPN = (valPZ + valZN) * 0.5f;
    } else {
        if (crdPN.x == 32) crdPN = conv_r(crdPN, fInd);
        else if (crdPN.y == -1) crdPN = conv_b(crdPN, fInd);
        valPN = texelFetch(texIrrd, ivec3(crdPN + fOrig, layer), 0).rgb;
    }

    if (crdPZ.x == 32) crdPZ = conv_r(crdPZ, fInd);
    valPZ = texelFetch(texIrrd, ivec3(crdPZ + fOrig, layer), 0).rgb;

    if (crdPP.x == 32 && crdPP.y == 32) {
        valPP = (valPZ + valZP) * 0.5f;
    } else {
        if (crdPP.x == 32) crdPP = conv_r(crdPP, fInd);
        else if (crdPP.y == 32) crdPP = conv_t(crdPP, fInd);
        valPP = texelFetch(texIrrd, ivec3(crdPP + fOrig, layer), 0).rgb;
    }

    ////////////////////

    fragColour = (value + valNN + valNZ + valNP + valZN + valZP + valPN + valPZ + valPP) / 8.f;

//    fragColour = vec3(float(probe) / float(14*8*8));
//    fragColour = vec3(0.5f);
}

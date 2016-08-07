const float data_ConeVertices[18*3] = {
    -0.84777, +0.56646, -1.0,   0.0, 0.0, 0.0,
    -0.56646, +0.84777, -1.0,   -0.19892, +1.00000, -1.0,
    +0.19892, +1.00000, -1.0,   -1.00000, +0.19892, -1.0,
    -1.00000, -0.19892, -1.0,   -0.84777, -0.56646, -1.0,
    -0.56646, -0.84777, -1.0,   -0.19892, -1.00000, -1.0,
    +0.19892, -1.00000, -1.0,   +0.56646, -0.84777, -1.0,
    +0.84777, -0.56646, -1.0,   +1.00000, -0.19892, -1.0,
    +1.00000, +0.19892, -1.0,   +0.84777, +0.56646, -1.0,
    +0.56646, +0.84777, -1.0,   0.0, 0.0, -1.0
};

const unsigned char data_ConeIndices[32*3] = {
     0,  1,  2,    3,  1,  4,    5,  1,  0,    6,  1,  5,
     7,  1,  6,    8,  1,  7,    9,  1,  8,   10,  1,  9,
    11,  1, 10,   12,  1, 11,   13,  1, 12,   14,  1, 13,
    15,  1, 14,   16,  1, 15,    2,  1,  3,    4,  1, 16,
    10, 17, 11,   11, 17, 12,    3, 17,  2,   13, 17, 14,
    16, 17,  4,   12, 17, 13,   14, 17, 15,    9, 17, 10,
     2, 17,  0,    4, 17,  3,   15, 17, 16,    5, 17,  6,
     8, 17,  9,    0, 17,  5,    6, 17,  7,    7, 17,  8
};


const float data_SphereVertices[24*3] = {
    -0.41422, +0.41422, +1.00000,   -0.41422, -0.41422, +1.00000,
    +0.41422, +0.41422, +1.00000,   +0.41422, +1.00000, -0.41422,
    -0.41422, +1.00000, -0.41422,   -0.41422, +1.00000, +0.41422,
    -0.41422, -1.00000, +0.41422,   -1.00000, -0.41422, +0.41422,
    -1.00000, -0.41422, -0.41422,   -0.41422, -1.00000, -0.41422,
    +0.41422, -1.00000, -0.41422,   +0.41422, -1.00000, +0.41422,
    +1.00000, +0.41422, +0.41422,   +1.00000, -0.41422, +0.41422,
    +1.00000, -0.41422, -0.41422,   +0.41422, -0.41422, -1.00000,
    -0.41422, +0.41422, -1.00000,   +0.41422, +0.41422, -1.00000,
    +0.41422, -0.41422, +1.00000,   +0.41422, +1.00000, +0.41422,
    -1.00000, +0.41422, +0.41422,   +1.00000, +0.41422, -0.41422,
    -1.00000, +0.41422, -0.41422,   -0.41422, -0.41422, -1.00000,
};

const unsigned char data_SphereIndices[44*3] = {
     0,  1,  2,    3,  4,  5,    6,  7,  8,    9, 10, 11,
    12, 13, 14,   15, 16, 17,   13, 18, 11,   19,  2, 12,
    20,  0,  5,    1,  7,  6,   14, 10, 15,   17,  3, 21,
    22,  4, 16,    9,  8, 23,    1, 11, 18,   19,  3,  5,
     0, 20,  7,   12,  2, 18,   17, 16,  3,   20,  4, 22,
     1, 18,  2,   22,  8,  7,    5,  0,  2,   13, 10, 14,
    15, 10,  9,   20, 22,  7,    6,  9, 11,   21, 12, 14,
    19, 12, 21,   15, 23, 16,    1,  6, 11,    1,  0,  7,
    19,  5,  2,   13, 12, 18,    3, 19, 21,   14, 15, 17,
    20,  5,  4,   21, 14, 17,    9,  6,  8,   23, 15,  9,
    13, 11, 10,   16,  4,  3,   22, 16, 23,    8, 22, 23
};

//const float data_SphereVertices[42*3] = {
// 0.000000,  0.000000, -0.537500,    0.228611, -0.166094, -0.457227,
//-0.087320, -0.268747, -0.457227,    0.388939, -0.282577, -0.240380,
// 0.457223,  0.000000, -0.282583,   -0.282580,  0.000000, -0.457225,
//-0.087320,  0.268747, -0.457227,    0.228611,  0.166094, -0.457227,
// 0.511194, -0.166094,  0.000000,   -0.148559, -0.457224, -0.240381,
// 0.141292, -0.434844, -0.282584,    0.000000, -0.537500,  0.000000,
//-0.480754,  0.000000, -0.240378,   -0.369902, -0.268748, -0.282583,
//-0.511194, -0.166094,  0.000000,   -0.148559,  0.457224, -0.240381,
//-0.369902,  0.268748, -0.282583,   -0.315935,  0.434846,  0.000000,
// 0.388939,  0.282577, -0.240380,    0.141292,  0.434844, -0.282584,
// 0.315935,  0.434846,  0.000000,    0.315935, -0.434846,  0.000000,
//-0.315935, -0.434846,  0.000000,   -0.511194,  0.166094,  0.000000,
// 0.000000,  0.537500,  0.000000,    0.511194,  0.166094,  0.000000,
// 0.148559, -0.457224,  0.240381,    0.369902, -0.268748,  0.282583,
// 0.087320, -0.268747,  0.457227,   -0.388939, -0.282577,  0.240380,
//-0.141292, -0.434844,  0.282584,   -0.228611, -0.166094,  0.457227,
//-0.388939,  0.282577,  0.240380,   -0.457223,  0.000000,  0.282583,
//-0.228611,  0.166094,  0.457227,    0.148559,  0.457224,  0.240381,
//-0.141292,  0.434844,  0.282584,    0.087320,  0.268747,  0.457227,
// 0.480754,  0.000000,  0.240378,    0.369902,  0.268748,  0.282583,
// 0.282580,  0.000000,  0.457225,    0.000000,  0.000000,  0.537500
//};

//const unsigned char data_SphereIndices[80*3] = {
//0,  1,  2,    3,  1,  4,    0,  2,  5,    0,  5,  6,
//0,  6,  7,    3,  4,  8,    9,  10, 11,   12, 13, 14,
//15, 16, 17,   18, 19, 20,   3,  8,  21,   9,  11, 22,
//12, 14, 23,   15, 17, 24,   18, 20, 25,   26, 27, 28,
//29, 30, 31,   32, 33, 34,   35, 36, 37,   38, 39, 40,
//40, 37, 41,   40, 39, 37,   39, 35, 37,   37, 34, 41,
//37, 36, 34,   36, 32, 34,   34, 31, 41,   34, 33, 31,
//33, 29, 31,   31, 28, 41,   31, 30, 28,   30, 26, 28,
//28, 40, 41,   28, 27, 40,   27, 38, 40,   25, 39, 38,
//25, 20, 39,   20, 35, 39,   24, 36, 35,   24, 17, 36,
//17, 32, 36,   23, 33, 32,   23, 14, 33,   14, 29, 33,
//22, 30, 29,   22, 11, 30,   11, 26, 30,   21, 27, 26,
//21, 8,  27,   8,  38, 27,   20, 24, 35,   20, 19, 24,
//19, 15, 24,   17, 23, 32,   17, 16, 23,   16, 12, 23,
//14, 22, 29,   14, 13, 22,   13, 9,  22,   11, 21, 26,
//11, 10, 21,   10, 3,  21,   8,  25, 38,   8,  4,  25,
//4,  18, 25,   7,  19, 18,   7,  6,  19,   6,  15, 19,
//6,  16, 15,   6,  5,  16,   5,  12, 16,   5,  13, 12,
//5,  2,  13,   2,  9,  13,   4,  7,  18,   4,  1,  7,
//1,  0,  7,    2,  10, 9,    2,  1,  10,   1,  3,  10
//};


const float data_CubeVertices[8*3] = {
-0.5,  0.5, -0.5,   -0.5,  0.5,  0.5,
-0.5, -0.5, -0.5,    0.5,  0.5,  0.5,
 0.5,  0.5, -0.5,    0.5, -0.5,  0.5,
 0.5, -0.5, -0.5,   -0.5, -0.5,  0.5
};

const unsigned char data_CubeIndices[12*3] = {
1, 0, 2,   3, 4, 0,   5, 6, 4,   7, 2, 6,
0, 4, 6,   3, 1, 7,   7, 1, 2,   1, 3, 0,
3, 5, 4,   5, 7, 6,   2, 0, 6,   5, 3, 7
};

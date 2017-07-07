struct CameraBlock
{
    mat4 proj;    // 64
    mat4 view;    // 64
    mat4 invProj; // 64
    mat4 invView; // 64
    mat4 trnView; // 64
    vec3 pos;     // 12
    float rmin;   //  4
    vec3 dir;     // 12
    float rmax;   //  4
    
    // TOTAL: 352
};

#pragma once

#include <cstddef>
#include <cstdint>

#include <sqee/export.hpp>

//============================================================================//

// This code block is duplicated in glxext.h, so it must be protected.

//#ifndef GLEXT_64_TYPES_DEFINED
//#define GLEXT_64_TYPES_DEFINED

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef char GLchar;
typedef char GLcharARB;

//#endif

//============================================================================//

#ifdef SQEE_APPLE
    typedef void *GLhandleARB;
#else
    typedef unsigned int GLhandleARB;
#endif

//============================================================================//

typedef unsigned short GLhalfARB;
typedef unsigned short GLhalf;
typedef GLint GLfixed;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef int64_t GLint64;
typedef uint64_t GLuint64;
typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;
typedef int64_t GLint64EXT;
typedef uint64_t GLuint64EXT;
typedef struct __GLsync *GLsync;
struct _cl_context;
struct _cl_event;
typedef void (MSVC_STDCALL *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
typedef unsigned short GLhalfNV;
typedef GLintptr GLvdpauSurfaceNV;

//============================================================================//

namespace gl::exts {

class LoadTest
{
private:
    //Safe bool idiom. Joy!
    typedef void (LoadTest::*bool_type)() const;
    void big_long_name_that_really_doesnt_matter() const {}

public:
    operator bool_type() const
    {
        return m_isLoaded ? &LoadTest::big_long_name_that_really_doesnt_matter : 0;
    }

    int GetNumMissing() const {return m_numMissing;}

    LoadTest() = default;
    LoadTest(bool isLoaded, int numMissing) : m_isLoaded(isLoaded), m_numMissing(numMissing) {}

private:
    bool m_isLoaded = false;
    int m_numMissing = 0;
};

extern LoadTest var_KHR_debug;
extern LoadTest var_ARB_texture_storage;
extern LoadTest var_ARB_buffer_storage;
extern LoadTest var_ARB_direct_state_access;
extern LoadTest var_ARB_separate_shader_objects;

} // namespace gl::exts

//============================================================================//

namespace gl {

enum : GLenum {

    BUFFER                           = 0x82E0,
    CONTEXT_FLAG_DEBUG_BIT           = 0x00000002,
    DEBUG_CALLBACK_FUNCTION          = 0x8244,
    DEBUG_CALLBACK_USER_PARAM        = 0x8245,
    DEBUG_GROUP_STACK_DEPTH          = 0x826D,
    DEBUG_LOGGED_MESSAGES            = 0x9145,
    DEBUG_NEXT_LOGGED_MESSAGE_LENGTH = 0x8243,
    DEBUG_OUTPUT                     = 0x92E0,
    DEBUG_OUTPUT_SYNCHRONOUS         = 0x8242,
    DEBUG_SEVERITY_HIGH              = 0x9146,
    DEBUG_SEVERITY_LOW               = 0x9148,
    DEBUG_SEVERITY_MEDIUM            = 0x9147,
    DEBUG_SEVERITY_NOTIFICATION      = 0x826B,
    DEBUG_SOURCE_API                 = 0x8246,
    DEBUG_SOURCE_APPLICATION         = 0x824A,
    DEBUG_SOURCE_OTHER               = 0x824B,
    DEBUG_SOURCE_SHADER_COMPILER     = 0x8248,
    DEBUG_SOURCE_THIRD_PARTY         = 0x8249,
    DEBUG_SOURCE_WINDOW_SYSTEM       = 0x8247,
    DEBUG_TYPE_DEPRECATED_BEHAVIOR   = 0x824D,
    DEBUG_TYPE_ERROR                 = 0x824C,
    DEBUG_TYPE_MARKER                = 0x8268,
    DEBUG_TYPE_OTHER                 = 0x8251,
    DEBUG_TYPE_PERFORMANCE           = 0x8250,
    DEBUG_TYPE_POP_GROUP             = 0x826A,
    DEBUG_TYPE_PORTABILITY           = 0x824F,
    DEBUG_TYPE_PUSH_GROUP            = 0x8269,
    DEBUG_TYPE_UNDEFINED_BEHAVIOR    = 0x824E,
    DISPLAY_LIST                     = 0x82E7,
    MAX_DEBUG_GROUP_STACK_DEPTH      = 0x826C,
    MAX_DEBUG_LOGGED_MESSAGES        = 0x9144,
    MAX_DEBUG_MESSAGE_LENGTH         = 0x9143,
    MAX_LABEL_LENGTH                 = 0x82E8,
    PROGRAM                          = 0x82E2,
    PROGRAM_PIPELINE                 = 0x82E4,
    QUERY                            = 0x82E3,
    SAMPLER                          = 0x82E6,
    SHADER                           = 0x82E1,
    STACK_OVERFLOW                   = 0x0503,
    STACK_UNDERFLOW                  = 0x0504,
    VERTEX_ARRAY                     = 0x8074,

    TEXTURE_IMMUTABLE_FORMAT         = 0x912F,

    BUFFER_IMMUTABLE_STORAGE         = 0x821F,
    BUFFER_STORAGE_FLAGS             = 0x8220,
    CLIENT_MAPPED_BUFFER_BARRIER_BIT = 0x00004000,
    CLIENT_STORAGE_BIT               = 0x0200,
    DYNAMIC_STORAGE_BIT              = 0x0100,
    MAP_COHERENT_BIT                 = 0x0080,
    MAP_PERSISTENT_BIT               = 0x0040,
    MAP_READ_BIT                     = 0x0001,
    MAP_WRITE_BIT                    = 0x0002,

    QUERY_TARGET                     = 0x82EA,
    TEXTURE_BINDING_1D               = 0x8068,
    TEXTURE_BINDING_1D_ARRAY         = 0x8C1C,
    TEXTURE_BINDING_2D               = 0x8069,
    TEXTURE_BINDING_2D_ARRAY         = 0x8C1D,
    TEXTURE_BINDING_2D_MULTISAMPLE   = 0x9104,
    TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY = 0x9105,
    TEXTURE_BINDING_3D               = 0x806A,
    TEXTURE_BINDING_BUFFER           = 0x8C2C,
    TEXTURE_BINDING_CUBE_MAP         = 0x8514,
    TEXTURE_BINDING_CUBE_MAP_ARRAY   = 0x900A,
    TEXTURE_BINDING_RECTANGLE        = 0x84F6,
    TEXTURE_TARGET                   = 0x1006,

    ACTIVE_PROGRAM                   = 0x8259,
    ALL_SHADER_BITS                  = 0xFFFFFFFF,
    FRAGMENT_SHADER_BIT              = 0x00000002,
    GEOMETRY_SHADER_BIT              = 0x00000004,
    PROGRAM_PIPELINE_BINDING         = 0x825A,
    PROGRAM_SEPARABLE                = 0x8258,
    TESS_CONTROL_SHADER_BIT          = 0x00000008,
    TESS_EVALUATION_SHADER_BIT       = 0x00000010,
    VERTEX_SHADER_BIT                = 0x00000001,

    ALPHA                            = 0x1906,
    ALWAYS                           = 0x0207,
    AND                              = 0x1501,
    AND_INVERTED                     = 0x1504,
    AND_REVERSE                      = 0x1502,
    BACK                             = 0x0405,
    BACK_LEFT                        = 0x0402,
    BACK_RIGHT                       = 0x0403,
    BLEND                            = 0x0BE2,
    BLEND_DST                        = 0x0BE0,
    BLEND_SRC                        = 0x0BE1,
    BLUE                             = 0x1905,
    BYTE                             = 0x1400,
    CCW                              = 0x0901,
    CLEAR                            = 0x1500,
    COLOR                            = 0x1800,
    COLOR_BUFFER_BIT                 = 0x00004000,
    COLOR_CLEAR_VALUE                = 0x0C22,
    COLOR_LOGIC_OP                   = 0x0BF2,
    COLOR_WRITEMASK                  = 0x0C23,
    COPY                             = 0x1503,
    COPY_INVERTED                    = 0x150C,
    CULL_FACE                        = 0x0B44,
    CULL_FACE_MODE                   = 0x0B45,
    CW                               = 0x0900,
    DECR                             = 0x1E03,
    DEPTH                            = 0x1801,
    DEPTH_BUFFER_BIT                 = 0x00000100,
    DEPTH_CLEAR_VALUE                = 0x0B73,
    DEPTH_COMPONENT                  = 0x1902,
    DEPTH_FUNC                       = 0x0B74,
    DEPTH_RANGE                      = 0x0B70,
    DEPTH_TEST                       = 0x0B71,
    DEPTH_WRITEMASK                  = 0x0B72,
    DITHER                           = 0x0BD0,
    DONT_CARE                        = 0x1100,
    DOUBLE                           = 0x140A,
    DOUBLEBUFFER                     = 0x0C32,
    DRAW_BUFFER                      = 0x0C01,
    DST_ALPHA                        = 0x0304,
    DST_COLOR                        = 0x0306,
    EQUAL                            = 0x0202,
    EQUIV                            = 0x1509,
    EXTENSIONS                       = 0x1F03,
    FALSE_                           = 0,
    FASTEST                          = 0x1101,
    FILL                             = 0x1B02,
    FLOAT                            = 0x1406,
    FRONT                            = 0x0404,
    FRONT_AND_BACK                   = 0x0408,
    FRONT_FACE                       = 0x0B46,
    FRONT_LEFT                       = 0x0400,
    FRONT_RIGHT                      = 0x0401,
    GEQUAL                           = 0x0206,
    GREATER                          = 0x0204,
    GREEN                            = 0x1904,
    INCR                             = 0x1E02,
    INT                              = 0x1404,
    INVALID_ENUM                     = 0x0500,
    INVALID_OPERATION                = 0x0502,
    INVALID_VALUE                    = 0x0501,
    INVERT                           = 0x150A,
    KEEP                             = 0x1E00,
    LEFT                             = 0x0406,
    LEQUAL                           = 0x0203,
    LESS                             = 0x0201,
    LINE                             = 0x1B01,
    LINEAR                           = 0x2601,
    LINEAR_MIPMAP_LINEAR             = 0x2703,
    LINEAR_MIPMAP_NEAREST            = 0x2701,
    LINES                            = 0x0001,
    LINE_LOOP                        = 0x0002,
    LINE_SMOOTH                      = 0x0B20,
    LINE_SMOOTH_HINT                 = 0x0C52,
    LINE_STRIP                       = 0x0003,
    LINE_WIDTH                       = 0x0B21,
    LINE_WIDTH_GRANULARITY           = 0x0B23,
    LINE_WIDTH_RANGE                 = 0x0B22,
    LOGIC_OP_MODE                    = 0x0BF0,
    MAX_TEXTURE_SIZE                 = 0x0D33,
    MAX_VIEWPORT_DIMS                = 0x0D3A,
    NAND                             = 0x150E,
    NEAREST                          = 0x2600,
    NEAREST_MIPMAP_LINEAR            = 0x2702,
    NEAREST_MIPMAP_NEAREST           = 0x2700,
    NEVER                            = 0x0200,
    NICEST                           = 0x1102,
    NONE                             = 0,
    NOOP                             = 0x1505,
    NOR                              = 0x1508,
    NOTEQUAL                         = 0x0205,
    NO_ERROR_                        = 0,
    ONE                              = 1,
    ONE_MINUS_DST_ALPHA              = 0x0305,
    ONE_MINUS_DST_COLOR              = 0x0307,
    ONE_MINUS_SRC_ALPHA              = 0x0303,
    ONE_MINUS_SRC_COLOR              = 0x0301,
    OR                               = 0x1507,
    OR_INVERTED                      = 0x150D,
    OR_REVERSE                       = 0x150B,
    OUT_OF_MEMORY                    = 0x0505,
    PACK_ALIGNMENT                   = 0x0D05,
    PACK_LSB_FIRST                   = 0x0D01,
    PACK_ROW_LENGTH                  = 0x0D02,
    PACK_SKIP_PIXELS                 = 0x0D04,
    PACK_SKIP_ROWS                   = 0x0D03,
    PACK_SWAP_BYTES                  = 0x0D00,
    POINT                            = 0x1B00,
    POINTS                           = 0x0000,
    POINT_SIZE                       = 0x0B11,
    POINT_SIZE_GRANULARITY           = 0x0B13,
    POINT_SIZE_RANGE                 = 0x0B12,
    POLYGON_MODE                     = 0x0B40,
    POLYGON_OFFSET_FACTOR            = 0x8038,
    POLYGON_OFFSET_FILL              = 0x8037,
    POLYGON_OFFSET_LINE              = 0x2A02,
    POLYGON_OFFSET_POINT             = 0x2A01,
    POLYGON_OFFSET_UNITS             = 0x2A00,
    POLYGON_SMOOTH                   = 0x0B41,
    POLYGON_SMOOTH_HINT              = 0x0C53,
    PROXY_TEXTURE_1D                 = 0x8063,
    PROXY_TEXTURE_2D                 = 0x8064,
    R3_G3_B2                         = 0x2A10,
    READ_BUFFER                      = 0x0C02,
    RED                              = 0x1903,
    RENDERER                         = 0x1F01,
    REPEAT                           = 0x2901,
    REPLACE                          = 0x1E01,
    RGB                              = 0x1907,
    RGB10                            = 0x8052,
    RGB10_A2                         = 0x8059,
    RGB12                            = 0x8053,
    RGB16                            = 0x8054,
    RGB4                             = 0x804F,
    RGB5                             = 0x8050,
    RGB5_A1                          = 0x8057,
    RGB8                             = 0x8051,
    RGBA                             = 0x1908,
    RGBA12                           = 0x805A,
    RGBA16                           = 0x805B,
    RGBA2                            = 0x8055,
    RGBA4                            = 0x8056,
    RGBA8                            = 0x8058,
    RIGHT                            = 0x0407,
    SCISSOR_BOX                      = 0x0C10,
    SCISSOR_TEST                     = 0x0C11,
    SET                              = 0x150F,
    SHORT                            = 0x1402,
    SRC_ALPHA                        = 0x0302,
    SRC_ALPHA_SATURATE               = 0x0308,
    SRC_COLOR                        = 0x0300,
    STENCIL                          = 0x1802,
    STENCIL_BUFFER_BIT               = 0x00000400,
    STENCIL_CLEAR_VALUE              = 0x0B91,
    STENCIL_FAIL                     = 0x0B94,
    STENCIL_FUNC                     = 0x0B92,
    STENCIL_INDEX                    = 0x1901,
    STENCIL_PASS_DEPTH_FAIL          = 0x0B95,
    STENCIL_PASS_DEPTH_PASS          = 0x0B96,
    STENCIL_REF                      = 0x0B97,
    STENCIL_TEST                     = 0x0B90,
    STENCIL_VALUE_MASK               = 0x0B93,
    STENCIL_WRITEMASK                = 0x0B98,
    STEREO                           = 0x0C33,
    SUBPIXEL_BITS                    = 0x0D50,
    TEXTURE                          = 0x1702,
    TEXTURE_1D                       = 0x0DE0,
    TEXTURE_2D                       = 0x0DE1,
    TEXTURE_ALPHA_SIZE               = 0x805F,
    //TEXTURE_BINDING_1D taken from ext: ARB_direct_state_access
    //TEXTURE_BINDING_2D taken from ext: ARB_direct_state_access
    TEXTURE_BLUE_SIZE                = 0x805E,
    TEXTURE_BORDER_COLOR             = 0x1004,
    TEXTURE_GREEN_SIZE               = 0x805D,
    TEXTURE_HEIGHT                   = 0x1001,
    TEXTURE_INTERNAL_FORMAT          = 0x1003,
    TEXTURE_MAG_FILTER               = 0x2800,
    TEXTURE_MIN_FILTER               = 0x2801,
    TEXTURE_RED_SIZE                 = 0x805C,
    TEXTURE_WIDTH                    = 0x1000,
    TEXTURE_WRAP_S                   = 0x2802,
    TEXTURE_WRAP_T                   = 0x2803,
    TRIANGLES                        = 0x0004,
    TRIANGLE_FAN                     = 0x0006,
    TRIANGLE_STRIP                   = 0x0005,
    TRUE_                            = 1,
    UNPACK_ALIGNMENT                 = 0x0CF5,
    UNPACK_LSB_FIRST                 = 0x0CF1,
    UNPACK_ROW_LENGTH                = 0x0CF2,
    UNPACK_SKIP_PIXELS               = 0x0CF4,
    UNPACK_SKIP_ROWS                 = 0x0CF3,
    UNPACK_SWAP_BYTES                = 0x0CF0,
    UNSIGNED_BYTE                    = 0x1401,
    UNSIGNED_INT                     = 0x1405,
    UNSIGNED_SHORT                   = 0x1403,
    VENDOR                           = 0x1F00,
    VERSION                          = 0x1F02,
    VIEWPORT                         = 0x0BA2,
    XOR                              = 0x1506,
    ZERO                             = 0,

    ALIASED_LINE_WIDTH_RANGE         = 0x846E,
    BGR                              = 0x80E0,
    BGRA                             = 0x80E1,
    CLAMP_TO_EDGE                    = 0x812F,
    MAX_3D_TEXTURE_SIZE              = 0x8073,
    MAX_ELEMENTS_INDICES             = 0x80E9,
    MAX_ELEMENTS_VERTICES            = 0x80E8,
    PACK_IMAGE_HEIGHT                = 0x806C,
    PACK_SKIP_IMAGES                 = 0x806B,
    PROXY_TEXTURE_3D                 = 0x8070,
    SMOOTH_LINE_WIDTH_GRANULARITY    = 0x0B23,
    SMOOTH_LINE_WIDTH_RANGE          = 0x0B22,
    SMOOTH_POINT_SIZE_GRANULARITY    = 0x0B13,
    SMOOTH_POINT_SIZE_RANGE          = 0x0B12,
    TEXTURE_3D                       = 0x806F,
    TEXTURE_BASE_LEVEL               = 0x813C,
    //TEXTURE_BINDING_3D taken from ext: ARB_direct_state_access
    TEXTURE_DEPTH                    = 0x8071,
    TEXTURE_MAX_LEVEL                = 0x813D,
    TEXTURE_MAX_LOD                  = 0x813B,
    TEXTURE_MIN_LOD                  = 0x813A,
    TEXTURE_WRAP_R                   = 0x8072,
    UNPACK_IMAGE_HEIGHT              = 0x806E,
    UNPACK_SKIP_IMAGES               = 0x806D,
    UNSIGNED_BYTE_2_3_3_REV          = 0x8362,
    UNSIGNED_BYTE_3_3_2              = 0x8032,
    UNSIGNED_INT_10_10_10_2          = 0x8036,
    UNSIGNED_INT_2_10_10_10_REV      = 0x8368,
    UNSIGNED_INT_8_8_8_8             = 0x8035,
    UNSIGNED_INT_8_8_8_8_REV         = 0x8367,
    UNSIGNED_SHORT_1_5_5_5_REV       = 0x8366,
    UNSIGNED_SHORT_4_4_4_4           = 0x8033,
    UNSIGNED_SHORT_4_4_4_4_REV       = 0x8365,
    UNSIGNED_SHORT_5_5_5_1           = 0x8034,
    UNSIGNED_SHORT_5_6_5             = 0x8363,
    UNSIGNED_SHORT_5_6_5_REV         = 0x8364,

    ACTIVE_TEXTURE                   = 0x84E0,
    CLAMP_TO_BORDER                  = 0x812D,
    COMPRESSED_RGB                   = 0x84ED,
    COMPRESSED_RGBA                  = 0x84EE,
    COMPRESSED_TEXTURE_FORMATS       = 0x86A3,
    MAX_CUBE_MAP_TEXTURE_SIZE        = 0x851C,
    MULTISAMPLE                      = 0x809D,
    NUM_COMPRESSED_TEXTURE_FORMATS   = 0x86A2,
    PROXY_TEXTURE_CUBE_MAP           = 0x851B,
    SAMPLES                          = 0x80A9,
    SAMPLE_ALPHA_TO_COVERAGE         = 0x809E,
    SAMPLE_ALPHA_TO_ONE              = 0x809F,
    SAMPLE_BUFFERS                   = 0x80A8,
    SAMPLE_COVERAGE                  = 0x80A0,
    SAMPLE_COVERAGE_INVERT           = 0x80AB,
    SAMPLE_COVERAGE_VALUE            = 0x80AA,
    TEXTURE0                         = 0x84C0,
    TEXTURE1                         = 0x84C1,
    TEXTURE10                        = 0x84CA,
    TEXTURE11                        = 0x84CB,
    TEXTURE12                        = 0x84CC,
    TEXTURE13                        = 0x84CD,
    TEXTURE14                        = 0x84CE,
    TEXTURE15                        = 0x84CF,
    TEXTURE16                        = 0x84D0,
    TEXTURE17                        = 0x84D1,
    TEXTURE18                        = 0x84D2,
    TEXTURE19                        = 0x84D3,
    TEXTURE2                         = 0x84C2,
    TEXTURE20                        = 0x84D4,
    TEXTURE21                        = 0x84D5,
    TEXTURE22                        = 0x84D6,
    TEXTURE23                        = 0x84D7,
    TEXTURE24                        = 0x84D8,
    TEXTURE25                        = 0x84D9,
    TEXTURE26                        = 0x84DA,
    TEXTURE27                        = 0x84DB,
    TEXTURE28                        = 0x84DC,
    TEXTURE29                        = 0x84DD,
    TEXTURE3                         = 0x84C3,
    TEXTURE30                        = 0x84DE,
    TEXTURE31                        = 0x84DF,
    TEXTURE4                         = 0x84C4,
    TEXTURE5                         = 0x84C5,
    TEXTURE6                         = 0x84C6,
    TEXTURE7                         = 0x84C7,
    TEXTURE8                         = 0x84C8,
    TEXTURE9                         = 0x84C9,
    //TEXTURE_BINDING_CUBE_MAP taken from ext: ARB_direct_state_access
    TEXTURE_COMPRESSED               = 0x86A1,
    TEXTURE_COMPRESSED_IMAGE_SIZE    = 0x86A0,
    TEXTURE_COMPRESSION_HINT         = 0x84EF,
    TEXTURE_CUBE_MAP                 = 0x8513,
    TEXTURE_CUBE_MAP_NEGATIVE_X      = 0x8516,
    TEXTURE_CUBE_MAP_NEGATIVE_Y      = 0x8518,
    TEXTURE_CUBE_MAP_NEGATIVE_Z      = 0x851A,
    TEXTURE_CUBE_MAP_POSITIVE_X      = 0x8515,
    TEXTURE_CUBE_MAP_POSITIVE_Y      = 0x8517,
    TEXTURE_CUBE_MAP_POSITIVE_Z      = 0x8519,

    BLEND_COLOR                      = 0x8005,
    BLEND_DST_ALPHA                  = 0x80CA,
    BLEND_DST_RGB                    = 0x80C8,
    BLEND_SRC_ALPHA                  = 0x80CB,
    BLEND_SRC_RGB                    = 0x80C9,
    CONSTANT_ALPHA                   = 0x8003,
    CONSTANT_COLOR                   = 0x8001,
    DECR_WRAP                        = 0x8508,
    DEPTH_COMPONENT16                = 0x81A5,
    DEPTH_COMPONENT24                = 0x81A6,
    DEPTH_COMPONENT32                = 0x81A7,
    FUNC_ADD                         = 0x8006,
    FUNC_REVERSE_SUBTRACT            = 0x800B,
    FUNC_SUBTRACT                    = 0x800A,
    INCR_WRAP                        = 0x8507,
    MAX                              = 0x8008,
    MAX_TEXTURE_LOD_BIAS             = 0x84FD,
    MIN                              = 0x8007,
    MIRRORED_REPEAT                  = 0x8370,
    ONE_MINUS_CONSTANT_ALPHA         = 0x8004,
    ONE_MINUS_CONSTANT_COLOR         = 0x8002,
    POINT_FADE_THRESHOLD_SIZE        = 0x8128,
    TEXTURE_COMPARE_FUNC             = 0x884D,
    TEXTURE_COMPARE_MODE             = 0x884C,
    TEXTURE_DEPTH_SIZE               = 0x884A,
    TEXTURE_LOD_BIAS                 = 0x8501,

    ARRAY_BUFFER                     = 0x8892,
    ARRAY_BUFFER_BINDING             = 0x8894,
    BUFFER_ACCESS                    = 0x88BB,
    BUFFER_MAPPED                    = 0x88BC,
    BUFFER_MAP_POINTER               = 0x88BD,
    BUFFER_SIZE                      = 0x8764,
    BUFFER_USAGE                     = 0x8765,
    CURRENT_QUERY                    = 0x8865,
    DYNAMIC_COPY                     = 0x88EA,
    DYNAMIC_DRAW                     = 0x88E8,
    DYNAMIC_READ                     = 0x88E9,
    ELEMENT_ARRAY_BUFFER             = 0x8893,
    ELEMENT_ARRAY_BUFFER_BINDING     = 0x8895,
    QUERY_COUNTER_BITS               = 0x8864,
    QUERY_RESULT                     = 0x8866,
    QUERY_RESULT_AVAILABLE           = 0x8867,
    READ_ONLY                        = 0x88B8,
    READ_WRITE                       = 0x88BA,
    SAMPLES_PASSED                   = 0x8914,
    SRC1_ALPHA                       = 0x8589,
    STATIC_COPY                      = 0x88E6,
    STATIC_DRAW                      = 0x88E4,
    STATIC_READ                      = 0x88E5,
    STREAM_COPY                      = 0x88E2,
    STREAM_DRAW                      = 0x88E0,
    STREAM_READ                      = 0x88E1,
    VERTEX_ATTRIB_ARRAY_BUFFER_BINDING = 0x889F,
    WRITE_ONLY                       = 0x88B9,

    ACTIVE_ATTRIBUTES                = 0x8B89,
    ACTIVE_ATTRIBUTE_MAX_LENGTH      = 0x8B8A,
    ACTIVE_UNIFORMS                  = 0x8B86,
    ACTIVE_UNIFORM_MAX_LENGTH        = 0x8B87,
    ATTACHED_SHADERS                 = 0x8B85,
    BLEND_EQUATION_ALPHA             = 0x883D,
    BLEND_EQUATION_RGB               = 0x8009,
    BOOL                             = 0x8B56,
    BOOL_VEC2                        = 0x8B57,
    BOOL_VEC3                        = 0x8B58,
    BOOL_VEC4                        = 0x8B59,
    COMPILE_STATUS                   = 0x8B81,
    CURRENT_PROGRAM                  = 0x8B8D,
    CURRENT_VERTEX_ATTRIB            = 0x8626,
    DELETE_STATUS                    = 0x8B80,
    DRAW_BUFFER0                     = 0x8825,
    DRAW_BUFFER1                     = 0x8826,
    DRAW_BUFFER10                    = 0x882F,
    DRAW_BUFFER11                    = 0x8830,
    DRAW_BUFFER12                    = 0x8831,
    DRAW_BUFFER13                    = 0x8832,
    DRAW_BUFFER14                    = 0x8833,
    DRAW_BUFFER15                    = 0x8834,
    DRAW_BUFFER2                     = 0x8827,
    DRAW_BUFFER3                     = 0x8828,
    DRAW_BUFFER4                     = 0x8829,
    DRAW_BUFFER5                     = 0x882A,
    DRAW_BUFFER6                     = 0x882B,
    DRAW_BUFFER7                     = 0x882C,
    DRAW_BUFFER8                     = 0x882D,
    DRAW_BUFFER9                     = 0x882E,
    FLOAT_MAT2                       = 0x8B5A,
    FLOAT_MAT3                       = 0x8B5B,
    FLOAT_MAT4                       = 0x8B5C,
    FLOAT_VEC2                       = 0x8B50,
    FLOAT_VEC3                       = 0x8B51,
    FLOAT_VEC4                       = 0x8B52,
    FRAGMENT_SHADER                  = 0x8B30,
    FRAGMENT_SHADER_DERIVATIVE_HINT  = 0x8B8B,
    INFO_LOG_LENGTH                  = 0x8B84,
    INT_VEC2                         = 0x8B53,
    INT_VEC3                         = 0x8B54,
    INT_VEC4                         = 0x8B55,
    LINK_STATUS                      = 0x8B82,
    LOWER_LEFT                       = 0x8CA1,
    MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D,
    MAX_DRAW_BUFFERS                 = 0x8824,
    MAX_FRAGMENT_UNIFORM_COMPONENTS  = 0x8B49,
    MAX_TEXTURE_IMAGE_UNITS          = 0x8872,
    MAX_VARYING_FLOATS               = 0x8B4B,
    MAX_VERTEX_ATTRIBS               = 0x8869,
    MAX_VERTEX_TEXTURE_IMAGE_UNITS   = 0x8B4C,
    MAX_VERTEX_UNIFORM_COMPONENTS    = 0x8B4A,
    POINT_SPRITE_COORD_ORIGIN        = 0x8CA0,
    SAMPLER_1D                       = 0x8B5D,
    SAMPLER_1D_SHADOW                = 0x8B61,
    SAMPLER_2D                       = 0x8B5E,
    SAMPLER_2D_SHADOW                = 0x8B62,
    SAMPLER_3D                       = 0x8B5F,
    SAMPLER_CUBE                     = 0x8B60,
    SHADER_SOURCE_LENGTH             = 0x8B88,
    SHADER_TYPE                      = 0x8B4F,
    SHADING_LANGUAGE_VERSION         = 0x8B8C,
    STENCIL_BACK_FAIL                = 0x8801,
    STENCIL_BACK_FUNC                = 0x8800,
    STENCIL_BACK_PASS_DEPTH_FAIL     = 0x8802,
    STENCIL_BACK_PASS_DEPTH_PASS     = 0x8803,
    STENCIL_BACK_REF                 = 0x8CA3,
    STENCIL_BACK_VALUE_MASK          = 0x8CA4,
    STENCIL_BACK_WRITEMASK           = 0x8CA5,
    UPPER_LEFT                       = 0x8CA2,
    VALIDATE_STATUS                  = 0x8B83,
    VERTEX_ATTRIB_ARRAY_ENABLED      = 0x8622,
    VERTEX_ATTRIB_ARRAY_NORMALIZED   = 0x886A,
    VERTEX_ATTRIB_ARRAY_POINTER      = 0x8645,
    VERTEX_ATTRIB_ARRAY_SIZE         = 0x8623,
    VERTEX_ATTRIB_ARRAY_STRIDE       = 0x8624,
    VERTEX_ATTRIB_ARRAY_TYPE         = 0x8625,
    VERTEX_PROGRAM_POINT_SIZE        = 0x8642,
    VERTEX_SHADER                    = 0x8B31,

    COMPRESSED_SRGB                  = 0x8C48,
    COMPRESSED_SRGB_ALPHA            = 0x8C49,
    FLOAT_MAT2x3                     = 0x8B65,
    FLOAT_MAT2x4                     = 0x8B66,
    FLOAT_MAT3x2                     = 0x8B67,
    FLOAT_MAT3x4                     = 0x8B68,
    FLOAT_MAT4x2                     = 0x8B69,
    FLOAT_MAT4x3                     = 0x8B6A,
    PIXEL_PACK_BUFFER                = 0x88EB,
    PIXEL_PACK_BUFFER_BINDING        = 0x88ED,
    PIXEL_UNPACK_BUFFER              = 0x88EC,
    PIXEL_UNPACK_BUFFER_BINDING      = 0x88EF,
    SRGB                             = 0x8C40,
    SRGB8                            = 0x8C41,
    SRGB8_ALPHA8                     = 0x8C43,
    SRGB_ALPHA                       = 0x8C42,

    BGRA_INTEGER                     = 0x8D9B,
    BGR_INTEGER                      = 0x8D9A,
    BLUE_INTEGER                     = 0x8D96,
    BUFFER_ACCESS_FLAGS              = 0x911F,
    BUFFER_MAP_LENGTH                = 0x9120,
    BUFFER_MAP_OFFSET                = 0x9121,
    CLAMP_READ_COLOR                 = 0x891C,
    CLIP_DISTANCE0                   = 0x3000,
    CLIP_DISTANCE1                   = 0x3001,
    CLIP_DISTANCE2                   = 0x3002,
    CLIP_DISTANCE3                   = 0x3003,
    CLIP_DISTANCE4                   = 0x3004,
    CLIP_DISTANCE5                   = 0x3005,
    CLIP_DISTANCE6                   = 0x3006,
    CLIP_DISTANCE7                   = 0x3007,
    COLOR_ATTACHMENT0                = 0x8CE0,
    COLOR_ATTACHMENT1                = 0x8CE1,
    COLOR_ATTACHMENT10               = 0x8CEA,
    COLOR_ATTACHMENT11               = 0x8CEB,
    COLOR_ATTACHMENT12               = 0x8CEC,
    COLOR_ATTACHMENT13               = 0x8CED,
    COLOR_ATTACHMENT14               = 0x8CEE,
    COLOR_ATTACHMENT15               = 0x8CEF,
    COLOR_ATTACHMENT16               = 0x8CF0,
    COLOR_ATTACHMENT17               = 0x8CF1,
    COLOR_ATTACHMENT18               = 0x8CF2,
    COLOR_ATTACHMENT19               = 0x8CF3,
    COLOR_ATTACHMENT2                = 0x8CE2,
    COLOR_ATTACHMENT20               = 0x8CF4,
    COLOR_ATTACHMENT21               = 0x8CF5,
    COLOR_ATTACHMENT22               = 0x8CF6,
    COLOR_ATTACHMENT23               = 0x8CF7,
    COLOR_ATTACHMENT24               = 0x8CF8,
    COLOR_ATTACHMENT25               = 0x8CF9,
    COLOR_ATTACHMENT26               = 0x8CFA,
    COLOR_ATTACHMENT27               = 0x8CFB,
    COLOR_ATTACHMENT28               = 0x8CFC,
    COLOR_ATTACHMENT29               = 0x8CFD,
    COLOR_ATTACHMENT3                = 0x8CE3,
    COLOR_ATTACHMENT30               = 0x8CFE,
    COLOR_ATTACHMENT31               = 0x8CFF,
    COLOR_ATTACHMENT4                = 0x8CE4,
    COLOR_ATTACHMENT5                = 0x8CE5,
    COLOR_ATTACHMENT6                = 0x8CE6,
    COLOR_ATTACHMENT7                = 0x8CE7,
    COLOR_ATTACHMENT8                = 0x8CE8,
    COLOR_ATTACHMENT9                = 0x8CE9,
    COMPARE_REF_TO_TEXTURE           = 0x884E,
    COMPRESSED_RED                   = 0x8225,
    COMPRESSED_RED_RGTC1             = 0x8DBB,
    COMPRESSED_RG                    = 0x8226,
    COMPRESSED_RG_RGTC2              = 0x8DBD,
    COMPRESSED_SIGNED_RED_RGTC1      = 0x8DBC,
    COMPRESSED_SIGNED_RG_RGTC2       = 0x8DBE,
    CONTEXT_FLAGS                    = 0x821E,
    CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT = 0x00000001,
    DEPTH24_STENCIL8                 = 0x88F0,
    DEPTH32F_STENCIL8                = 0x8CAD,
    DEPTH_ATTACHMENT                 = 0x8D00,
    DEPTH_COMPONENT32F               = 0x8CAC,
    DEPTH_STENCIL                    = 0x84F9,
    DEPTH_STENCIL_ATTACHMENT         = 0x821A,
    DRAW_FRAMEBUFFER                 = 0x8CA9,
    DRAW_FRAMEBUFFER_BINDING         = 0x8CA6,
    FIXED_ONLY                       = 0x891D,
    FLOAT_32_UNSIGNED_INT_24_8_REV   = 0x8DAD,
    FRAMEBUFFER                      = 0x8D40,
    FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE = 0x8215,
    FRAMEBUFFER_ATTACHMENT_BLUE_SIZE = 0x8214,
    FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING = 0x8210,
    FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE = 0x8211,
    FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE = 0x8216,
    FRAMEBUFFER_ATTACHMENT_GREEN_SIZE = 0x8213,
    FRAMEBUFFER_ATTACHMENT_OBJECT_NAME = 0x8CD1,
    FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = 0x8CD0,
    FRAMEBUFFER_ATTACHMENT_RED_SIZE  = 0x8212,
    FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE = 0x8217,
    FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE = 0x8CD3,
    FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER = 0x8CD4,
    FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL = 0x8CD2,
    FRAMEBUFFER_BINDING              = 0x8CA6,
    FRAMEBUFFER_COMPLETE             = 0x8CD5,
    FRAMEBUFFER_DEFAULT              = 0x8218,
    FRAMEBUFFER_INCOMPLETE_ATTACHMENT = 0x8CD6,
    FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER = 0x8CDB,
    FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT = 0x8CD7,
    FRAMEBUFFER_INCOMPLETE_MULTISAMPLE = 0x8D56,
    FRAMEBUFFER_INCOMPLETE_READ_BUFFER = 0x8CDC,
    FRAMEBUFFER_SRGB                 = 0x8DB9,
    FRAMEBUFFER_UNDEFINED            = 0x8219,
    FRAMEBUFFER_UNSUPPORTED          = 0x8CDD,
    GREEN_INTEGER                    = 0x8D95,
    HALF_FLOAT                       = 0x140B,
    INTERLEAVED_ATTRIBS              = 0x8C8C,
    INT_SAMPLER_1D                   = 0x8DC9,
    INT_SAMPLER_1D_ARRAY             = 0x8DCE,
    INT_SAMPLER_2D                   = 0x8DCA,
    INT_SAMPLER_2D_ARRAY             = 0x8DCF,
    INT_SAMPLER_3D                   = 0x8DCB,
    INT_SAMPLER_CUBE                 = 0x8DCC,
    INVALID_FRAMEBUFFER_OPERATION    = 0x0506,
    MAJOR_VERSION                    = 0x821B,
    MAP_FLUSH_EXPLICIT_BIT           = 0x0010,
    MAP_INVALIDATE_BUFFER_BIT        = 0x0008,
    MAP_INVALIDATE_RANGE_BIT         = 0x0004,
    //MAP_READ_BIT taken from ext: ARB_buffer_storage
    MAP_UNSYNCHRONIZED_BIT           = 0x0020,
    //MAP_WRITE_BIT taken from ext: ARB_buffer_storage
    MAX_ARRAY_TEXTURE_LAYERS         = 0x88FF,
    MAX_CLIP_DISTANCES               = 0x0D32,
    MAX_COLOR_ATTACHMENTS            = 0x8CDF,
    MAX_PROGRAM_TEXEL_OFFSET         = 0x8905,
    MAX_RENDERBUFFER_SIZE            = 0x84E8,
    MAX_SAMPLES                      = 0x8D57,
    MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS = 0x8C8A,
    MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS = 0x8C8B,
    MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS = 0x8C80,
    MAX_VARYING_COMPONENTS           = 0x8B4B,
    MINOR_VERSION                    = 0x821C,
    MIN_PROGRAM_TEXEL_OFFSET         = 0x8904,
    NUM_EXTENSIONS                   = 0x821D,
    PRIMITIVES_GENERATED             = 0x8C87,
    PROXY_TEXTURE_1D_ARRAY           = 0x8C19,
    PROXY_TEXTURE_2D_ARRAY           = 0x8C1B,
    QUERY_BY_REGION_NO_WAIT          = 0x8E16,
    QUERY_BY_REGION_WAIT             = 0x8E15,
    QUERY_NO_WAIT                    = 0x8E14,
    QUERY_WAIT                       = 0x8E13,
    R11F_G11F_B10F                   = 0x8C3A,
    R16                              = 0x822A,
    R16F                             = 0x822D,
    R16I                             = 0x8233,
    R16UI                            = 0x8234,
    R32F                             = 0x822E,
    R32I                             = 0x8235,
    R32UI                            = 0x8236,
    R8                               = 0x8229,
    R8I                              = 0x8231,
    R8UI                             = 0x8232,
    RASTERIZER_DISCARD               = 0x8C89,
    READ_FRAMEBUFFER                 = 0x8CA8,
    READ_FRAMEBUFFER_BINDING         = 0x8CAA,
    RED_INTEGER                      = 0x8D94,
    RENDERBUFFER                     = 0x8D41,
    RENDERBUFFER_ALPHA_SIZE          = 0x8D53,
    RENDERBUFFER_BINDING             = 0x8CA7,
    RENDERBUFFER_BLUE_SIZE           = 0x8D52,
    RENDERBUFFER_DEPTH_SIZE          = 0x8D54,
    RENDERBUFFER_GREEN_SIZE          = 0x8D51,
    RENDERBUFFER_HEIGHT              = 0x8D43,
    RENDERBUFFER_INTERNAL_FORMAT     = 0x8D44,
    RENDERBUFFER_RED_SIZE            = 0x8D50,
    RENDERBUFFER_SAMPLES             = 0x8CAB,
    RENDERBUFFER_STENCIL_SIZE        = 0x8D55,
    RENDERBUFFER_WIDTH               = 0x8D42,
    RG                               = 0x8227,
    RG16                             = 0x822C,
    RG16F                            = 0x822F,
    RG16I                            = 0x8239,
    RG16UI                           = 0x823A,
    RG32F                            = 0x8230,
    RG32I                            = 0x823B,
    RG32UI                           = 0x823C,
    RG8                              = 0x822B,
    RG8I                             = 0x8237,
    RG8UI                            = 0x8238,
    RGB16F                           = 0x881B,
    RGB16I                           = 0x8D89,
    RGB16UI                          = 0x8D77,
    RGB32F                           = 0x8815,
    RGB32I                           = 0x8D83,
    RGB32UI                          = 0x8D71,
    RGB8I                            = 0x8D8F,
    RGB8UI                           = 0x8D7D,
    RGB9_E5                          = 0x8C3D,
    RGBA16F                          = 0x881A,
    RGBA16I                          = 0x8D88,
    RGBA16UI                         = 0x8D76,
    RGBA32F                          = 0x8814,
    RGBA32I                          = 0x8D82,
    RGBA32UI                         = 0x8D70,
    RGBA8I                           = 0x8D8E,
    RGBA8UI                          = 0x8D7C,
    RGBA_INTEGER                     = 0x8D99,
    RGB_INTEGER                      = 0x8D98,
    RG_INTEGER                       = 0x8228,
    SAMPLER_1D_ARRAY                 = 0x8DC0,
    SAMPLER_1D_ARRAY_SHADOW          = 0x8DC3,
    SAMPLER_2D_ARRAY                 = 0x8DC1,
    SAMPLER_2D_ARRAY_SHADOW          = 0x8DC4,
    SAMPLER_CUBE_SHADOW              = 0x8DC5,
    SEPARATE_ATTRIBS                 = 0x8C8D,
    STENCIL_ATTACHMENT               = 0x8D20,
    STENCIL_INDEX1                   = 0x8D46,
    STENCIL_INDEX16                  = 0x8D49,
    STENCIL_INDEX4                   = 0x8D47,
    STENCIL_INDEX8                   = 0x8D48,
    TEXTURE_1D_ARRAY                 = 0x8C18,
    TEXTURE_2D_ARRAY                 = 0x8C1A,
    TEXTURE_ALPHA_TYPE               = 0x8C13,
    //TEXTURE_BINDING_1D_ARRAY taken from ext: ARB_direct_state_access
    //TEXTURE_BINDING_2D_ARRAY taken from ext: ARB_direct_state_access
    TEXTURE_BLUE_TYPE                = 0x8C12,
    TEXTURE_DEPTH_TYPE               = 0x8C16,
    TEXTURE_GREEN_TYPE               = 0x8C11,
    TEXTURE_RED_TYPE                 = 0x8C10,
    TEXTURE_SHARED_SIZE              = 0x8C3F,
    TEXTURE_STENCIL_SIZE             = 0x88F1,
    TRANSFORM_FEEDBACK_BUFFER        = 0x8C8E,
    TRANSFORM_FEEDBACK_BUFFER_BINDING = 0x8C8F,
    TRANSFORM_FEEDBACK_BUFFER_MODE   = 0x8C7F,
    TRANSFORM_FEEDBACK_BUFFER_SIZE   = 0x8C85,
    TRANSFORM_FEEDBACK_BUFFER_START  = 0x8C84,
    TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = 0x8C88,
    TRANSFORM_FEEDBACK_VARYINGS      = 0x8C83,
    TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH = 0x8C76,
    UNSIGNED_INT_10F_11F_11F_REV     = 0x8C3B,
    UNSIGNED_INT_24_8                = 0x84FA,
    UNSIGNED_INT_5_9_9_9_REV         = 0x8C3E,
    UNSIGNED_INT_SAMPLER_1D          = 0x8DD1,
    UNSIGNED_INT_SAMPLER_1D_ARRAY    = 0x8DD6,
    UNSIGNED_INT_SAMPLER_2D          = 0x8DD2,
    UNSIGNED_INT_SAMPLER_2D_ARRAY    = 0x8DD7,
    UNSIGNED_INT_SAMPLER_3D          = 0x8DD3,
    UNSIGNED_INT_SAMPLER_CUBE        = 0x8DD4,
    UNSIGNED_INT_VEC2                = 0x8DC6,
    UNSIGNED_INT_VEC3                = 0x8DC7,
    UNSIGNED_INT_VEC4                = 0x8DC8,
    UNSIGNED_NORMALIZED              = 0x8C17,
    VERTEX_ARRAY_BINDING             = 0x85B5,
    VERTEX_ATTRIB_ARRAY_INTEGER      = 0x88FD,

    ACTIVE_UNIFORM_BLOCKS            = 0x8A36,
    ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH = 0x8A35,
    COPY_READ_BUFFER                 = 0x8F36,
    COPY_WRITE_BUFFER                = 0x8F37,
    INT_SAMPLER_2D_RECT              = 0x8DCD,
    INT_SAMPLER_BUFFER               = 0x8DD0,
    INVALID_INDEX                    = 0xFFFFFFFF,
    MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS = 0x8A33,
    MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS = 0x8A32,
    MAX_COMBINED_UNIFORM_BLOCKS      = 0x8A2E,
    MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS = 0x8A31,
    MAX_FRAGMENT_UNIFORM_BLOCKS      = 0x8A2D,
    MAX_GEOMETRY_UNIFORM_BLOCKS      = 0x8A2C,
    MAX_RECTANGLE_TEXTURE_SIZE       = 0x84F8,
    MAX_TEXTURE_BUFFER_SIZE          = 0x8C2B,
    MAX_UNIFORM_BLOCK_SIZE           = 0x8A30,
    MAX_UNIFORM_BUFFER_BINDINGS      = 0x8A2F,
    MAX_VERTEX_UNIFORM_BLOCKS        = 0x8A2B,
    PRIMITIVE_RESTART                = 0x8F9D,
    PRIMITIVE_RESTART_INDEX          = 0x8F9E,
    PROXY_TEXTURE_RECTANGLE          = 0x84F7,
    R16_SNORM                        = 0x8F98,
    R8_SNORM                         = 0x8F94,
    RG16_SNORM                       = 0x8F99,
    RG8_SNORM                        = 0x8F95,
    RGB16_SNORM                      = 0x8F9A,
    RGB8_SNORM                       = 0x8F96,
    RGBA16_SNORM                     = 0x8F9B,
    RGBA8_SNORM                      = 0x8F97,
    SAMPLER_2D_RECT                  = 0x8B63,
    SAMPLER_2D_RECT_SHADOW           = 0x8B64,
    SAMPLER_BUFFER                   = 0x8DC2,
    SIGNED_NORMALIZED                = 0x8F9C,
    //TEXTURE_BINDING_BUFFER taken from ext: ARB_direct_state_access
    //TEXTURE_BINDING_RECTANGLE taken from ext: ARB_direct_state_access
    TEXTURE_BUFFER                   = 0x8C2A,
    TEXTURE_BUFFER_DATA_STORE_BINDING = 0x8C2D,
    TEXTURE_RECTANGLE                = 0x84F5,
    UNIFORM_ARRAY_STRIDE             = 0x8A3C,
    UNIFORM_BLOCK_ACTIVE_UNIFORMS    = 0x8A42,
    UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES = 0x8A43,
    UNIFORM_BLOCK_BINDING            = 0x8A3F,
    UNIFORM_BLOCK_DATA_SIZE          = 0x8A40,
    UNIFORM_BLOCK_INDEX              = 0x8A3A,
    UNIFORM_BLOCK_NAME_LENGTH        = 0x8A41,
    UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER = 0x8A46,
    UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER = 0x8A45,
    UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER = 0x8A44,
    UNIFORM_BUFFER                   = 0x8A11,
    UNIFORM_BUFFER_BINDING           = 0x8A28,
    UNIFORM_BUFFER_OFFSET_ALIGNMENT  = 0x8A34,
    UNIFORM_BUFFER_SIZE              = 0x8A2A,
    UNIFORM_BUFFER_START             = 0x8A29,
    UNIFORM_IS_ROW_MAJOR             = 0x8A3E,
    UNIFORM_MATRIX_STRIDE            = 0x8A3D,
    UNIFORM_NAME_LENGTH              = 0x8A39,
    UNIFORM_OFFSET                   = 0x8A3B,
    UNIFORM_SIZE                     = 0x8A38,
    UNIFORM_TYPE                     = 0x8A37,
    UNSIGNED_INT_SAMPLER_2D_RECT     = 0x8DD5,
    UNSIGNED_INT_SAMPLER_BUFFER      = 0x8DD8,

    ALREADY_SIGNALED                 = 0x911A,
    CONDITION_SATISFIED              = 0x911C,
    CONTEXT_COMPATIBILITY_PROFILE_BIT = 0x00000002,
    CONTEXT_CORE_PROFILE_BIT         = 0x00000001,
    CONTEXT_PROFILE_MASK             = 0x9126,
    DEPTH_CLAMP                      = 0x864F,
    FIRST_VERTEX_CONVENTION          = 0x8E4D,
    FRAMEBUFFER_ATTACHMENT_LAYERED   = 0x8DA7,
    FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS = 0x8DA8,
    GEOMETRY_INPUT_TYPE              = 0x8917,
    GEOMETRY_OUTPUT_TYPE             = 0x8918,
    GEOMETRY_SHADER                  = 0x8DD9,
    GEOMETRY_VERTICES_OUT            = 0x8916,
    INT_SAMPLER_2D_MULTISAMPLE       = 0x9109,
    INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910C,
    LAST_VERTEX_CONVENTION           = 0x8E4E,
    LINES_ADJACENCY                  = 0x000A,
    LINE_STRIP_ADJACENCY             = 0x000B,
    MAX_COLOR_TEXTURE_SAMPLES        = 0x910E,
    MAX_DEPTH_TEXTURE_SAMPLES        = 0x910F,
    MAX_FRAGMENT_INPUT_COMPONENTS    = 0x9125,
    MAX_GEOMETRY_INPUT_COMPONENTS    = 0x9123,
    MAX_GEOMETRY_OUTPUT_COMPONENTS   = 0x9124,
    MAX_GEOMETRY_OUTPUT_VERTICES     = 0x8DE0,
    MAX_GEOMETRY_TEXTURE_IMAGE_UNITS = 0x8C29,
    MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS = 0x8DE1,
    MAX_GEOMETRY_UNIFORM_COMPONENTS  = 0x8DDF,
    MAX_INTEGER_SAMPLES              = 0x9110,
    MAX_SAMPLE_MASK_WORDS            = 0x8E59,
    MAX_SERVER_WAIT_TIMEOUT          = 0x9111,
    MAX_VERTEX_OUTPUT_COMPONENTS     = 0x9122,
    OBJECT_TYPE                      = 0x9112,
    PROGRAM_POINT_SIZE               = 0x8642,
    PROVOKING_VERTEX                 = 0x8E4F,
    PROXY_TEXTURE_2D_MULTISAMPLE     = 0x9101,
    PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY = 0x9103,
    QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION = 0x8E4C,
    SAMPLER_2D_MULTISAMPLE           = 0x9108,
    SAMPLER_2D_MULTISAMPLE_ARRAY     = 0x910B,
    SAMPLE_MASK                      = 0x8E51,
    SAMPLE_MASK_VALUE                = 0x8E52,
    SAMPLE_POSITION                  = 0x8E50,
    SIGNALED                         = 0x9119,
    SYNC_CONDITION                   = 0x9113,
    SYNC_FENCE                       = 0x9116,
    SYNC_FLAGS                       = 0x9115,
    SYNC_FLUSH_COMMANDS_BIT          = 0x00000001,
    SYNC_GPU_COMMANDS_COMPLETE       = 0x9117,
    SYNC_STATUS                      = 0x9114,
    TEXTURE_2D_MULTISAMPLE           = 0x9100,
    TEXTURE_2D_MULTISAMPLE_ARRAY     = 0x9102,
    //TEXTURE_BINDING_2D_MULTISAMPLE taken from ext: ARB_direct_state_access
    //TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY taken from ext: ARB_direct_state_access
    TEXTURE_CUBE_MAP_SEAMLESS        = 0x884F,
    TEXTURE_FIXED_SAMPLE_LOCATIONS   = 0x9107,
    TEXTURE_SAMPLES                  = 0x9106,
    TIMEOUT_EXPIRED                  = 0x911B,
    TIMEOUT_IGNORED                  = 0xFFFFFFFF,
    TRIANGLES_ADJACENCY              = 0x000C,
    TRIANGLE_STRIP_ADJACENCY         = 0x000D,
    UNSIGNALED                       = 0x9118,
    UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE = 0x910A,
    UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910D,
    WAIT_FAILED_                     = 0x911D,

    ANY_SAMPLES_PASSED               = 0x8C2F,
    INT_2_10_10_10_REV               = 0x8D9F,
    MAX_DUAL_SOURCE_DRAW_BUFFERS     = 0x88FC,
    ONE_MINUS_SRC1_ALPHA             = 0x88FB,
    ONE_MINUS_SRC1_COLOR             = 0x88FA,
    RGB10_A2UI                       = 0x906F,
    SAMPLER_BINDING                  = 0x8919,
    SRC1_COLOR                       = 0x88F9,
    TEXTURE_SWIZZLE_A                = 0x8E45,
    TEXTURE_SWIZZLE_B                = 0x8E44,
    TEXTURE_SWIZZLE_G                = 0x8E43,
    TEXTURE_SWIZZLE_R                = 0x8E42,
    TEXTURE_SWIZZLE_RGBA             = 0x8E46,
    TIMESTAMP                        = 0x8E28,
    TIME_ELAPSED                     = 0x88BF,
    VERTEX_ATTRIB_ARRAY_DIVISOR      = 0x88FE,

};

} // namespace gl

//============================================================================//

namespace gl::detail {

SQEE_API extern void (MSVC_STDCALL *DebugMessageCallback)(GLDEBUGPROC callback, const void* userParam);
SQEE_API extern void (MSVC_STDCALL *DebugMessageControl)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
SQEE_API extern void (MSVC_STDCALL *DebugMessageInsert)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf);
SQEE_API extern GLuint (MSVC_STDCALL *GetDebugMessageLog)(GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog);
SQEE_API extern void (MSVC_STDCALL *GetObjectLabel)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei* length, GLchar* label);
SQEE_API extern void (MSVC_STDCALL *GetObjectPtrLabel)(const void* ptr, GLsizei bufSize, GLsizei* length, GLchar* label);
SQEE_API extern void (MSVC_STDCALL *GetPointerv)(GLenum pname, void** params);
SQEE_API extern void (MSVC_STDCALL *ObjectLabel)(GLenum identifier, GLuint name, GLsizei length, const GLchar* label);
SQEE_API extern void (MSVC_STDCALL *ObjectPtrLabel)(const void* ptr, GLsizei length, const GLchar* label);
SQEE_API extern void (MSVC_STDCALL *PopDebugGroup)(void);
SQEE_API extern void (MSVC_STDCALL *PushDebugGroup)(GLenum source, GLuint id, GLsizei length, const GLchar* message);

SQEE_API extern void (MSVC_STDCALL *TexStorage1D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
SQEE_API extern void (MSVC_STDCALL *TexStorage2D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *TexStorage3D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

SQEE_API extern void (MSVC_STDCALL *BufferStorage)(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags);

SQEE_API extern void (MSVC_STDCALL *BindTextureUnit)(GLuint unit, GLuint texture);
SQEE_API extern void (MSVC_STDCALL *BlitNamedFramebuffer)(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
SQEE_API extern GLenum (MSVC_STDCALL *CheckNamedFramebufferStatus)(GLuint framebuffer, GLenum target);
SQEE_API extern void (MSVC_STDCALL *ClearNamedBufferData)(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data);
SQEE_API extern void (MSVC_STDCALL *ClearNamedBufferSubData)(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data);
SQEE_API extern void (MSVC_STDCALL *ClearNamedFramebufferfi)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat depth, GLint stencil);
SQEE_API extern void (MSVC_STDCALL *ClearNamedFramebufferfv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ClearNamedFramebufferiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *ClearNamedFramebufferuiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *CompressedTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (MSVC_STDCALL *CompressedTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (MSVC_STDCALL *CompressedTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (MSVC_STDCALL *CopyNamedBufferSubData)(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
SQEE_API extern void (MSVC_STDCALL *CopyTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
SQEE_API extern void (MSVC_STDCALL *CopyTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *CopyTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *CreateBuffers)(GLsizei n, GLuint* buffers);
SQEE_API extern void (MSVC_STDCALL *CreateFramebuffers)(GLsizei n, GLuint* framebuffers);
SQEE_API extern void (MSVC_STDCALL *CreateProgramPipelines)(GLsizei n, GLuint* pipelines);
SQEE_API extern void (MSVC_STDCALL *CreateQueries)(GLenum target, GLsizei n, GLuint* ids);
SQEE_API extern void (MSVC_STDCALL *CreateRenderbuffers)(GLsizei n, GLuint* renderbuffers);
SQEE_API extern void (MSVC_STDCALL *CreateSamplers)(GLsizei n, GLuint* samplers);
SQEE_API extern void (MSVC_STDCALL *CreateTextures)(GLenum target, GLsizei n, GLuint* textures);
SQEE_API extern void (MSVC_STDCALL *CreateTransformFeedbacks)(GLsizei n, GLuint* ids);
SQEE_API extern void (MSVC_STDCALL *CreateVertexArrays)(GLsizei n, GLuint* arrays);
SQEE_API extern void (MSVC_STDCALL *DisableVertexArrayAttrib)(GLuint vaobj, GLuint index);
SQEE_API extern void (MSVC_STDCALL *EnableVertexArrayAttrib)(GLuint vaobj, GLuint index);
SQEE_API extern void (MSVC_STDCALL *FlushMappedNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length);
SQEE_API extern void (MSVC_STDCALL *GenerateTextureMipmap)(GLuint texture);
SQEE_API extern void (MSVC_STDCALL *GetCompressedTextureImage)(GLuint texture, GLint level, GLsizei bufSize, void* pixels);
SQEE_API extern void (MSVC_STDCALL *GetNamedBufferParameteri64v)(GLuint buffer, GLenum pname, GLint64* params);
SQEE_API extern void (MSVC_STDCALL *GetNamedBufferParameteriv)(GLuint buffer, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetNamedBufferPointerv)(GLuint buffer, GLenum pname, void** params);
SQEE_API extern void (MSVC_STDCALL *GetNamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data);
SQEE_API extern void (MSVC_STDCALL *GetNamedFramebufferAttachmentParameteriv)(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetNamedFramebufferParameteriv)(GLuint framebuffer, GLenum pname, GLint* param);
SQEE_API extern void (MSVC_STDCALL *GetNamedRenderbufferParameteriv)(GLuint renderbuffer, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetQueryBufferObjecti64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
SQEE_API extern void (MSVC_STDCALL *GetQueryBufferObjectiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
SQEE_API extern void (MSVC_STDCALL *GetQueryBufferObjectui64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
SQEE_API extern void (MSVC_STDCALL *GetQueryBufferObjectuiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
SQEE_API extern void (MSVC_STDCALL *GetTextureImage)(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels);
SQEE_API extern void (MSVC_STDCALL *GetTextureLevelParameterfv)(GLuint texture, GLint level, GLenum pname, GLfloat* params);
SQEE_API extern void (MSVC_STDCALL *GetTextureLevelParameteriv)(GLuint texture, GLint level, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetTextureParameterIiv)(GLuint texture, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetTextureParameterIuiv)(GLuint texture, GLenum pname, GLuint* params);
SQEE_API extern void (MSVC_STDCALL *GetTextureParameterfv)(GLuint texture, GLenum pname, GLfloat* params);
SQEE_API extern void (MSVC_STDCALL *GetTextureParameteriv)(GLuint texture, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetTransformFeedbacki64_v)(GLuint xfb, GLenum pname, GLuint index, GLint64* param);
SQEE_API extern void (MSVC_STDCALL *GetTransformFeedbacki_v)(GLuint xfb, GLenum pname, GLuint index, GLint* param);
SQEE_API extern void (MSVC_STDCALL *GetTransformFeedbackiv)(GLuint xfb, GLenum pname, GLint* param);
SQEE_API extern void (MSVC_STDCALL *GetVertexArrayIndexed64iv)(GLuint vaobj, GLuint index, GLenum pname, GLint64* param);
SQEE_API extern void (MSVC_STDCALL *GetVertexArrayIndexediv)(GLuint vaobj, GLuint index, GLenum pname, GLint* param);
SQEE_API extern void (MSVC_STDCALL *GetVertexArrayiv)(GLuint vaobj, GLenum pname, GLint* param);
SQEE_API extern void (MSVC_STDCALL *InvalidateNamedFramebufferData)(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments);
SQEE_API extern void (MSVC_STDCALL *InvalidateNamedFramebufferSubData)(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void* (MSVC_STDCALL *MapNamedBuffer)(GLuint buffer, GLenum access);
SQEE_API extern void* (MSVC_STDCALL *MapNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
SQEE_API extern void (MSVC_STDCALL *NamedBufferData)(GLuint buffer, GLsizeiptr size, const void* data, GLenum usage);
SQEE_API extern void (MSVC_STDCALL *NamedBufferStorage)(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
SQEE_API extern void (MSVC_STDCALL *NamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
SQEE_API extern void (MSVC_STDCALL *NamedFramebufferDrawBuffer)(GLuint framebuffer, GLenum buf);
SQEE_API extern void (MSVC_STDCALL *NamedFramebufferDrawBuffers)(GLuint framebuffer, GLsizei n, const GLenum* bufs);
SQEE_API extern void (MSVC_STDCALL *NamedFramebufferParameteri)(GLuint framebuffer, GLenum pname, GLint param);
SQEE_API extern void (MSVC_STDCALL *NamedFramebufferReadBuffer)(GLuint framebuffer, GLenum src);
SQEE_API extern void (MSVC_STDCALL *NamedFramebufferRenderbuffer)(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
SQEE_API extern void (MSVC_STDCALL *NamedFramebufferTexture)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
SQEE_API extern void (MSVC_STDCALL *NamedFramebufferTextureLayer)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
SQEE_API extern void (MSVC_STDCALL *NamedRenderbufferStorage)(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *NamedRenderbufferStorageMultisample)(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *TextureBuffer)(GLuint texture, GLenum internalformat, GLuint buffer);
SQEE_API extern void (MSVC_STDCALL *TextureBufferRange)(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
SQEE_API extern void (MSVC_STDCALL *TextureParameterIiv)(GLuint texture, GLenum pname, const GLint* params);
SQEE_API extern void (MSVC_STDCALL *TextureParameterIuiv)(GLuint texture, GLenum pname, const GLuint* params);
SQEE_API extern void (MSVC_STDCALL *TextureParameterf)(GLuint texture, GLenum pname, GLfloat param);
SQEE_API extern void (MSVC_STDCALL *TextureParameterfv)(GLuint texture, GLenum pname, const GLfloat* param);
SQEE_API extern void (MSVC_STDCALL *TextureParameteri)(GLuint texture, GLenum pname, GLint param);
SQEE_API extern void (MSVC_STDCALL *TextureParameteriv)(GLuint texture, GLenum pname, const GLint* param);
SQEE_API extern void (MSVC_STDCALL *TextureStorage1D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
SQEE_API extern void (MSVC_STDCALL *TextureStorage2D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *TextureStorage2DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
SQEE_API extern void (MSVC_STDCALL *TextureStorage3D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
SQEE_API extern void (MSVC_STDCALL *TextureStorage3DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
SQEE_API extern void (MSVC_STDCALL *TextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (MSVC_STDCALL *TextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (MSVC_STDCALL *TextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (MSVC_STDCALL *TransformFeedbackBufferBase)(GLuint xfb, GLuint index, GLuint buffer);
SQEE_API extern void (MSVC_STDCALL *TransformFeedbackBufferRange)(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
SQEE_API extern GLboolean (MSVC_STDCALL *UnmapNamedBuffer)(GLuint buffer);
SQEE_API extern void (MSVC_STDCALL *VertexArrayAttribBinding)(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
SQEE_API extern void (MSVC_STDCALL *VertexArrayAttribFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
SQEE_API extern void (MSVC_STDCALL *VertexArrayAttribIFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
SQEE_API extern void (MSVC_STDCALL *VertexArrayAttribLFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
SQEE_API extern void (MSVC_STDCALL *VertexArrayBindingDivisor)(GLuint vaobj, GLuint bindingindex, GLuint divisor);
SQEE_API extern void (MSVC_STDCALL *VertexArrayElementBuffer)(GLuint vaobj, GLuint buffer);
SQEE_API extern void (MSVC_STDCALL *VertexArrayVertexBuffer)(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
SQEE_API extern void (MSVC_STDCALL *VertexArrayVertexBuffers)(GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides);

SQEE_API extern void (MSVC_STDCALL *ActiveShaderProgram)(GLuint pipeline, GLuint program);
SQEE_API extern void (MSVC_STDCALL *BindProgramPipeline)(GLuint pipeline);
SQEE_API extern GLuint (MSVC_STDCALL *CreateShaderProgramv)(GLenum type, GLsizei count, const GLchar *const* strings);
SQEE_API extern void (MSVC_STDCALL *DeleteProgramPipelines)(GLsizei n, const GLuint* pipelines);
SQEE_API extern void (MSVC_STDCALL *GenProgramPipelines)(GLsizei n, GLuint* pipelines);
SQEE_API extern void (MSVC_STDCALL *GetProgramPipelineInfoLog)(GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
SQEE_API extern void (MSVC_STDCALL *GetProgramPipelineiv)(GLuint pipeline, GLenum pname, GLint* params);
SQEE_API extern GLboolean (MSVC_STDCALL *IsProgramPipeline)(GLuint pipeline);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform1d)(GLuint program, GLint location, GLdouble v0);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform1dv)(GLuint program, GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform1f)(GLuint program, GLint location, GLfloat v0);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform1fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform1i)(GLuint program, GLint location, GLint v0);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform1iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform1ui)(GLuint program, GLint location, GLuint v0);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform1uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform2d)(GLuint program, GLint location, GLdouble v0, GLdouble v1);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform2dv)(GLuint program, GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform2f)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform2fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform2i)(GLuint program, GLint location, GLint v0, GLint v1);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform2iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform2ui)(GLuint program, GLint location, GLuint v0, GLuint v1);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform2uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform3d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform3dv)(GLuint program, GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform3f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform3fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform3i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform3iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform3ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform3uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform4d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform4dv)(GLuint program, GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform4f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform4fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform4i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform4iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform4ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
SQEE_API extern void (MSVC_STDCALL *ProgramUniform4uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix2x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix2x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix2x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix2x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix3x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix3x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix3x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix3x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix4x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix4x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix4x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (MSVC_STDCALL *ProgramUniformMatrix4x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *UseProgramStages)(GLuint pipeline, GLbitfield stages, GLuint program);
SQEE_API extern void (MSVC_STDCALL *ValidateProgramPipeline)(GLuint pipeline);

SQEE_API extern void (MSVC_STDCALL *BlendFunc)(GLenum sfactor, GLenum dfactor);
SQEE_API extern void (MSVC_STDCALL *Clear)(GLbitfield mask);
SQEE_API extern void (MSVC_STDCALL *ClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
SQEE_API extern void (MSVC_STDCALL *ClearDepth)(GLdouble depth);
SQEE_API extern void (MSVC_STDCALL *ClearStencil)(GLint s);
SQEE_API extern void (MSVC_STDCALL *ColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
SQEE_API extern void (MSVC_STDCALL *CullFace)(GLenum mode);
SQEE_API extern void (MSVC_STDCALL *DepthFunc)(GLenum func);
SQEE_API extern void (MSVC_STDCALL *DepthMask)(GLboolean flag);
SQEE_API extern void (MSVC_STDCALL *DepthRange)(GLdouble ren_near, GLdouble ren_far);
SQEE_API extern void (MSVC_STDCALL *Disable)(GLenum cap);
SQEE_API extern void (MSVC_STDCALL *DrawBuffer)(GLenum buf);
SQEE_API extern void (MSVC_STDCALL *Enable)(GLenum cap);
SQEE_API extern void (MSVC_STDCALL *Finish)(void);
SQEE_API extern void (MSVC_STDCALL *Flush)(void);
SQEE_API extern void (MSVC_STDCALL *FrontFace)(GLenum mode);
SQEE_API extern void (MSVC_STDCALL *GetBooleanv)(GLenum pname, GLboolean* data);
SQEE_API extern void (MSVC_STDCALL *GetDoublev)(GLenum pname, GLdouble* data);
SQEE_API extern GLenum (MSVC_STDCALL *GetError)(void);
SQEE_API extern void (MSVC_STDCALL *GetFloatv)(GLenum pname, GLfloat* data);
SQEE_API extern void (MSVC_STDCALL *GetIntegerv)(GLenum pname, GLint* data);
SQEE_API extern const GLubyte* (MSVC_STDCALL *GetString)(GLenum name);
SQEE_API extern void (MSVC_STDCALL *GetTexImage)(GLenum target, GLint level, GLenum format, GLenum type, void* pixels);
SQEE_API extern void (MSVC_STDCALL *GetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat* params);
SQEE_API extern void (MSVC_STDCALL *GetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetTexParameterfv)(GLenum target, GLenum pname, GLfloat* params);
SQEE_API extern void (MSVC_STDCALL *GetTexParameteriv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *Hint)(GLenum target, GLenum mode);
SQEE_API extern GLboolean (MSVC_STDCALL *IsEnabled)(GLenum cap);
SQEE_API extern void (MSVC_STDCALL *LineWidth)(GLfloat width);
SQEE_API extern void (MSVC_STDCALL *LogicOp)(GLenum opcode);
SQEE_API extern void (MSVC_STDCALL *PixelStoref)(GLenum pname, GLfloat param);
SQEE_API extern void (MSVC_STDCALL *PixelStorei)(GLenum pname, GLint param);
SQEE_API extern void (MSVC_STDCALL *PointSize)(GLfloat size);
SQEE_API extern void (MSVC_STDCALL *PolygonMode)(GLenum face, GLenum mode);
SQEE_API extern void (MSVC_STDCALL *ReadBuffer)(GLenum src);
SQEE_API extern void (MSVC_STDCALL *ReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels);
SQEE_API extern void (MSVC_STDCALL *Scissor)(GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *StencilFunc)(GLenum func, GLint ref, GLuint mask);
SQEE_API extern void (MSVC_STDCALL *StencilMask)(GLuint mask);
SQEE_API extern void (MSVC_STDCALL *StencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
SQEE_API extern void (MSVC_STDCALL *TexImage1D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (MSVC_STDCALL *TexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (MSVC_STDCALL *TexParameterf)(GLenum target, GLenum pname, GLfloat param);
SQEE_API extern void (MSVC_STDCALL *TexParameterfv)(GLenum target, GLenum pname, const GLfloat* params);
SQEE_API extern void (MSVC_STDCALL *TexParameteri)(GLenum target, GLenum pname, GLint param);
SQEE_API extern void (MSVC_STDCALL *TexParameteriv)(GLenum target, GLenum pname, const GLint* params);
SQEE_API extern void (MSVC_STDCALL *Viewport)(GLint x, GLint y, GLsizei width, GLsizei height);

SQEE_API extern void (MSVC_STDCALL *BindTexture)(GLenum target, GLuint texture);
SQEE_API extern void (MSVC_STDCALL *CopyTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
SQEE_API extern void (MSVC_STDCALL *CopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
SQEE_API extern void (MSVC_STDCALL *CopyTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
SQEE_API extern void (MSVC_STDCALL *CopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *DeleteTextures)(GLsizei n, const GLuint* textures);
SQEE_API extern void (MSVC_STDCALL *DrawArrays)(GLenum mode, GLint first, GLsizei count);
SQEE_API extern void (MSVC_STDCALL *DrawElements)(GLenum mode, GLsizei count, GLenum type, const void* indices);
SQEE_API extern void (MSVC_STDCALL *GenTextures)(GLsizei n, GLuint* textures);
SQEE_API extern GLboolean (MSVC_STDCALL *IsTexture)(GLuint texture);
SQEE_API extern void (MSVC_STDCALL *PolygonOffset)(GLfloat factor, GLfloat units);
SQEE_API extern void (MSVC_STDCALL *TexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (MSVC_STDCALL *TexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);

SQEE_API extern void (MSVC_STDCALL *CopyTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *DrawRangeElements)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
SQEE_API extern void (MSVC_STDCALL *TexImage3D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (MSVC_STDCALL *TexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

SQEE_API extern void (MSVC_STDCALL *ActiveTexture)(GLenum texture);
SQEE_API extern void (MSVC_STDCALL *CompressedTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void* data);
SQEE_API extern void (MSVC_STDCALL *CompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
SQEE_API extern void (MSVC_STDCALL *CompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data);
SQEE_API extern void (MSVC_STDCALL *CompressedTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (MSVC_STDCALL *CompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (MSVC_STDCALL *CompressedTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (MSVC_STDCALL *GetCompressedTexImage)(GLenum target, GLint level, void* img);
SQEE_API extern void (MSVC_STDCALL *SampleCoverage)(GLfloat value, GLboolean invert);

SQEE_API extern void (MSVC_STDCALL *BlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
SQEE_API extern void (MSVC_STDCALL *BlendEquation)(GLenum mode);
SQEE_API extern void (MSVC_STDCALL *BlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
SQEE_API extern void (MSVC_STDCALL *MultiDrawArrays)(GLenum mode, const GLint* first, const GLsizei* count, GLsizei drawcount);
SQEE_API extern void (MSVC_STDCALL *MultiDrawElements)(GLenum mode, const GLsizei* count, GLenum type, const void *const* indices, GLsizei drawcount);
SQEE_API extern void (MSVC_STDCALL *PointParameterf)(GLenum pname, GLfloat param);
SQEE_API extern void (MSVC_STDCALL *PointParameterfv)(GLenum pname, const GLfloat* params);
SQEE_API extern void (MSVC_STDCALL *PointParameteri)(GLenum pname, GLint param);
SQEE_API extern void (MSVC_STDCALL *PointParameteriv)(GLenum pname, const GLint* params);

SQEE_API extern void (MSVC_STDCALL *BeginQuery)(GLenum target, GLuint id);
SQEE_API extern void (MSVC_STDCALL *BindBuffer)(GLenum target, GLuint buffer);
SQEE_API extern void (MSVC_STDCALL *BufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
SQEE_API extern void (MSVC_STDCALL *BufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
SQEE_API extern void (MSVC_STDCALL *DeleteBuffers)(GLsizei n, const GLuint* buffers);
SQEE_API extern void (MSVC_STDCALL *DeleteQueries)(GLsizei n, const GLuint* ids);
SQEE_API extern void (MSVC_STDCALL *EndQuery)(GLenum target);
SQEE_API extern void (MSVC_STDCALL *GenBuffers)(GLsizei n, GLuint* buffers);
SQEE_API extern void (MSVC_STDCALL *GenQueries)(GLsizei n, GLuint* ids);
SQEE_API extern void (MSVC_STDCALL *GetBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetBufferPointerv)(GLenum target, GLenum pname, void** params);
SQEE_API extern void (MSVC_STDCALL *GetBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, void* data);
SQEE_API extern void (MSVC_STDCALL *GetQueryObjectiv)(GLuint id, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetQueryObjectuiv)(GLuint id, GLenum pname, GLuint* params);
SQEE_API extern void (MSVC_STDCALL *GetQueryiv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern GLboolean (MSVC_STDCALL *IsBuffer)(GLuint buffer);
SQEE_API extern GLboolean (MSVC_STDCALL *IsQuery)(GLuint id);
SQEE_API extern void* (MSVC_STDCALL *MapBuffer)(GLenum target, GLenum access);
SQEE_API extern GLboolean (MSVC_STDCALL *UnmapBuffer)(GLenum target);

SQEE_API extern void (MSVC_STDCALL *AttachShader)(GLuint program, GLuint shader);
SQEE_API extern void (MSVC_STDCALL *BindAttribLocation)(GLuint program, GLuint index, const GLchar* name);
SQEE_API extern void (MSVC_STDCALL *BlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
SQEE_API extern void (MSVC_STDCALL *CompileShader)(GLuint shader);
SQEE_API extern GLuint (MSVC_STDCALL *CreateProgram)(void);
SQEE_API extern GLuint (MSVC_STDCALL *CreateShader)(GLenum type);
SQEE_API extern void (MSVC_STDCALL *DeleteProgram)(GLuint program);
SQEE_API extern void (MSVC_STDCALL *DeleteShader)(GLuint shader);
SQEE_API extern void (MSVC_STDCALL *DetachShader)(GLuint program, GLuint shader);
SQEE_API extern void (MSVC_STDCALL *DisableVertexAttribArray)(GLuint index);
SQEE_API extern void (MSVC_STDCALL *DrawBuffers)(GLsizei n, const GLenum* bufs);
SQEE_API extern void (MSVC_STDCALL *EnableVertexAttribArray)(GLuint index);
SQEE_API extern void (MSVC_STDCALL *GetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
SQEE_API extern void (MSVC_STDCALL *GetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
SQEE_API extern void (MSVC_STDCALL *GetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
SQEE_API extern GLint (MSVC_STDCALL *GetAttribLocation)(GLuint program, const GLchar* name);
SQEE_API extern void (MSVC_STDCALL *GetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
SQEE_API extern void (MSVC_STDCALL *GetProgramiv)(GLuint program, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
SQEE_API extern void (MSVC_STDCALL *GetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);
SQEE_API extern void (MSVC_STDCALL *GetShaderiv)(GLuint shader, GLenum pname, GLint* params);
SQEE_API extern GLint (MSVC_STDCALL *GetUniformLocation)(GLuint program, const GLchar* name);
SQEE_API extern void (MSVC_STDCALL *GetUniformfv)(GLuint program, GLint location, GLfloat* params);
SQEE_API extern void (MSVC_STDCALL *GetUniformiv)(GLuint program, GLint location, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetVertexAttribPointerv)(GLuint index, GLenum pname, void** pointer);
SQEE_API extern void (MSVC_STDCALL *GetVertexAttribdv)(GLuint index, GLenum pname, GLdouble* params);
SQEE_API extern void (MSVC_STDCALL *GetVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params);
SQEE_API extern void (MSVC_STDCALL *GetVertexAttribiv)(GLuint index, GLenum pname, GLint* params);
SQEE_API extern GLboolean (MSVC_STDCALL *IsProgram)(GLuint program);
SQEE_API extern GLboolean (MSVC_STDCALL *IsShader)(GLuint shader);
SQEE_API extern void (MSVC_STDCALL *LinkProgram)(GLuint program);
SQEE_API extern void (MSVC_STDCALL *ShaderSource)(GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length);
SQEE_API extern void (MSVC_STDCALL *StencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
SQEE_API extern void (MSVC_STDCALL *StencilMaskSeparate)(GLenum face, GLuint mask);
SQEE_API extern void (MSVC_STDCALL *StencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
SQEE_API extern void (MSVC_STDCALL *Uniform1f)(GLint location, GLfloat v0);
SQEE_API extern void (MSVC_STDCALL *Uniform1fv)(GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *Uniform1i)(GLint location, GLint v0);
SQEE_API extern void (MSVC_STDCALL *Uniform1iv)(GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *Uniform2f)(GLint location, GLfloat v0, GLfloat v1);
SQEE_API extern void (MSVC_STDCALL *Uniform2fv)(GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *Uniform2i)(GLint location, GLint v0, GLint v1);
SQEE_API extern void (MSVC_STDCALL *Uniform2iv)(GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *Uniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
SQEE_API extern void (MSVC_STDCALL *Uniform3fv)(GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *Uniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
SQEE_API extern void (MSVC_STDCALL *Uniform3iv)(GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *Uniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
SQEE_API extern void (MSVC_STDCALL *Uniform4fv)(GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *Uniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
SQEE_API extern void (MSVC_STDCALL *Uniform4iv)(GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *UniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *UniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *UniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *UseProgram)(GLuint program);
SQEE_API extern void (MSVC_STDCALL *ValidateProgram)(GLuint program);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib1d)(GLuint index, GLdouble x);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib1dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib1f)(GLuint index, GLfloat x);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib1fv)(GLuint index, const GLfloat* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib1s)(GLuint index, GLshort x);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib1sv)(GLuint index, const GLshort* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib2d)(GLuint index, GLdouble x, GLdouble y);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib2dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib2fv)(GLuint index, const GLfloat* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib2s)(GLuint index, GLshort x, GLshort y);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib2sv)(GLuint index, const GLshort* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib3dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib3fv)(GLuint index, const GLfloat* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib3s)(GLuint index, GLshort x, GLshort y, GLshort z);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib3sv)(GLuint index, const GLshort* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4Nbv)(GLuint index, const GLbyte* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4Niv)(GLuint index, const GLint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4Nsv)(GLuint index, const GLshort* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4Nub)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4Nubv)(GLuint index, const GLubyte* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4Nuiv)(GLuint index, const GLuint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4Nusv)(GLuint index, const GLushort* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4bv)(GLuint index, const GLbyte* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4fv)(GLuint index, const GLfloat* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4iv)(GLuint index, const GLint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4s)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4sv)(GLuint index, const GLshort* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4ubv)(GLuint index, const GLubyte* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttrib4usv)(GLuint index, const GLushort* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);

SQEE_API extern void (MSVC_STDCALL *UniformMatrix2x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *UniformMatrix2x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *UniformMatrix3x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *UniformMatrix3x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *UniformMatrix4x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *UniformMatrix4x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

SQEE_API extern void (MSVC_STDCALL *BeginConditionalRender)(GLuint id, GLenum mode);
SQEE_API extern void (MSVC_STDCALL *BeginTransformFeedback)(GLenum primitiveMode);
SQEE_API extern void (MSVC_STDCALL *BindBufferBase)(GLenum target, GLuint index, GLuint buffer);
SQEE_API extern void (MSVC_STDCALL *BindBufferRange)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
SQEE_API extern void (MSVC_STDCALL *BindFragDataLocation)(GLuint program, GLuint color, const GLchar* name);
SQEE_API extern void (MSVC_STDCALL *BindFramebuffer)(GLenum target, GLuint framebuffer);
SQEE_API extern void (MSVC_STDCALL *BindRenderbuffer)(GLenum target, GLuint renderbuffer);
SQEE_API extern void (MSVC_STDCALL *BindVertexArray)(GLuint ren_array);
SQEE_API extern void (MSVC_STDCALL *BlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
SQEE_API extern GLenum (MSVC_STDCALL *CheckFramebufferStatus)(GLenum target);
SQEE_API extern void (MSVC_STDCALL *ClampColor)(GLenum target, GLenum clamp);
SQEE_API extern void (MSVC_STDCALL *ClearBufferfi)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
SQEE_API extern void (MSVC_STDCALL *ClearBufferfv)(GLenum buffer, GLint drawbuffer, const GLfloat* value);
SQEE_API extern void (MSVC_STDCALL *ClearBufferiv)(GLenum buffer, GLint drawbuffer, const GLint* value);
SQEE_API extern void (MSVC_STDCALL *ClearBufferuiv)(GLenum buffer, GLint drawbuffer, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *ColorMaski)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
SQEE_API extern void (MSVC_STDCALL *DeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
SQEE_API extern void (MSVC_STDCALL *DeleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);
SQEE_API extern void (MSVC_STDCALL *DeleteVertexArrays)(GLsizei n, const GLuint* arrays);
SQEE_API extern void (MSVC_STDCALL *Disablei)(GLenum target, GLuint index);
SQEE_API extern void (MSVC_STDCALL *Enablei)(GLenum target, GLuint index);
SQEE_API extern void (MSVC_STDCALL *EndConditionalRender)(void);
SQEE_API extern void (MSVC_STDCALL *EndTransformFeedback)(void);
SQEE_API extern void (MSVC_STDCALL *FlushMappedBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length);
SQEE_API extern void (MSVC_STDCALL *FramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
SQEE_API extern void (MSVC_STDCALL *FramebufferTexture1D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
SQEE_API extern void (MSVC_STDCALL *FramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
SQEE_API extern void (MSVC_STDCALL *FramebufferTexture3D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
SQEE_API extern void (MSVC_STDCALL *FramebufferTextureLayer)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
SQEE_API extern void (MSVC_STDCALL *GenFramebuffers)(GLsizei n, GLuint* framebuffers);
SQEE_API extern void (MSVC_STDCALL *GenRenderbuffers)(GLsizei n, GLuint* renderbuffers);
SQEE_API extern void (MSVC_STDCALL *GenVertexArrays)(GLsizei n, GLuint* arrays);
SQEE_API extern void (MSVC_STDCALL *GenerateMipmap)(GLenum target);
SQEE_API extern void (MSVC_STDCALL *GetBooleani_v)(GLenum target, GLuint index, GLboolean* data);
SQEE_API extern GLint (MSVC_STDCALL *GetFragDataLocation)(GLuint program, const GLchar* name);
SQEE_API extern void (MSVC_STDCALL *GetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetIntegeri_v)(GLenum target, GLuint index, GLint* data);
SQEE_API extern void (MSVC_STDCALL *GetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern const GLubyte* (MSVC_STDCALL *GetStringi)(GLenum name, GLuint index);
SQEE_API extern void (MSVC_STDCALL *GetTexParameterIiv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetTexParameterIuiv)(GLenum target, GLenum pname, GLuint* params);
SQEE_API extern void (MSVC_STDCALL *GetTransformFeedbackVarying)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name);
SQEE_API extern void (MSVC_STDCALL *GetUniformuiv)(GLuint program, GLint location, GLuint* params);
SQEE_API extern void (MSVC_STDCALL *GetVertexAttribIiv)(GLuint index, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetVertexAttribIuiv)(GLuint index, GLenum pname, GLuint* params);
SQEE_API extern GLboolean (MSVC_STDCALL *IsEnabledi)(GLenum target, GLuint index);
SQEE_API extern GLboolean (MSVC_STDCALL *IsFramebuffer)(GLuint framebuffer);
SQEE_API extern GLboolean (MSVC_STDCALL *IsRenderbuffer)(GLuint renderbuffer);
SQEE_API extern GLboolean (MSVC_STDCALL *IsVertexArray)(GLuint ren_array);
SQEE_API extern void* (MSVC_STDCALL *MapBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
SQEE_API extern void (MSVC_STDCALL *RenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *RenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (MSVC_STDCALL *TexParameterIiv)(GLenum target, GLenum pname, const GLint* params);
SQEE_API extern void (MSVC_STDCALL *TexParameterIuiv)(GLenum target, GLenum pname, const GLuint* params);
SQEE_API extern void (MSVC_STDCALL *TransformFeedbackVaryings)(GLuint program, GLsizei count, const GLchar *const* varyings, GLenum bufferMode);
SQEE_API extern void (MSVC_STDCALL *Uniform1ui)(GLint location, GLuint v0);
SQEE_API extern void (MSVC_STDCALL *Uniform1uiv)(GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *Uniform2ui)(GLint location, GLuint v0, GLuint v1);
SQEE_API extern void (MSVC_STDCALL *Uniform2uiv)(GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *Uniform3ui)(GLint location, GLuint v0, GLuint v1, GLuint v2);
SQEE_API extern void (MSVC_STDCALL *Uniform3uiv)(GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *Uniform4ui)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
SQEE_API extern void (MSVC_STDCALL *Uniform4uiv)(GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI1i)(GLuint index, GLint x);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI1iv)(GLuint index, const GLint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI1ui)(GLuint index, GLuint x);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI1uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI2i)(GLuint index, GLint x, GLint y);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI2iv)(GLuint index, const GLint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI2ui)(GLuint index, GLuint x, GLuint y);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI2uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI3i)(GLuint index, GLint x, GLint y, GLint z);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI3iv)(GLuint index, const GLint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI3ui)(GLuint index, GLuint x, GLuint y, GLuint z);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI3uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI4bv)(GLuint index, const GLbyte* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI4i)(GLuint index, GLint x, GLint y, GLint z, GLint w);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI4iv)(GLuint index, const GLint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI4sv)(GLuint index, const GLshort* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI4ubv)(GLuint index, const GLubyte* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI4ui)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI4uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribI4usv)(GLuint index, const GLushort* v);
SQEE_API extern void (MSVC_STDCALL *VertexAttribIPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer);

SQEE_API extern void (MSVC_STDCALL *CopyBufferSubData)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
SQEE_API extern void (MSVC_STDCALL *DrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
SQEE_API extern void (MSVC_STDCALL *DrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount);
SQEE_API extern void (MSVC_STDCALL *GetActiveUniformBlockName)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName);
SQEE_API extern void (MSVC_STDCALL *GetActiveUniformBlockiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetActiveUniformName)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformName);
SQEE_API extern void (MSVC_STDCALL *GetActiveUniformsiv)(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);
SQEE_API extern GLuint (MSVC_STDCALL *GetUniformBlockIndex)(GLuint program, const GLchar* uniformBlockName);
SQEE_API extern void (MSVC_STDCALL *GetUniformIndices)(GLuint program, GLsizei uniformCount, const GLchar *const* uniformNames, GLuint* uniformIndices);
SQEE_API extern void (MSVC_STDCALL *PrimitiveRestartIndex)(GLuint index);
SQEE_API extern void (MSVC_STDCALL *TexBuffer)(GLenum target, GLenum internalformat, GLuint buffer);
SQEE_API extern void (MSVC_STDCALL *UniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

SQEE_API extern GLenum (MSVC_STDCALL *ClientWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
SQEE_API extern void (MSVC_STDCALL *DeleteSync)(GLsync sync);
SQEE_API extern void (MSVC_STDCALL *DrawElementsBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex);
SQEE_API extern void (MSVC_STDCALL *DrawElementsInstancedBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLint basevertex);
SQEE_API extern void (MSVC_STDCALL *DrawRangeElementsBaseVertex)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices, GLint basevertex);
SQEE_API extern GLsync (MSVC_STDCALL *FenceSync)(GLenum condition, GLbitfield flags);
SQEE_API extern void (MSVC_STDCALL *FramebufferTexture)(GLenum target, GLenum attachment, GLuint texture, GLint level);
SQEE_API extern void (MSVC_STDCALL *GetBufferParameteri64v)(GLenum target, GLenum pname, GLint64* params);
SQEE_API extern void (MSVC_STDCALL *GetInteger64i_v)(GLenum target, GLuint index, GLint64* data);
SQEE_API extern void (MSVC_STDCALL *GetInteger64v)(GLenum pname, GLint64* data);
SQEE_API extern void (MSVC_STDCALL *GetMultisamplefv)(GLenum pname, GLuint index, GLfloat* val);
SQEE_API extern void (MSVC_STDCALL *GetSynciv)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values);
SQEE_API extern GLboolean (MSVC_STDCALL *IsSync)(GLsync sync);
SQEE_API extern void (MSVC_STDCALL *MultiDrawElementsBaseVertex)(GLenum mode, const GLsizei* count, GLenum type, const void *const* indices, GLsizei drawcount, const GLint* basevertex);
SQEE_API extern void (MSVC_STDCALL *ProvokingVertex)(GLenum mode);
SQEE_API extern void (MSVC_STDCALL *SampleMaski)(GLuint maskNumber, GLbitfield mask);
SQEE_API extern void (MSVC_STDCALL *TexImage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
SQEE_API extern void (MSVC_STDCALL *TexImage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
SQEE_API extern void (MSVC_STDCALL *WaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);

SQEE_API extern void (MSVC_STDCALL *BindFragDataLocationIndexed)(GLuint program, GLuint colorNumber, GLuint index, const GLchar* name);
SQEE_API extern void (MSVC_STDCALL *BindSampler)(GLuint unit, GLuint sampler);
SQEE_API extern void (MSVC_STDCALL *DeleteSamplers)(GLsizei count, const GLuint* samplers);
SQEE_API extern void (MSVC_STDCALL *GenSamplers)(GLsizei count, GLuint* samplers);
SQEE_API extern GLint (MSVC_STDCALL *GetFragDataIndex)(GLuint program, const GLchar* name);
SQEE_API extern void (MSVC_STDCALL *GetQueryObjecti64v)(GLuint id, GLenum pname, GLint64* params);
SQEE_API extern void (MSVC_STDCALL *GetQueryObjectui64v)(GLuint id, GLenum pname, GLuint64* params);
SQEE_API extern void (MSVC_STDCALL *GetSamplerParameterIiv)(GLuint sampler, GLenum pname, GLint* params);
SQEE_API extern void (MSVC_STDCALL *GetSamplerParameterIuiv)(GLuint sampler, GLenum pname, GLuint* params);
SQEE_API extern void (MSVC_STDCALL *GetSamplerParameterfv)(GLuint sampler, GLenum pname, GLfloat* params);
SQEE_API extern void (MSVC_STDCALL *GetSamplerParameteriv)(GLuint sampler, GLenum pname, GLint* params);
SQEE_API extern GLboolean (MSVC_STDCALL *IsSampler)(GLuint sampler);
SQEE_API extern void (MSVC_STDCALL *QueryCounter)(GLuint id, GLenum target);
SQEE_API extern void (MSVC_STDCALL *SamplerParameterIiv)(GLuint sampler, GLenum pname, const GLint* param);
SQEE_API extern void (MSVC_STDCALL *SamplerParameterIuiv)(GLuint sampler, GLenum pname, const GLuint* param);
SQEE_API extern void (MSVC_STDCALL *SamplerParameterf)(GLuint sampler, GLenum pname, GLfloat param);
SQEE_API extern void (MSVC_STDCALL *SamplerParameterfv)(GLuint sampler, GLenum pname, const GLfloat* param);
SQEE_API extern void (MSVC_STDCALL *SamplerParameteri)(GLuint sampler, GLenum pname, GLint param);
SQEE_API extern void (MSVC_STDCALL *SamplerParameteriv)(GLuint sampler, GLenum pname, const GLint* param);
SQEE_API extern void (MSVC_STDCALL *VertexAttribDivisor)(GLuint index, GLuint divisor);
SQEE_API extern void (MSVC_STDCALL *VertexAttribP1ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
SQEE_API extern void (MSVC_STDCALL *VertexAttribP1uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *VertexAttribP2ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
SQEE_API extern void (MSVC_STDCALL *VertexAttribP2uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *VertexAttribP3ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
SQEE_API extern void (MSVC_STDCALL *VertexAttribP3uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint* value);
SQEE_API extern void (MSVC_STDCALL *VertexAttribP4ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
SQEE_API extern void (MSVC_STDCALL *VertexAttribP4uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint* value);

} // namespace gl::detail

//============================================================================//

namespace gl {

inline void DebugMessageCallback(GLDEBUGPROC callback, const void* userParam){detail::DebugMessageCallback(callback, userParam);}
inline void DebugMessageControl(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled){detail::DebugMessageControl(source, type, severity, count, ids, enabled);}
inline void DebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf){detail::DebugMessageInsert(source, type, id, severity, length, buf);}
inline GLuint GetDebugMessageLog(GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog){return detail::GetDebugMessageLog(count, bufSize, sources, types, ids, severities, lengths, messageLog);}
inline void GetObjectLabel(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei* length, GLchar* label){detail::GetObjectLabel(identifier, name, bufSize, length, label);}
inline void GetObjectPtrLabel(const void* ptr, GLsizei bufSize, GLsizei* length, GLchar* label){detail::GetObjectPtrLabel(ptr, bufSize, length, label);}
inline void GetPointerv(GLenum pname, void** params){detail::GetPointerv(pname, params);}
inline void ObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar* label){detail::ObjectLabel(identifier, name, length, label);}
inline void ObjectPtrLabel(const void* ptr, GLsizei length, const GLchar* label){detail::ObjectPtrLabel(ptr, length, label);}
inline void PopDebugGroup(void){detail::PopDebugGroup();}
inline void PushDebugGroup(GLenum source, GLuint id, GLsizei length, const GLchar* message){detail::PushDebugGroup(source, id, length, message);}

inline void TexStorage1D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width){detail::TexStorage1D(target, levels, internalformat, width);}
inline void TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height){detail::TexStorage2D(target, levels, internalformat, width, height);}
inline void TexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth){detail::TexStorage3D(target, levels, internalformat, width, height, depth);}

inline void BufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags){detail::BufferStorage(target, size, data, flags);}

inline void BindTextureUnit(GLuint unit, GLuint texture){detail::BindTextureUnit(unit, texture);}
inline void BlitNamedFramebuffer(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter){detail::BlitNamedFramebuffer(readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);}
inline GLenum CheckNamedFramebufferStatus(GLuint framebuffer, GLenum target){return detail::CheckNamedFramebufferStatus(framebuffer, target);}
inline void ClearNamedBufferData(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data){detail::ClearNamedBufferData(buffer, internalformat, format, type, data);}
inline void ClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data){detail::ClearNamedBufferSubData(buffer, internalformat, offset, size, format, type, data);}
inline void ClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat depth, GLint stencil){detail::ClearNamedFramebufferfi(framebuffer, buffer, drawbuffer, depth, stencil);}
inline void ClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value){detail::ClearNamedFramebufferfv(framebuffer, buffer, drawbuffer, value);}
inline void ClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value){detail::ClearNamedFramebufferiv(framebuffer, buffer, drawbuffer, value);}
inline void ClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value){detail::ClearNamedFramebufferuiv(framebuffer, buffer, drawbuffer, value);}
inline void CompressedTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data){detail::CompressedTextureSubImage1D(texture, level, xoffset, width, format, imageSize, data);}
inline void CompressedTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data){detail::CompressedTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, imageSize, data);}
inline void CompressedTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data){detail::CompressedTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);}
inline void CopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size){detail::CopyNamedBufferSubData(readBuffer, writeBuffer, readOffset, writeOffset, size);}
inline void CopyTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width){detail::CopyTextureSubImage1D(texture, level, xoffset, x, y, width);}
inline void CopyTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height){detail::CopyTextureSubImage2D(texture, level, xoffset, yoffset, x, y, width, height);}
inline void CopyTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height){detail::CopyTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, x, y, width, height);}
inline void CreateBuffers(GLsizei n, GLuint* buffers){detail::CreateBuffers(n, buffers);}
inline void CreateFramebuffers(GLsizei n, GLuint* framebuffers){detail::CreateFramebuffers(n, framebuffers);}
inline void CreateProgramPipelines(GLsizei n, GLuint* pipelines){detail::CreateProgramPipelines(n, pipelines);}
inline void CreateQueries(GLenum target, GLsizei n, GLuint* ids){detail::CreateQueries(target, n, ids);}
inline void CreateRenderbuffers(GLsizei n, GLuint* renderbuffers){detail::CreateRenderbuffers(n, renderbuffers);}
inline void CreateSamplers(GLsizei n, GLuint* samplers){detail::CreateSamplers(n, samplers);}
inline void CreateTextures(GLenum target, GLsizei n, GLuint* textures){detail::CreateTextures(target, n, textures);}
inline void CreateTransformFeedbacks(GLsizei n, GLuint* ids){detail::CreateTransformFeedbacks(n, ids);}
inline void CreateVertexArrays(GLsizei n, GLuint* arrays){detail::CreateVertexArrays(n, arrays);}
inline void DisableVertexArrayAttrib(GLuint vaobj, GLuint index){detail::DisableVertexArrayAttrib(vaobj, index);}
inline void EnableVertexArrayAttrib(GLuint vaobj, GLuint index){detail::EnableVertexArrayAttrib(vaobj, index);}
inline void FlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length){detail::FlushMappedNamedBufferRange(buffer, offset, length);}
inline void GenerateTextureMipmap(GLuint texture){detail::GenerateTextureMipmap(texture);}
inline void GetCompressedTextureImage(GLuint texture, GLint level, GLsizei bufSize, void* pixels){detail::GetCompressedTextureImage(texture, level, bufSize, pixels);}
inline void GetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params){detail::GetNamedBufferParameteri64v(buffer, pname, params);}
inline void GetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params){detail::GetNamedBufferParameteriv(buffer, pname, params);}
inline void GetNamedBufferPointerv(GLuint buffer, GLenum pname, void** params){detail::GetNamedBufferPointerv(buffer, pname, params);}
inline void GetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data){detail::GetNamedBufferSubData(buffer, offset, size, data);}
inline void GetNamedFramebufferAttachmentParameteriv(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params){detail::GetNamedFramebufferAttachmentParameteriv(framebuffer, attachment, pname, params);}
inline void GetNamedFramebufferParameteriv(GLuint framebuffer, GLenum pname, GLint* param){detail::GetNamedFramebufferParameteriv(framebuffer, pname, param);}
inline void GetNamedRenderbufferParameteriv(GLuint renderbuffer, GLenum pname, GLint* params){detail::GetNamedRenderbufferParameteriv(renderbuffer, pname, params);}
inline void GetQueryBufferObjecti64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset){detail::GetQueryBufferObjecti64v(id, buffer, pname, offset);}
inline void GetQueryBufferObjectiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset){detail::GetQueryBufferObjectiv(id, buffer, pname, offset);}
inline void GetQueryBufferObjectui64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset){detail::GetQueryBufferObjectui64v(id, buffer, pname, offset);}
inline void GetQueryBufferObjectuiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset){detail::GetQueryBufferObjectuiv(id, buffer, pname, offset);}
inline void GetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels){detail::GetTextureImage(texture, level, format, type, bufSize, pixels);}
inline void GetTextureLevelParameterfv(GLuint texture, GLint level, GLenum pname, GLfloat* params){detail::GetTextureLevelParameterfv(texture, level, pname, params);}
inline void GetTextureLevelParameteriv(GLuint texture, GLint level, GLenum pname, GLint* params){detail::GetTextureLevelParameteriv(texture, level, pname, params);}
inline void GetTextureParameterIiv(GLuint texture, GLenum pname, GLint* params){detail::GetTextureParameterIiv(texture, pname, params);}
inline void GetTextureParameterIuiv(GLuint texture, GLenum pname, GLuint* params){detail::GetTextureParameterIuiv(texture, pname, params);}
inline void GetTextureParameterfv(GLuint texture, GLenum pname, GLfloat* params){detail::GetTextureParameterfv(texture, pname, params);}
inline void GetTextureParameteriv(GLuint texture, GLenum pname, GLint* params){detail::GetTextureParameteriv(texture, pname, params);}
inline void GetTransformFeedbacki64_v(GLuint xfb, GLenum pname, GLuint index, GLint64* param){detail::GetTransformFeedbacki64_v(xfb, pname, index, param);}
inline void GetTransformFeedbacki_v(GLuint xfb, GLenum pname, GLuint index, GLint* param){detail::GetTransformFeedbacki_v(xfb, pname, index, param);}
inline void GetTransformFeedbackiv(GLuint xfb, GLenum pname, GLint* param){detail::GetTransformFeedbackiv(xfb, pname, param);}
inline void GetVertexArrayIndexed64iv(GLuint vaobj, GLuint index, GLenum pname, GLint64* param){detail::GetVertexArrayIndexed64iv(vaobj, index, pname, param);}
inline void GetVertexArrayIndexediv(GLuint vaobj, GLuint index, GLenum pname, GLint* param){detail::GetVertexArrayIndexediv(vaobj, index, pname, param);}
inline void GetVertexArrayiv(GLuint vaobj, GLenum pname, GLint* param){detail::GetVertexArrayiv(vaobj, pname, param);}
inline void InvalidateNamedFramebufferData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments){detail::InvalidateNamedFramebufferData(framebuffer, numAttachments, attachments);}
inline void InvalidateNamedFramebufferSubData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height){detail::InvalidateNamedFramebufferSubData(framebuffer, numAttachments, attachments, x, y, width, height);}
inline void* MapNamedBuffer(GLuint buffer, GLenum access){return detail::MapNamedBuffer(buffer, access);}
inline void* MapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access){return detail::MapNamedBufferRange(buffer, offset, length, access);}
inline void NamedBufferData(GLuint buffer, GLsizeiptr size, const void* data, GLenum usage){detail::NamedBufferData(buffer, size, data, usage);}
inline void NamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags){detail::NamedBufferStorage(buffer, size, data, flags);}
inline void NamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data){detail::NamedBufferSubData(buffer, offset, size, data);}
inline void NamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf){detail::NamedFramebufferDrawBuffer(framebuffer, buf);}
inline void NamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs){detail::NamedFramebufferDrawBuffers(framebuffer, n, bufs);}
inline void NamedFramebufferParameteri(GLuint framebuffer, GLenum pname, GLint param){detail::NamedFramebufferParameteri(framebuffer, pname, param);}
inline void NamedFramebufferReadBuffer(GLuint framebuffer, GLenum src){detail::NamedFramebufferReadBuffer(framebuffer, src);}
inline void NamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer){detail::NamedFramebufferRenderbuffer(framebuffer, attachment, renderbuffertarget, renderbuffer);}
inline void NamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level){detail::NamedFramebufferTexture(framebuffer, attachment, texture, level);}
inline void NamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer){detail::NamedFramebufferTextureLayer(framebuffer, attachment, texture, level, layer);}
inline void NamedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height){detail::NamedRenderbufferStorage(renderbuffer, internalformat, width, height);}
inline void NamedRenderbufferStorageMultisample(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height){detail::NamedRenderbufferStorageMultisample(renderbuffer, samples, internalformat, width, height);}
inline void TextureBuffer(GLuint texture, GLenum internalformat, GLuint buffer){detail::TextureBuffer(texture, internalformat, buffer);}
inline void TextureBufferRange(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size){detail::TextureBufferRange(texture, internalformat, buffer, offset, size);}
inline void TextureParameterIiv(GLuint texture, GLenum pname, const GLint* params){detail::TextureParameterIiv(texture, pname, params);}
inline void TextureParameterIuiv(GLuint texture, GLenum pname, const GLuint* params){detail::TextureParameterIuiv(texture, pname, params);}
inline void TextureParameterf(GLuint texture, GLenum pname, GLfloat param){detail::TextureParameterf(texture, pname, param);}
inline void TextureParameterfv(GLuint texture, GLenum pname, const GLfloat* param){detail::TextureParameterfv(texture, pname, param);}
inline void TextureParameteri(GLuint texture, GLenum pname, GLint param){detail::TextureParameteri(texture, pname, param);}
inline void TextureParameteriv(GLuint texture, GLenum pname, const GLint* param){detail::TextureParameteriv(texture, pname, param);}
inline void TextureStorage1D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width){detail::TextureStorage1D(texture, levels, internalformat, width);}
inline void TextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height){detail::TextureStorage2D(texture, levels, internalformat, width, height);}
inline void TextureStorage2DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations){detail::TextureStorage2DMultisample(texture, samples, internalformat, width, height, fixedsamplelocations);}
inline void TextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth){detail::TextureStorage3D(texture, levels, internalformat, width, height, depth);}
inline void TextureStorage3DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations){detail::TextureStorage3DMultisample(texture, samples, internalformat, width, height, depth, fixedsamplelocations);}
inline void TextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels){detail::TextureSubImage1D(texture, level, xoffset, width, format, type, pixels);}
inline void TextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels){detail::TextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, type, pixels);}
inline void TextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels){detail::TextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);}
inline void TransformFeedbackBufferBase(GLuint xfb, GLuint index, GLuint buffer){detail::TransformFeedbackBufferBase(xfb, index, buffer);}
inline void TransformFeedbackBufferRange(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size){detail::TransformFeedbackBufferRange(xfb, index, buffer, offset, size);}
inline GLboolean UnmapNamedBuffer(GLuint buffer){return detail::UnmapNamedBuffer(buffer);}
inline void VertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex){detail::VertexArrayAttribBinding(vaobj, attribindex, bindingindex);}
inline void VertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset){detail::VertexArrayAttribFormat(vaobj, attribindex, size, type, normalized, relativeoffset);}
inline void VertexArrayAttribIFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset){detail::VertexArrayAttribIFormat(vaobj, attribindex, size, type, relativeoffset);}
inline void VertexArrayAttribLFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset){detail::VertexArrayAttribLFormat(vaobj, attribindex, size, type, relativeoffset);}
inline void VertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor){detail::VertexArrayBindingDivisor(vaobj, bindingindex, divisor);}
inline void VertexArrayElementBuffer(GLuint vaobj, GLuint buffer){detail::VertexArrayElementBuffer(vaobj, buffer);}
inline void VertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride){detail::VertexArrayVertexBuffer(vaobj, bindingindex, buffer, offset, stride);}
inline void VertexArrayVertexBuffers(GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides){detail::VertexArrayVertexBuffers(vaobj, first, count, buffers, offsets, strides);}

inline void ActiveShaderProgram(GLuint pipeline, GLuint program){detail::ActiveShaderProgram(pipeline, program);}
inline void BindProgramPipeline(GLuint pipeline){detail::BindProgramPipeline(pipeline);}
inline GLuint CreateShaderProgramv(GLenum type, GLsizei count, const GLchar* const * strings){return detail::CreateShaderProgramv(type, count, strings);}
inline void DeleteProgramPipelines(GLsizei n, const GLuint* pipelines){detail::DeleteProgramPipelines(n, pipelines);}
inline void GenProgramPipelines(GLsizei n, GLuint* pipelines){detail::GenProgramPipelines(n, pipelines);}
inline void GetProgramPipelineInfoLog(GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog){detail::GetProgramPipelineInfoLog(pipeline, bufSize, length, infoLog);}
inline void GetProgramPipelineiv(GLuint pipeline, GLenum pname, GLint* params){detail::GetProgramPipelineiv(pipeline, pname, params);}
inline GLboolean IsProgramPipeline(GLuint pipeline){return detail::IsProgramPipeline(pipeline);}
inline void ProgramUniform1d(GLuint program, GLint location, GLdouble v0){detail::ProgramUniform1d(program, location, v0);}
inline void ProgramUniform1dv(GLuint program, GLint location, GLsizei count, const GLdouble* value){detail::ProgramUniform1dv(program, location, count, value);}
inline void ProgramUniform1f(GLuint program, GLint location, GLfloat v0){detail::ProgramUniform1f(program, location, v0);}
inline void ProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat* value){detail::ProgramUniform1fv(program, location, count, value);}
inline void ProgramUniform1i(GLuint program, GLint location, GLint v0){detail::ProgramUniform1i(program, location, v0);}
inline void ProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint* value){detail::ProgramUniform1iv(program, location, count, value);}
inline void ProgramUniform1ui(GLuint program, GLint location, GLuint v0){detail::ProgramUniform1ui(program, location, v0);}
inline void ProgramUniform1uiv(GLuint program, GLint location, GLsizei count, const GLuint* value){detail::ProgramUniform1uiv(program, location, count, value);}
inline void ProgramUniform2d(GLuint program, GLint location, GLdouble v0, GLdouble v1){detail::ProgramUniform2d(program, location, v0, v1);}
inline void ProgramUniform2dv(GLuint program, GLint location, GLsizei count, const GLdouble* value){detail::ProgramUniform2dv(program, location, count, value);}
inline void ProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1){detail::ProgramUniform2f(program, location, v0, v1);}
inline void ProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat* value){detail::ProgramUniform2fv(program, location, count, value);}
inline void ProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1){detail::ProgramUniform2i(program, location, v0, v1);}
inline void ProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint* value){detail::ProgramUniform2iv(program, location, count, value);}
inline void ProgramUniform2ui(GLuint program, GLint location, GLuint v0, GLuint v1){detail::ProgramUniform2ui(program, location, v0, v1);}
inline void ProgramUniform2uiv(GLuint program, GLint location, GLsizei count, const GLuint* value){detail::ProgramUniform2uiv(program, location, count, value);}
inline void ProgramUniform3d(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2){detail::ProgramUniform3d(program, location, v0, v1, v2);}
inline void ProgramUniform3dv(GLuint program, GLint location, GLsizei count, const GLdouble* value){detail::ProgramUniform3dv(program, location, count, value);}
inline void ProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2){detail::ProgramUniform3f(program, location, v0, v1, v2);}
inline void ProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat* value){detail::ProgramUniform3fv(program, location, count, value);}
inline void ProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2){detail::ProgramUniform3i(program, location, v0, v1, v2);}
inline void ProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint* value){detail::ProgramUniform3iv(program, location, count, value);}
inline void ProgramUniform3ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2){detail::ProgramUniform3ui(program, location, v0, v1, v2);}
inline void ProgramUniform3uiv(GLuint program, GLint location, GLsizei count, const GLuint* value){detail::ProgramUniform3uiv(program, location, count, value);}
inline void ProgramUniform4d(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3){detail::ProgramUniform4d(program, location, v0, v1, v2, v3);}
inline void ProgramUniform4dv(GLuint program, GLint location, GLsizei count, const GLdouble* value){detail::ProgramUniform4dv(program, location, count, value);}
inline void ProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){detail::ProgramUniform4f(program, location, v0, v1, v2, v3);}
inline void ProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value){detail::ProgramUniform4fv(program, location, count, value);}
inline void ProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3){detail::ProgramUniform4i(program, location, v0, v1, v2, v3);}
inline void ProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint* value){detail::ProgramUniform4iv(program, location, count, value);}
inline void ProgramUniform4ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3){detail::ProgramUniform4ui(program, location, v0, v1, v2, v3);}
inline void ProgramUniform4uiv(GLuint program, GLint location, GLsizei count, const GLuint* value){detail::ProgramUniform4uiv(program, location, count, value);}
inline void ProgramUniformMatrix2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value){detail::ProgramUniformMatrix2dv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::ProgramUniformMatrix2fv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix2x3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value){detail::ProgramUniformMatrix2x3dv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix2x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::ProgramUniformMatrix2x3fv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix2x4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value){detail::ProgramUniformMatrix2x4dv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix2x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::ProgramUniformMatrix2x4fv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value){detail::ProgramUniformMatrix3dv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::ProgramUniformMatrix3fv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix3x2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value){detail::ProgramUniformMatrix3x2dv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix3x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::ProgramUniformMatrix3x2fv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix3x4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value){detail::ProgramUniformMatrix3x4dv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix3x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::ProgramUniformMatrix3x4fv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value){detail::ProgramUniformMatrix4dv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::ProgramUniformMatrix4fv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix4x2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value){detail::ProgramUniformMatrix4x2dv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix4x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::ProgramUniformMatrix4x2fv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix4x3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value){detail::ProgramUniformMatrix4x3dv(program, location, count, transpose, value);}
inline void ProgramUniformMatrix4x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::ProgramUniformMatrix4x3fv(program, location, count, transpose, value);}
inline void UseProgramStages(GLuint pipeline, GLbitfield stages, GLuint program){detail::UseProgramStages(pipeline, stages, program);}
inline void ValidateProgramPipeline(GLuint pipeline){detail::ValidateProgramPipeline(pipeline);}

inline void BlendFunc(GLenum sfactor, GLenum dfactor){detail::BlendFunc(sfactor, dfactor);}
inline void Clear(GLbitfield mask){detail::Clear(mask);}
inline void ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){detail::ClearColor(red, green, blue, alpha);}
inline void ClearDepth(GLdouble depth){detail::ClearDepth(depth);}
inline void ClearStencil(GLint s){detail::ClearStencil(s);}
inline void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha){detail::ColorMask(red, green, blue, alpha);}
inline void CullFace(GLenum mode){detail::CullFace(mode);}
inline void DepthFunc(GLenum func){detail::DepthFunc(func);}
inline void DepthMask(GLboolean flag){detail::DepthMask(flag);}
inline void DepthRange(GLdouble ren_near, GLdouble ren_far){detail::DepthRange(ren_near, ren_far);}
inline void Disable(GLenum cap){detail::Disable(cap);}
inline void DrawBuffer(GLenum buf){detail::DrawBuffer(buf);}
inline void Enable(GLenum cap){detail::Enable(cap);}
inline void Finish(void){detail::Finish();}
inline void Flush(void){detail::Flush();}
inline void FrontFace(GLenum mode){detail::FrontFace(mode);}
inline void GetBooleanv(GLenum pname, GLboolean* data){detail::GetBooleanv(pname, data);}
inline void GetDoublev(GLenum pname, GLdouble* data){detail::GetDoublev(pname, data);}
inline GLenum GetError(void){return detail::GetError();}
inline void GetFloatv(GLenum pname, GLfloat* data){detail::GetFloatv(pname, data);}
inline void GetIntegerv(GLenum pname, GLint* data){detail::GetIntegerv(pname, data);}
inline const GLubyte* GetString(GLenum name){return detail::GetString(name);}
inline void GetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels){detail::GetTexImage(target, level, format, type, pixels);}
inline void GetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat* params){detail::GetTexLevelParameterfv(target, level, pname, params);}
inline void GetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params){detail::GetTexLevelParameteriv(target, level, pname, params);}
inline void GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params){detail::GetTexParameterfv(target, pname, params);}
inline void GetTexParameteriv(GLenum target, GLenum pname, GLint* params){detail::GetTexParameteriv(target, pname, params);}
inline void Hint(GLenum target, GLenum mode){detail::Hint(target, mode);}
inline GLboolean IsEnabled(GLenum cap){return detail::IsEnabled(cap);}
inline void LineWidth(GLfloat width){detail::LineWidth(width);}
inline void LogicOp(GLenum opcode){detail::LogicOp(opcode);}
inline void PixelStoref(GLenum pname, GLfloat param){detail::PixelStoref(pname, param);}
inline void PixelStorei(GLenum pname, GLint param){detail::PixelStorei(pname, param);}
inline void PointSize(GLfloat size){detail::PointSize(size);}
inline void PolygonMode(GLenum face, GLenum mode){detail::PolygonMode(face, mode);}
inline void ReadBuffer(GLenum src){detail::ReadBuffer(src);}
inline void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels){detail::ReadPixels(x, y, width, height, format, type, pixels);}
inline void Scissor(GLint x, GLint y, GLsizei width, GLsizei height){detail::Scissor(x, y, width, height);}
inline void StencilFunc(GLenum func, GLint ref, GLuint mask){detail::StencilFunc(func, ref, mask);}
inline void StencilMask(GLuint mask){detail::StencilMask(mask);}
inline void StencilOp(GLenum fail, GLenum zfail, GLenum zpass){detail::StencilOp(fail, zfail, zpass);}
inline void TexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void* pixels){detail::TexImage1D(target, level, internalformat, width, border, format, type, pixels);}
inline void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels){detail::TexImage2D(target, level, internalformat, width, height, border, format, type, pixels);}
inline void TexParameterf(GLenum target, GLenum pname, GLfloat param){detail::TexParameterf(target, pname, param);}
inline void TexParameterfv(GLenum target, GLenum pname, const GLfloat* params){detail::TexParameterfv(target, pname, params);}
inline void TexParameteri(GLenum target, GLenum pname, GLint param){detail::TexParameteri(target, pname, param);}
inline void TexParameteriv(GLenum target, GLenum pname, const GLint* params){detail::TexParameteriv(target, pname, params);}
inline void Viewport(GLint x, GLint y, GLsizei width, GLsizei height){detail::Viewport(x, y, width, height);}

inline void BindTexture(GLenum target, GLuint texture){detail::BindTexture(target, texture);}
inline void CopyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border){detail::CopyTexImage1D(target, level, internalformat, x, y, width, border);}
inline void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border){detail::CopyTexImage2D(target, level, internalformat, x, y, width, height, border);}
inline void CopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width){detail::CopyTexSubImage1D(target, level, xoffset, x, y, width);}
inline void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height){detail::CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);}
inline void DeleteTextures(GLsizei n, const GLuint* textures){detail::DeleteTextures(n, textures);}
inline void DrawArrays(GLenum mode, GLint first, GLsizei count){detail::DrawArrays(mode, first, count);}
inline void DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices){detail::DrawElements(mode, count, type, indices);}
inline void GenTextures(GLsizei n, GLuint* textures){detail::GenTextures(n, textures);}
inline GLboolean IsTexture(GLuint texture){return detail::IsTexture(texture);}
inline void PolygonOffset(GLfloat factor, GLfloat units){detail::PolygonOffset(factor, units);}
inline void TexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels){detail::TexSubImage1D(target, level, xoffset, width, format, type, pixels);}
inline void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels){detail::TexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);}

inline void CopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height){detail::CopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);}
inline void DrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices){detail::DrawRangeElements(mode, start, end, count, type, indices);}
inline void TexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels){detail::TexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);}
inline void TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels){detail::TexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);}

inline void ActiveTexture(GLenum texture){detail::ActiveTexture(texture);}
inline void CompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void* data){detail::CompressedTexImage1D(target, level, internalformat, width, border, imageSize, data);}
inline void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data){detail::CompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);}
inline void CompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data){detail::CompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);}
inline void CompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data){detail::CompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data);}
inline void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data){detail::CompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);}
inline void CompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data){detail::CompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);}
inline void GetCompressedTexImage(GLenum target, GLint level, void* img){detail::GetCompressedTexImage(target, level, img);}
inline void SampleCoverage(GLfloat value, GLboolean invert){detail::SampleCoverage(value, invert);}

inline void BlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){detail::BlendColor(red, green, blue, alpha);}
inline void BlendEquation(GLenum mode){detail::BlendEquation(mode);}
inline void BlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha){detail::BlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);}
inline void MultiDrawArrays(GLenum mode, const GLint* first, const GLsizei* count, GLsizei drawcount){detail::MultiDrawArrays(mode, first, count, drawcount);}
inline void MultiDrawElements(GLenum mode, const GLsizei* count, GLenum type, const void* const * indices, GLsizei drawcount){detail::MultiDrawElements(mode, count, type, indices, drawcount);}
inline void PointParameterf(GLenum pname, GLfloat param){detail::PointParameterf(pname, param);}
inline void PointParameterfv(GLenum pname, const GLfloat* params){detail::PointParameterfv(pname, params);}
inline void PointParameteri(GLenum pname, GLint param){detail::PointParameteri(pname, param);}
inline void PointParameteriv(GLenum pname, const GLint* params){detail::PointParameteriv(pname, params);}

inline void BeginQuery(GLenum target, GLuint id){detail::BeginQuery(target, id);}
inline void BindBuffer(GLenum target, GLuint buffer){detail::BindBuffer(target, buffer);}
inline void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage){detail::BufferData(target, size, data, usage);}
inline void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data){detail::BufferSubData(target, offset, size, data);}
inline void DeleteBuffers(GLsizei n, const GLuint* buffers){detail::DeleteBuffers(n, buffers);}
inline void DeleteQueries(GLsizei n, const GLuint* ids){detail::DeleteQueries(n, ids);}
inline void EndQuery(GLenum target){detail::EndQuery(target);}
inline void GenBuffers(GLsizei n, GLuint* buffers){detail::GenBuffers(n, buffers);}
inline void GenQueries(GLsizei n, GLuint* ids){detail::GenQueries(n, ids);}
inline void GetBufferParameteriv(GLenum target, GLenum pname, GLint* params){detail::GetBufferParameteriv(target, pname, params);}
inline void GetBufferPointerv(GLenum target, GLenum pname, void** params){detail::GetBufferPointerv(target, pname, params);}
inline void GetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, void* data){detail::GetBufferSubData(target, offset, size, data);}
inline void GetQueryObjectiv(GLuint id, GLenum pname, GLint* params){detail::GetQueryObjectiv(id, pname, params);}
inline void GetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params){detail::GetQueryObjectuiv(id, pname, params);}
inline void GetQueryiv(GLenum target, GLenum pname, GLint* params){detail::GetQueryiv(target, pname, params);}
inline GLboolean IsBuffer(GLuint buffer){return detail::IsBuffer(buffer);}
inline GLboolean IsQuery(GLuint id){return detail::IsQuery(id);}
inline void* MapBuffer(GLenum target, GLenum access){return detail::MapBuffer(target, access);}
inline GLboolean UnmapBuffer(GLenum target){return detail::UnmapBuffer(target);}

inline void AttachShader(GLuint program, GLuint shader){detail::AttachShader(program, shader);}
inline void BindAttribLocation(GLuint program, GLuint index, const GLchar* name){detail::BindAttribLocation(program, index, name);}
inline void BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha){detail::BlendEquationSeparate(modeRGB, modeAlpha);}
inline void CompileShader(GLuint shader){detail::CompileShader(shader);}
inline GLuint CreateProgram(void){return detail::CreateProgram();}
inline GLuint CreateShader(GLenum type){return detail::CreateShader(type);}
inline void DeleteProgram(GLuint program){detail::DeleteProgram(program);}
inline void DeleteShader(GLuint shader){detail::DeleteShader(shader);}
inline void DetachShader(GLuint program, GLuint shader){detail::DetachShader(program, shader);}
inline void DisableVertexAttribArray(GLuint index){detail::DisableVertexAttribArray(index);}
inline void DrawBuffers(GLsizei n, const GLenum* bufs){detail::DrawBuffers(n, bufs);}
inline void EnableVertexAttribArray(GLuint index){detail::EnableVertexAttribArray(index);}
inline void GetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name){detail::GetActiveAttrib(program, index, bufSize, length, size, type, name);}
inline void GetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name){detail::GetActiveUniform(program, index, bufSize, length, size, type, name);}
inline void GetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders){detail::GetAttachedShaders(program, maxCount, count, shaders);}
inline GLint GetAttribLocation(GLuint program, const GLchar* name){return detail::GetAttribLocation(program, name);}
inline void GetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog){detail::GetProgramInfoLog(program, bufSize, length, infoLog);}
inline void GetProgramiv(GLuint program, GLenum pname, GLint* params){detail::GetProgramiv(program, pname, params);}
inline void GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog){detail::GetShaderInfoLog(shader, bufSize, length, infoLog);}
inline void GetShaderSource(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source){detail::GetShaderSource(shader, bufSize, length, source);}
inline void GetShaderiv(GLuint shader, GLenum pname, GLint* params){detail::GetShaderiv(shader, pname, params);}
inline GLint GetUniformLocation(GLuint program, const GLchar* name){return detail::GetUniformLocation(program, name);}
inline void GetUniformfv(GLuint program, GLint location, GLfloat* params){detail::GetUniformfv(program, location, params);}
inline void GetUniformiv(GLuint program, GLint location, GLint* params){detail::GetUniformiv(program, location, params);}
inline void GetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer){detail::GetVertexAttribPointerv(index, pname, pointer);}
inline void GetVertexAttribdv(GLuint index, GLenum pname, GLdouble* params){detail::GetVertexAttribdv(index, pname, params);}
inline void GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params){detail::GetVertexAttribfv(index, pname, params);}
inline void GetVertexAttribiv(GLuint index, GLenum pname, GLint* params){detail::GetVertexAttribiv(index, pname, params);}
inline GLboolean IsProgram(GLuint program){return detail::IsProgram(program);}
inline GLboolean IsShader(GLuint shader){return detail::IsShader(shader);}
inline void LinkProgram(GLuint program){detail::LinkProgram(program);}
inline void ShaderSource(GLuint shader, GLsizei count, const GLchar* const * string, const GLint* length){detail::ShaderSource(shader, count, string, length);}
inline void StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask){detail::StencilFuncSeparate(face, func, ref, mask);}
inline void StencilMaskSeparate(GLenum face, GLuint mask){detail::StencilMaskSeparate(face, mask);}
inline void StencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass){detail::StencilOpSeparate(face, sfail, dpfail, dppass);}
inline void Uniform1f(GLint location, GLfloat v0){detail::Uniform1f(location, v0);}
inline void Uniform1fv(GLint location, GLsizei count, const GLfloat* value){detail::Uniform1fv(location, count, value);}
inline void Uniform1i(GLint location, GLint v0){detail::Uniform1i(location, v0);}
inline void Uniform1iv(GLint location, GLsizei count, const GLint* value){detail::Uniform1iv(location, count, value);}
inline void Uniform2f(GLint location, GLfloat v0, GLfloat v1){detail::Uniform2f(location, v0, v1);}
inline void Uniform2fv(GLint location, GLsizei count, const GLfloat* value){detail::Uniform2fv(location, count, value);}
inline void Uniform2i(GLint location, GLint v0, GLint v1){detail::Uniform2i(location, v0, v1);}
inline void Uniform2iv(GLint location, GLsizei count, const GLint* value){detail::Uniform2iv(location, count, value);}
inline void Uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2){detail::Uniform3f(location, v0, v1, v2);}
inline void Uniform3fv(GLint location, GLsizei count, const GLfloat* value){detail::Uniform3fv(location, count, value);}
inline void Uniform3i(GLint location, GLint v0, GLint v1, GLint v2){detail::Uniform3i(location, v0, v1, v2);}
inline void Uniform3iv(GLint location, GLsizei count, const GLint* value){detail::Uniform3iv(location, count, value);}
inline void Uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){detail::Uniform4f(location, v0, v1, v2, v3);}
inline void Uniform4fv(GLint location, GLsizei count, const GLfloat* value){detail::Uniform4fv(location, count, value);}
inline void Uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3){detail::Uniform4i(location, v0, v1, v2, v3);}
inline void Uniform4iv(GLint location, GLsizei count, const GLint* value){detail::Uniform4iv(location, count, value);}
inline void UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::UniformMatrix2fv(location, count, transpose, value);}
inline void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::UniformMatrix3fv(location, count, transpose, value);}
inline void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::UniformMatrix4fv(location, count, transpose, value);}
inline void UseProgram(GLuint program){detail::UseProgram(program);}
inline void ValidateProgram(GLuint program){detail::ValidateProgram(program);}
inline void VertexAttrib1d(GLuint index, GLdouble x){detail::VertexAttrib1d(index, x);}
inline void VertexAttrib1dv(GLuint index, const GLdouble* v){detail::VertexAttrib1dv(index, v);}
inline void VertexAttrib1f(GLuint index, GLfloat x){detail::VertexAttrib1f(index, x);}
inline void VertexAttrib1fv(GLuint index, const GLfloat* v){detail::VertexAttrib1fv(index, v);}
inline void VertexAttrib1s(GLuint index, GLshort x){detail::VertexAttrib1s(index, x);}
inline void VertexAttrib1sv(GLuint index, const GLshort* v){detail::VertexAttrib1sv(index, v);}
inline void VertexAttrib2d(GLuint index, GLdouble x, GLdouble y){detail::VertexAttrib2d(index, x, y);}
inline void VertexAttrib2dv(GLuint index, const GLdouble* v){detail::VertexAttrib2dv(index, v);}
inline void VertexAttrib2f(GLuint index, GLfloat x, GLfloat y){detail::VertexAttrib2f(index, x, y);}
inline void VertexAttrib2fv(GLuint index, const GLfloat* v){detail::VertexAttrib2fv(index, v);}
inline void VertexAttrib2s(GLuint index, GLshort x, GLshort y){detail::VertexAttrib2s(index, x, y);}
inline void VertexAttrib2sv(GLuint index, const GLshort* v){detail::VertexAttrib2sv(index, v);}
inline void VertexAttrib3d(GLuint index, GLdouble x, GLdouble y, GLdouble z){detail::VertexAttrib3d(index, x, y, z);}
inline void VertexAttrib3dv(GLuint index, const GLdouble* v){detail::VertexAttrib3dv(index, v);}
inline void VertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z){detail::VertexAttrib3f(index, x, y, z);}
inline void VertexAttrib3fv(GLuint index, const GLfloat* v){detail::VertexAttrib3fv(index, v);}
inline void VertexAttrib3s(GLuint index, GLshort x, GLshort y, GLshort z){detail::VertexAttrib3s(index, x, y, z);}
inline void VertexAttrib3sv(GLuint index, const GLshort* v){detail::VertexAttrib3sv(index, v);}
inline void VertexAttrib4Nbv(GLuint index, const GLbyte* v){detail::VertexAttrib4Nbv(index, v);}
inline void VertexAttrib4Niv(GLuint index, const GLint* v){detail::VertexAttrib4Niv(index, v);}
inline void VertexAttrib4Nsv(GLuint index, const GLshort* v){detail::VertexAttrib4Nsv(index, v);}
inline void VertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w){detail::VertexAttrib4Nub(index, x, y, z, w);}
inline void VertexAttrib4Nubv(GLuint index, const GLubyte* v){detail::VertexAttrib4Nubv(index, v);}
inline void VertexAttrib4Nuiv(GLuint index, const GLuint* v){detail::VertexAttrib4Nuiv(index, v);}
inline void VertexAttrib4Nusv(GLuint index, const GLushort* v){detail::VertexAttrib4Nusv(index, v);}
inline void VertexAttrib4bv(GLuint index, const GLbyte* v){detail::VertexAttrib4bv(index, v);}
inline void VertexAttrib4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w){detail::VertexAttrib4d(index, x, y, z, w);}
inline void VertexAttrib4dv(GLuint index, const GLdouble* v){detail::VertexAttrib4dv(index, v);}
inline void VertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w){detail::VertexAttrib4f(index, x, y, z, w);}
inline void VertexAttrib4fv(GLuint index, const GLfloat* v){detail::VertexAttrib4fv(index, v);}
inline void VertexAttrib4iv(GLuint index, const GLint* v){detail::VertexAttrib4iv(index, v);}
inline void VertexAttrib4s(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w){detail::VertexAttrib4s(index, x, y, z, w);}
inline void VertexAttrib4sv(GLuint index, const GLshort* v){detail::VertexAttrib4sv(index, v);}
inline void VertexAttrib4ubv(GLuint index, const GLubyte* v){detail::VertexAttrib4ubv(index, v);}
inline void VertexAttrib4uiv(GLuint index, const GLuint* v){detail::VertexAttrib4uiv(index, v);}
inline void VertexAttrib4usv(GLuint index, const GLushort* v){detail::VertexAttrib4usv(index, v);}
inline void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer){detail::VertexAttribPointer(index, size, type, normalized, stride, pointer);}

inline void UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::UniformMatrix2x3fv(location, count, transpose, value);}
inline void UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::UniformMatrix2x4fv(location, count, transpose, value);}
inline void UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::UniformMatrix3x2fv(location, count, transpose, value);}
inline void UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::UniformMatrix3x4fv(location, count, transpose, value);}
inline void UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::UniformMatrix4x2fv(location, count, transpose, value);}
inline void UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){detail::UniformMatrix4x3fv(location, count, transpose, value);}

inline void BeginConditionalRender(GLuint id, GLenum mode){detail::BeginConditionalRender(id, mode);}
inline void BeginTransformFeedback(GLenum primitiveMode){detail::BeginTransformFeedback(primitiveMode);}
inline void BindBufferBase(GLenum target, GLuint index, GLuint buffer){detail::BindBufferBase(target, index, buffer);}
inline void BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size){detail::BindBufferRange(target, index, buffer, offset, size);}
inline void BindFragDataLocation(GLuint program, GLuint color, const GLchar* name){detail::BindFragDataLocation(program, color, name);}
inline void BindFramebuffer(GLenum target, GLuint framebuffer){detail::BindFramebuffer(target, framebuffer);}
inline void BindRenderbuffer(GLenum target, GLuint renderbuffer){detail::BindRenderbuffer(target, renderbuffer);}
inline void BindVertexArray(GLuint ren_array){detail::BindVertexArray(ren_array);}
inline void BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter){detail::BlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);}
inline GLenum CheckFramebufferStatus(GLenum target){return detail::CheckFramebufferStatus(target);}
inline void ClampColor(GLenum target, GLenum clamp){detail::ClampColor(target, clamp);}
inline void ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil){detail::ClearBufferfi(buffer, drawbuffer, depth, stencil);}
inline void ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value){detail::ClearBufferfv(buffer, drawbuffer, value);}
inline void ClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value){detail::ClearBufferiv(buffer, drawbuffer, value);}
inline void ClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value){detail::ClearBufferuiv(buffer, drawbuffer, value);}
inline void ColorMaski(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a){detail::ColorMaski(index, r, g, b, a);}
inline void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers){detail::DeleteFramebuffers(n, framebuffers);}
inline void DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers){detail::DeleteRenderbuffers(n, renderbuffers);}
inline void DeleteVertexArrays(GLsizei n, const GLuint* arrays){detail::DeleteVertexArrays(n, arrays);}
inline void Disablei(GLenum target, GLuint index){detail::Disablei(target, index);}
inline void Enablei(GLenum target, GLuint index){detail::Enablei(target, index);}
inline void EndConditionalRender(void){detail::EndConditionalRender();}
inline void EndTransformFeedback(void){detail::EndTransformFeedback();}
inline void FlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length){detail::FlushMappedBufferRange(target, offset, length);}
inline void FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer){detail::FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);}
inline void FramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level){detail::FramebufferTexture1D(target, attachment, textarget, texture, level);}
inline void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level){detail::FramebufferTexture2D(target, attachment, textarget, texture, level);}
inline void FramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset){detail::FramebufferTexture3D(target, attachment, textarget, texture, level, zoffset);}
inline void FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer){detail::FramebufferTextureLayer(target, attachment, texture, level, layer);}
inline void GenFramebuffers(GLsizei n, GLuint* framebuffers){detail::GenFramebuffers(n, framebuffers);}
inline void GenRenderbuffers(GLsizei n, GLuint* renderbuffers){detail::GenRenderbuffers(n, renderbuffers);}
inline void GenVertexArrays(GLsizei n, GLuint* arrays){detail::GenVertexArrays(n, arrays);}
inline void GenerateMipmap(GLenum target){detail::GenerateMipmap(target);}
inline void GetBooleani_v(GLenum target, GLuint index, GLboolean* data){detail::GetBooleani_v(target, index, data);}
inline GLint GetFragDataLocation(GLuint program, const GLchar* name){return detail::GetFragDataLocation(program, name);}
inline void GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params){detail::GetFramebufferAttachmentParameteriv(target, attachment, pname, params);}
inline void GetIntegeri_v(GLenum target, GLuint index, GLint* data){detail::GetIntegeri_v(target, index, data);}
inline void GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params){detail::GetRenderbufferParameteriv(target, pname, params);}
inline const GLubyte* GetStringi(GLenum name, GLuint index){return detail::GetStringi(name, index);}
inline void GetTexParameterIiv(GLenum target, GLenum pname, GLint* params){detail::GetTexParameterIiv(target, pname, params);}
inline void GetTexParameterIuiv(GLenum target, GLenum pname, GLuint* params){detail::GetTexParameterIuiv(target, pname, params);}
inline void GetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name){detail::GetTransformFeedbackVarying(program, index, bufSize, length, size, type, name);}
inline void GetUniformuiv(GLuint program, GLint location, GLuint* params){detail::GetUniformuiv(program, location, params);}
inline void GetVertexAttribIiv(GLuint index, GLenum pname, GLint* params){detail::GetVertexAttribIiv(index, pname, params);}
inline void GetVertexAttribIuiv(GLuint index, GLenum pname, GLuint* params){detail::GetVertexAttribIuiv(index, pname, params);}
inline GLboolean IsEnabledi(GLenum target, GLuint index){return detail::IsEnabledi(target, index);}
inline GLboolean IsFramebuffer(GLuint framebuffer){return detail::IsFramebuffer(framebuffer);}
inline GLboolean IsRenderbuffer(GLuint renderbuffer){return detail::IsRenderbuffer(renderbuffer);}
inline GLboolean IsVertexArray(GLuint ren_array){return detail::IsVertexArray(ren_array);}
inline void* MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access){return detail::MapBufferRange(target, offset, length, access);}
inline void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height){detail::RenderbufferStorage(target, internalformat, width, height);}
inline void RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height){detail::RenderbufferStorageMultisample(target, samples, internalformat, width, height);}
inline void TexParameterIiv(GLenum target, GLenum pname, const GLint* params){detail::TexParameterIiv(target, pname, params);}
inline void TexParameterIuiv(GLenum target, GLenum pname, const GLuint* params){detail::TexParameterIuiv(target, pname, params);}
inline void TransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar* const * varyings, GLenum bufferMode){detail::TransformFeedbackVaryings(program, count, varyings, bufferMode);}
inline void Uniform1ui(GLint location, GLuint v0){detail::Uniform1ui(location, v0);}
inline void Uniform1uiv(GLint location, GLsizei count, const GLuint* value){detail::Uniform1uiv(location, count, value);}
inline void Uniform2ui(GLint location, GLuint v0, GLuint v1){detail::Uniform2ui(location, v0, v1);}
inline void Uniform2uiv(GLint location, GLsizei count, const GLuint* value){detail::Uniform2uiv(location, count, value);}
inline void Uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2){detail::Uniform3ui(location, v0, v1, v2);}
inline void Uniform3uiv(GLint location, GLsizei count, const GLuint* value){detail::Uniform3uiv(location, count, value);}
inline void Uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3){detail::Uniform4ui(location, v0, v1, v2, v3);}
inline void Uniform4uiv(GLint location, GLsizei count, const GLuint* value){detail::Uniform4uiv(location, count, value);}
inline void VertexAttribI1i(GLuint index, GLint x){detail::VertexAttribI1i(index, x);}
inline void VertexAttribI1iv(GLuint index, const GLint* v){detail::VertexAttribI1iv(index, v);}
inline void VertexAttribI1ui(GLuint index, GLuint x){detail::VertexAttribI1ui(index, x);}
inline void VertexAttribI1uiv(GLuint index, const GLuint* v){detail::VertexAttribI1uiv(index, v);}
inline void VertexAttribI2i(GLuint index, GLint x, GLint y){detail::VertexAttribI2i(index, x, y);}
inline void VertexAttribI2iv(GLuint index, const GLint* v){detail::VertexAttribI2iv(index, v);}
inline void VertexAttribI2ui(GLuint index, GLuint x, GLuint y){detail::VertexAttribI2ui(index, x, y);}
inline void VertexAttribI2uiv(GLuint index, const GLuint* v){detail::VertexAttribI2uiv(index, v);}
inline void VertexAttribI3i(GLuint index, GLint x, GLint y, GLint z){detail::VertexAttribI3i(index, x, y, z);}
inline void VertexAttribI3iv(GLuint index, const GLint* v){detail::VertexAttribI3iv(index, v);}
inline void VertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z){detail::VertexAttribI3ui(index, x, y, z);}
inline void VertexAttribI3uiv(GLuint index, const GLuint* v){detail::VertexAttribI3uiv(index, v);}
inline void VertexAttribI4bv(GLuint index, const GLbyte* v){detail::VertexAttribI4bv(index, v);}
inline void VertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w){detail::VertexAttribI4i(index, x, y, z, w);}
inline void VertexAttribI4iv(GLuint index, const GLint* v){detail::VertexAttribI4iv(index, v);}
inline void VertexAttribI4sv(GLuint index, const GLshort* v){detail::VertexAttribI4sv(index, v);}
inline void VertexAttribI4ubv(GLuint index, const GLubyte* v){detail::VertexAttribI4ubv(index, v);}
inline void VertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w){detail::VertexAttribI4ui(index, x, y, z, w);}
inline void VertexAttribI4uiv(GLuint index, const GLuint* v){detail::VertexAttribI4uiv(index, v);}
inline void VertexAttribI4usv(GLuint index, const GLushort* v){detail::VertexAttribI4usv(index, v);}
inline void VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer){detail::VertexAttribIPointer(index, size, type, stride, pointer);}

inline void CopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size){detail::CopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);}
inline void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount){detail::DrawArraysInstanced(mode, first, count, instancecount);}
inline void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount){detail::DrawElementsInstanced(mode, count, type, indices, instancecount);}
inline void GetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName){detail::GetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName);}
inline void GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params){detail::GetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);}
inline void GetActiveUniformName(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformName){detail::GetActiveUniformName(program, uniformIndex, bufSize, length, uniformName);}
inline void GetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params){detail::GetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params);}
inline GLuint GetUniformBlockIndex(GLuint program, const GLchar* uniformBlockName){return detail::GetUniformBlockIndex(program, uniformBlockName);}
inline void GetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar* const * uniformNames, GLuint* uniformIndices){detail::GetUniformIndices(program, uniformCount, uniformNames, uniformIndices);}
inline void PrimitiveRestartIndex(GLuint index){detail::PrimitiveRestartIndex(index);}
inline void TexBuffer(GLenum target, GLenum internalformat, GLuint buffer){detail::TexBuffer(target, internalformat, buffer);}
inline void UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding){detail::UniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);}

inline GLenum ClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout){return detail::ClientWaitSync(sync, flags, timeout);}
inline void DeleteSync(GLsync sync){detail::DeleteSync(sync);}
inline void DrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex){detail::DrawElementsBaseVertex(mode, count, type, indices, basevertex);}
inline void DrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLint basevertex){detail::DrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex);}
inline void DrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices, GLint basevertex){detail::DrawRangeElementsBaseVertex(mode, start, end, count, type, indices, basevertex);}
inline GLsync FenceSync(GLenum condition, GLbitfield flags){return detail::FenceSync(condition, flags);}
inline void FramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level){detail::FramebufferTexture(target, attachment, texture, level);}
inline void GetBufferParameteri64v(GLenum target, GLenum pname, GLint64* params){detail::GetBufferParameteri64v(target, pname, params);}
inline void GetInteger64i_v(GLenum target, GLuint index, GLint64* data){detail::GetInteger64i_v(target, index, data);}
inline void GetInteger64v(GLenum pname, GLint64* data){detail::GetInteger64v(pname, data);}
inline void GetMultisamplefv(GLenum pname, GLuint index, GLfloat* val){detail::GetMultisamplefv(pname, index, val);}
inline void GetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values){detail::GetSynciv(sync, pname, bufSize, length, values);}
inline GLboolean IsSync(GLsync sync){return detail::IsSync(sync);}
inline void MultiDrawElementsBaseVertex(GLenum mode, const GLsizei* count, GLenum type, const void* const * indices, GLsizei drawcount, const GLint* basevertex){detail::MultiDrawElementsBaseVertex(mode, count, type, indices, drawcount, basevertex);}
inline void ProvokingVertex(GLenum mode){detail::ProvokingVertex(mode);}
inline void SampleMaski(GLuint maskNumber, GLbitfield mask){detail::SampleMaski(maskNumber, mask);}
inline void TexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations){detail::TexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);}
inline void TexImage3DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations){detail::TexImage3DMultisample(target, samples, internalformat, width, height, depth, fixedsamplelocations);}
inline void WaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout){detail::WaitSync(sync, flags, timeout);}

inline void BindFragDataLocationIndexed(GLuint program, GLuint colorNumber, GLuint index, const GLchar* name){detail::BindFragDataLocationIndexed(program, colorNumber, index, name);}
inline void BindSampler(GLuint unit, GLuint sampler){detail::BindSampler(unit, sampler);}
inline void DeleteSamplers(GLsizei count, const GLuint* samplers){detail::DeleteSamplers(count, samplers);}
inline void GenSamplers(GLsizei count, GLuint* samplers){detail::GenSamplers(count, samplers);}
inline GLint GetFragDataIndex(GLuint program, const GLchar* name){return detail::GetFragDataIndex(program, name);}
inline void GetQueryObjecti64v(GLuint id, GLenum pname, GLint64* params){detail::GetQueryObjecti64v(id, pname, params);}
inline void GetQueryObjectui64v(GLuint id, GLenum pname, GLuint64* params){detail::GetQueryObjectui64v(id, pname, params);}
inline void GetSamplerParameterIiv(GLuint sampler, GLenum pname, GLint* params){detail::GetSamplerParameterIiv(sampler, pname, params);}
inline void GetSamplerParameterIuiv(GLuint sampler, GLenum pname, GLuint* params){detail::GetSamplerParameterIuiv(sampler, pname, params);}
inline void GetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat* params){detail::GetSamplerParameterfv(sampler, pname, params);}
inline void GetSamplerParameteriv(GLuint sampler, GLenum pname, GLint* params){detail::GetSamplerParameteriv(sampler, pname, params);}
inline GLboolean IsSampler(GLuint sampler){return detail::IsSampler(sampler);}
inline void QueryCounter(GLuint id, GLenum target){detail::QueryCounter(id, target);}
inline void SamplerParameterIiv(GLuint sampler, GLenum pname, const GLint* param){detail::SamplerParameterIiv(sampler, pname, param);}
inline void SamplerParameterIuiv(GLuint sampler, GLenum pname, const GLuint* param){detail::SamplerParameterIuiv(sampler, pname, param);}
inline void SamplerParameterf(GLuint sampler, GLenum pname, GLfloat param){detail::SamplerParameterf(sampler, pname, param);}
inline void SamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat* param){detail::SamplerParameterfv(sampler, pname, param);}
inline void SamplerParameteri(GLuint sampler, GLenum pname, GLint param){detail::SamplerParameteri(sampler, pname, param);}
inline void SamplerParameteriv(GLuint sampler, GLenum pname, const GLint* param){detail::SamplerParameteriv(sampler, pname, param);}
inline void VertexAttribDivisor(GLuint index, GLuint divisor){detail::VertexAttribDivisor(index, divisor);}
inline void VertexAttribP1ui(GLuint index, GLenum type, GLboolean normalized, GLuint value){detail::VertexAttribP1ui(index, type, normalized, value);}
inline void VertexAttribP1uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint* value){detail::VertexAttribP1uiv(index, type, normalized, value);}
inline void VertexAttribP2ui(GLuint index, GLenum type, GLboolean normalized, GLuint value){detail::VertexAttribP2ui(index, type, normalized, value);}
inline void VertexAttribP2uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint* value){detail::VertexAttribP2uiv(index, type, normalized, value);}
inline void VertexAttribP3ui(GLuint index, GLenum type, GLboolean normalized, GLuint value){detail::VertexAttribP3ui(index, type, normalized, value);}
inline void VertexAttribP3uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint* value){detail::VertexAttribP3uiv(index, type, normalized, value);}
inline void VertexAttribP4ui(GLuint index, GLenum type, GLboolean normalized, GLuint value){detail::VertexAttribP4ui(index, type, normalized, value);}
inline void VertexAttribP4uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint* value){detail::VertexAttribP4uiv(index, type, normalized, value);}

} // namespace gl

//============================================================================//

namespace gl::sys {

exts::LoadTest LoadFunctions();

int GetMinorVersion();
int GetMajorVersion();
bool IsVersionGEQ(int majorVersion, int minorVersion);

} // namespace gl::sys

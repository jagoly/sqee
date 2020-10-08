#include <sqee/gl/Loader.hpp>

#include <sqee/gl/Functions.hpp>

//============================================================================//

/*
static void* get_proc(const char *namez);

#ifdef SQEE_WINDOWS

#include <windows.h>
static HMODULE libGL;

typedef void* (APIENTRYP PFNWGLGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNWGLGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;

#ifdef _MSC_VER
#ifdef __has_include
  #if __has_include(<winapifamily.h>)
    #define HAVE_WINAPIFAMILY 1
  #endif
#elif _MSC_VER >= 1700 && !_USING_V110_SDK71_
  #define HAVE_WINAPIFAMILY 1
#endif
#endif

#ifdef HAVE_WINAPIFAMILY
  #include <winapifamily.h>
  #if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
    #define IS_UWP 1
  #endif
#endif

static
int open_gl(void) {
#ifndef IS_UWP
    libGL = LoadLibraryW(L"opengl32.dll"));
    if(libGL != NULL) {
        void (* tmp)(void);
        tmp = (void(*)(void)) GetProcAddress(libGL, "wglGetProcAddress"));
        gladGetProcAddressPtr = (PFNWGLGETPROCADDRESSPROC_PRIVATE) tmp;
        return gladGetProcAddressPtr != NULL;
    }
#endif

    return 0;
}

static
void close_gl(void) {
    if(libGL != NULL) {
        FreeLibrary((HMODULE) libGL);
        libGL = NULL;
    }
}
#else
#include <dlfcn.h>
static void* libGL;

#if !defined(__APPLE__) && !defined(__HAIKU__)
typedef void* (APIENTRYP PFNGLXGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNGLXGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;
#endif

static
int open_gl(void) {
#ifdef __APPLE__
    static const char *NAMES[] = {
        "../Frameworks/OpenGL.framework/OpenGL",
        "/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL"
    };
#else
    static const char *NAMES[] = {"libGL.so.1", "libGL.so"};
#endif

    unsigned int index = 0;
    for(index = 0; index < (sizeof(NAMES) / sizeof(NAMES[0])); index++) {
        libGL = dlopen(NAMES[index], RTLD_NOW | RTLD_GLOBAL);

        if(libGL != NULL) {
#if defined(__APPLE__) || defined(__HAIKU__)
            return 1;
#else
            gladGetProcAddressPtr = (PFNGLXGETPROCADDRESSPROC_PRIVATE)dlsym(libGL,
                "glXGetProcAddressARB"));
            return gladGetProcAddressPtr != NULL;
#endif
        }
    }

    return 0;
}

static
void close_gl(void) {
    if(libGL != NULL) {
        dlclose(libGL);
        libGL = NULL;
    }
}
#endif

static
void* get_proc(const char *namez) {
    void* result = NULL;
    if(libGL == NULL) return NULL;

#if !defined(__APPLE__) && !defined(__HAIKU__)
    if(gladGetProcAddressPtr != NULL) {
        result = gladGetProcAddressPtr(namez);
    }
#endif
    if(result == NULL) {
#if defined(_WIN32) || defined(__CYGWIN__)
        result = (void*)GetProcAddress((HMODULE) libGL, namez);
#else
        result = dlsym(libGL, namez);
#endif
    }

    return result;
}

int gladLoadGL(void) {
    int status = 0;

    if(open_gl()) {
        status = gladLoadGLLoader(&get_proc);
        close_gl();
    }

    return status;
}

struct gladGLversionStruct GLVersion = { 0, 0 };

#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
#define _GLAD_IS_SOME_NEW_VERSION 1
#endif

static int max_loaded_major;
static int max_loaded_minor;

static const char *exts = NULL;
static int num_exts_i = 0;
static char **exts_i = NULL;

static int get_exts(void) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        exts = (const char *)glGetString(GL_EXTENSIONS);
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        unsigned int index;

        num_exts_i = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts_i);
        if (num_exts_i > 0) {
            exts_i = (char **)malloc((size_t)num_exts_i * (sizeof *exts_i));
        }

        if (exts_i == NULL) {
            return 0;
        }

        for(index = 0; index < (unsigned)num_exts_i; index++) {
            const char *gl_str_tmp = (const char*)glGetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp);

            char *local_str = (char*)malloc((len+1) * sizeof(char));
            if(local_str != NULL) {
                memcpy(local_str, gl_str_tmp, (len+1) * sizeof(char));
            }
            exts_i[index] = local_str;
        }
    }
#endif
    return 1;
}

static void free_exts(void) {
    if (exts_i != NULL) {
        int index;
        for(index = 0; index < num_exts_i; index++) {
            free((char *)exts_i[index]);
        }
        free((void *)exts_i);
        exts_i = NULL;
    }
}

static int has_ext(const char *ext) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = exts;
        if(extensions == NULL || ext == NULL) {
            return 0;
        }

        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }

            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
                (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        int index;
        if(exts_i == NULL) return 0;
        for(index = 0; index < num_exts_i; index++) {
            const char *e = exts_i[index];

            if(exts_i[index] != NULL && strcmp(e, ext) == 0) {
                return 1;
            }
        }
    }
#endif

    return 0;
}*/

/*
int glad_GL_VERSION_1_0 = reinterpret_cast<decltype(glad_GL_VERSION_1_0)>(load("GL_VERSION_1_0"));
int glad_GL_VERSION_1_1 = reinterpret_cast<decltype(glad_GL_VERSION_1_1)>(load("GL_VERSION_1_1"));
int glad_GL_VERSION_1_2 = reinterpret_cast<decltype(glad_GL_VERSION_1_2)>(load("GL_VERSION_1_2"));
int glad_GL_VERSION_1_3 = reinterpret_cast<decltype(glad_GL_VERSION_1_3)>(load("GL_VERSION_1_3"));
int glad_GL_VERSION_1_4 = reinterpret_cast<decltype(glad_GL_VERSION_1_4)>(load("GL_VERSION_1_4"));
int glad_GL_VERSION_1_5 = reinterpret_cast<decltype(glad_GL_VERSION_1_5)>(load("GL_VERSION_1_5"));
int glad_GL_VERSION_2_0 = reinterpret_cast<decltype(glad_GL_VERSION_2_0)>(load("GL_VERSION_2_0"));
int glad_GL_VERSION_2_1 = reinterpret_cast<decltype(glad_GL_VERSION_2_1)>(load("GL_VERSION_2_1"));
int glad_GL_VERSION_3_0 = reinterpret_cast<decltype(glad_GL_VERSION_3_0)>(load("GL_VERSION_3_0"));
int glad_GL_VERSION_3_1 = reinterpret_cast<decltype(glad_GL_VERSION_3_1)>(load("GL_VERSION_3_1"));
int glad_GL_VERSION_3_2 = reinterpret_cast<decltype(glad_GL_VERSION_3_2)>(load("GL_VERSION_3_2"));
int glad_GL_VERSION_3_3 = reinterpret_cast<decltype(glad_GL_VERSION_3_3)>(load("GL_VERSION_3_3"));
int glad_GL_VERSION_4_0 = reinterpret_cast<decltype(glad_GL_VERSION_4_0)>(load("GL_VERSION_4_0"));
int glad_GL_VERSION_4_1 = reinterpret_cast<decltype(glad_GL_VERSION_4_1)>(load("GL_VERSION_4_1"));
int glad_GL_VERSION_4_2 = reinterpret_cast<decltype(glad_GL_VERSION_4_2)>(load("GL_VERSION_4_2"));
int glad_GL_VERSION_4_3 = reinterpret_cast<decltype(glad_GL_VERSION_4_3)>(load("GL_VERSION_4_3"));
int glad_GL_VERSION_4_4 = reinterpret_cast<decltype(glad_GL_VERSION_4_4)>(load("GL_VERSION_4_4"));
int glad_GL_VERSION_4_5 = reinterpret_cast<decltype(glad_GL_VERSION_4_5)>(load("GL_VERSION_4_5"));*/

namespace sq::gl::detail {

decltype(glActiveShaderProgram) glActiveShaderProgram = nullptr;
decltype(glActiveTexture) glActiveTexture = nullptr;
decltype(glAttachShader) glAttachShader = nullptr;
decltype(glBeginConditionalRender) glBeginConditionalRender = nullptr;
decltype(glBeginQuery) glBeginQuery = nullptr;
decltype(glBeginQueryIndexed) glBeginQueryIndexed = nullptr;
decltype(glBeginTransformFeedback) glBeginTransformFeedback = nullptr;
decltype(glBindAttribLocation) glBindAttribLocation = nullptr;
decltype(glBindBuffer) glBindBuffer = nullptr;
decltype(glBindBufferBase) glBindBufferBase = nullptr;
decltype(glBindBufferRange) glBindBufferRange = nullptr;
decltype(glBindBuffersBase) glBindBuffersBase = nullptr;
decltype(glBindBuffersRange) glBindBuffersRange = nullptr;
decltype(glBindFragDataLocation) glBindFragDataLocation = nullptr;
decltype(glBindFragDataLocationIndexed) glBindFragDataLocationIndexed = nullptr;
decltype(glBindFramebuffer) glBindFramebuffer = nullptr;
decltype(glBindImageTexture) glBindImageTexture = nullptr;
decltype(glBindImageTextures) glBindImageTextures = nullptr;
decltype(glBindProgramPipeline) glBindProgramPipeline = nullptr;
decltype(glBindRenderbuffer) glBindRenderbuffer = nullptr;
decltype(glBindSampler) glBindSampler = nullptr;
decltype(glBindSamplers) glBindSamplers = nullptr;
decltype(glBindTexture) glBindTexture = nullptr;
decltype(glBindTextureUnit) glBindTextureUnit = nullptr;
decltype(glBindTextures) glBindTextures = nullptr;
decltype(glBindTransformFeedback) glBindTransformFeedback = nullptr;
decltype(glBindVertexArray) glBindVertexArray = nullptr;
decltype(glBindVertexBuffer) glBindVertexBuffer = nullptr;
decltype(glBindVertexBuffers) glBindVertexBuffers = nullptr;
decltype(glBlendColor) glBlendColor = nullptr;
decltype(glBlendEquation) glBlendEquation = nullptr;
decltype(glBlendEquationSeparate) glBlendEquationSeparate = nullptr;
decltype(glBlendEquationSeparatei) glBlendEquationSeparatei = nullptr;
decltype(glBlendEquationi) glBlendEquationi = nullptr;
decltype(glBlendFunc) glBlendFunc = nullptr;
decltype(glBlendFuncSeparate) glBlendFuncSeparate = nullptr;
decltype(glBlendFuncSeparatei) glBlendFuncSeparatei = nullptr;
decltype(glBlendFunci) glBlendFunci = nullptr;
decltype(glBlitFramebuffer) glBlitFramebuffer = nullptr;
decltype(glBlitNamedFramebuffer) glBlitNamedFramebuffer = nullptr;
decltype(glBufferData) glBufferData = nullptr;
decltype(glBufferStorage) glBufferStorage = nullptr;
decltype(glBufferSubData) glBufferSubData = nullptr;
decltype(glCheckFramebufferStatus) glCheckFramebufferStatus = nullptr;
decltype(glCheckNamedFramebufferStatus) glCheckNamedFramebufferStatus = nullptr;
decltype(glClampColor) glClampColor = nullptr;
decltype(glClear) glClear = nullptr;
decltype(glClearBufferData) glClearBufferData = nullptr;
decltype(glClearBufferSubData) glClearBufferSubData = nullptr;
decltype(glClearBufferfi) glClearBufferfi = nullptr;
decltype(glClearBufferfv) glClearBufferfv = nullptr;
decltype(glClearBufferiv) glClearBufferiv = nullptr;
decltype(glClearBufferuiv) glClearBufferuiv = nullptr;
decltype(glClearColor) glClearColor = nullptr;
decltype(glClearDepth) glClearDepth = nullptr;
decltype(glClearDepthf) glClearDepthf = nullptr;
decltype(glClearNamedBufferData) glClearNamedBufferData = nullptr;
decltype(glClearNamedBufferSubData) glClearNamedBufferSubData = nullptr;
decltype(glClearNamedFramebufferfi) glClearNamedFramebufferfi = nullptr;
decltype(glClearNamedFramebufferfv) glClearNamedFramebufferfv = nullptr;
decltype(glClearNamedFramebufferiv) glClearNamedFramebufferiv = nullptr;
decltype(glClearNamedFramebufferuiv) glClearNamedFramebufferuiv = nullptr;
decltype(glClearStencil) glClearStencil = nullptr;
decltype(glClearTexImage) glClearTexImage = nullptr;
decltype(glClearTexSubImage) glClearTexSubImage = nullptr;
decltype(glClientWaitSync) glClientWaitSync = nullptr;
decltype(glClipControl) glClipControl = nullptr;
decltype(glColorMask) glColorMask = nullptr;
decltype(glColorMaski) glColorMaski = nullptr;
decltype(glColorP3ui) glColorP3ui = nullptr;
decltype(glColorP3uiv) glColorP3uiv = nullptr;
decltype(glColorP4ui) glColorP4ui = nullptr;
decltype(glColorP4uiv) glColorP4uiv = nullptr;
decltype(glCompileShader) glCompileShader = nullptr;
decltype(glCompressedTexImage1D) glCompressedTexImage1D = nullptr;
decltype(glCompressedTexImage2D) glCompressedTexImage2D = nullptr;
decltype(glCompressedTexImage3D) glCompressedTexImage3D = nullptr;
decltype(glCompressedTexSubImage1D) glCompressedTexSubImage1D = nullptr;
decltype(glCompressedTexSubImage2D) glCompressedTexSubImage2D = nullptr;
decltype(glCompressedTexSubImage3D) glCompressedTexSubImage3D = nullptr;
decltype(glCompressedTextureSubImage1D) glCompressedTextureSubImage1D = nullptr;
decltype(glCompressedTextureSubImage2D) glCompressedTextureSubImage2D = nullptr;
decltype(glCompressedTextureSubImage3D) glCompressedTextureSubImage3D = nullptr;
decltype(glCopyBufferSubData) glCopyBufferSubData = nullptr;
decltype(glCopyImageSubData) glCopyImageSubData = nullptr;
decltype(glCopyNamedBufferSubData) glCopyNamedBufferSubData = nullptr;
decltype(glCopyTexImage1D) glCopyTexImage1D = nullptr;
decltype(glCopyTexImage2D) glCopyTexImage2D = nullptr;
decltype(glCopyTexSubImage1D) glCopyTexSubImage1D = nullptr;
decltype(glCopyTexSubImage2D) glCopyTexSubImage2D = nullptr;
decltype(glCopyTexSubImage3D) glCopyTexSubImage3D = nullptr;
decltype(glCopyTextureSubImage1D) glCopyTextureSubImage1D = nullptr;
decltype(glCopyTextureSubImage2D) glCopyTextureSubImage2D = nullptr;
decltype(glCopyTextureSubImage3D) glCopyTextureSubImage3D = nullptr;
decltype(glCreateBuffers) glCreateBuffers = nullptr;
decltype(glCreateFramebuffers) glCreateFramebuffers = nullptr;
decltype(glCreateProgram) glCreateProgram = nullptr;
decltype(glCreateProgramPipelines) glCreateProgramPipelines = nullptr;
decltype(glCreateQueries) glCreateQueries = nullptr;
decltype(glCreateRenderbuffers) glCreateRenderbuffers = nullptr;
decltype(glCreateSamplers) glCreateSamplers = nullptr;
decltype(glCreateShader) glCreateShader = nullptr;
decltype(glCreateShaderProgramv) glCreateShaderProgramv = nullptr;
decltype(glCreateTextures) glCreateTextures = nullptr;
decltype(glCreateTransformFeedbacks) glCreateTransformFeedbacks = nullptr;
decltype(glCreateVertexArrays) glCreateVertexArrays = nullptr;
decltype(glCullFace) glCullFace = nullptr;
decltype(glDebugMessageCallback) glDebugMessageCallback = nullptr;
decltype(glDebugMessageControl) glDebugMessageControl = nullptr;
decltype(glDebugMessageInsert) glDebugMessageInsert = nullptr;
decltype(glDeleteBuffers) glDeleteBuffers = nullptr;
decltype(glDeleteFramebuffers) glDeleteFramebuffers = nullptr;
decltype(glDeleteProgram) glDeleteProgram = nullptr;
decltype(glDeleteProgramPipelines) glDeleteProgramPipelines = nullptr;
decltype(glDeleteQueries) glDeleteQueries = nullptr;
decltype(glDeleteRenderbuffers) glDeleteRenderbuffers = nullptr;
decltype(glDeleteSamplers) glDeleteSamplers = nullptr;
decltype(glDeleteShader) glDeleteShader = nullptr;
decltype(glDeleteSync) glDeleteSync = nullptr;
decltype(glDeleteTextures) glDeleteTextures = nullptr;
decltype(glDeleteTransformFeedbacks) glDeleteTransformFeedbacks = nullptr;
decltype(glDeleteVertexArrays) glDeleteVertexArrays = nullptr;
decltype(glDepthFunc) glDepthFunc = nullptr;
decltype(glDepthMask) glDepthMask = nullptr;
decltype(glDepthRange) glDepthRange = nullptr;
decltype(glDepthRangeArrayv) glDepthRangeArrayv = nullptr;
decltype(glDepthRangeIndexed) glDepthRangeIndexed = nullptr;
decltype(glDepthRangef) glDepthRangef = nullptr;
decltype(glDetachShader) glDetachShader = nullptr;
decltype(glDisable) glDisable = nullptr;
decltype(glDisableVertexArrayAttrib) glDisableVertexArrayAttrib = nullptr;
decltype(glDisableVertexAttribArray) glDisableVertexAttribArray = nullptr;
decltype(glDisablei) glDisablei = nullptr;
decltype(glDispatchCompute) glDispatchCompute = nullptr;
decltype(glDispatchComputeIndirect) glDispatchComputeIndirect = nullptr;
decltype(glDrawArrays) glDrawArrays = nullptr;
decltype(glDrawArraysIndirect) glDrawArraysIndirect = nullptr;
decltype(glDrawArraysInstanced) glDrawArraysInstanced = nullptr;
decltype(glDrawArraysInstancedBaseInstance) glDrawArraysInstancedBaseInstance = nullptr;
decltype(glDrawBuffer) glDrawBuffer = nullptr;
decltype(glDrawBuffers) glDrawBuffers = nullptr;
decltype(glDrawElements) glDrawElements = nullptr;
decltype(glDrawElementsBaseVertex) glDrawElementsBaseVertex = nullptr;
decltype(glDrawElementsIndirect) glDrawElementsIndirect = nullptr;
decltype(glDrawElementsInstanced) glDrawElementsInstanced = nullptr;
decltype(glDrawElementsInstancedBaseInstance) glDrawElementsInstancedBaseInstance = nullptr;
decltype(glDrawElementsInstancedBaseVertex) glDrawElementsInstancedBaseVertex = nullptr;
decltype(glDrawElementsInstancedBaseVertexBaseInstance) glDrawElementsInstancedBaseVertexBaseInstance = nullptr;
decltype(glDrawRangeElements) glDrawRangeElements = nullptr;
decltype(glDrawRangeElementsBaseVertex) glDrawRangeElementsBaseVertex = nullptr;
decltype(glDrawTransformFeedback) glDrawTransformFeedback = nullptr;
decltype(glDrawTransformFeedbackInstanced) glDrawTransformFeedbackInstanced = nullptr;
decltype(glDrawTransformFeedbackStream) glDrawTransformFeedbackStream = nullptr;
decltype(glDrawTransformFeedbackStreamInstanced) glDrawTransformFeedbackStreamInstanced = nullptr;
decltype(glEnable) glEnable = nullptr;
decltype(glEnableVertexArrayAttrib) glEnableVertexArrayAttrib = nullptr;
decltype(glEnableVertexAttribArray) glEnableVertexAttribArray = nullptr;
decltype(glEnablei) glEnablei = nullptr;
decltype(glEndConditionalRender) glEndConditionalRender = nullptr;
decltype(glEndQuery) glEndQuery = nullptr;
decltype(glEndQueryIndexed) glEndQueryIndexed = nullptr;
decltype(glEndTransformFeedback) glEndTransformFeedback = nullptr;
decltype(glFenceSync) glFenceSync = nullptr;
decltype(glFinish) glFinish = nullptr;
decltype(glFlush) glFlush = nullptr;
decltype(glFlushMappedBufferRange) glFlushMappedBufferRange = nullptr;
decltype(glFlushMappedNamedBufferRange) glFlushMappedNamedBufferRange = nullptr;
decltype(glFramebufferParameteri) glFramebufferParameteri = nullptr;
decltype(glFramebufferRenderbuffer) glFramebufferRenderbuffer = nullptr;
decltype(glFramebufferTexture) glFramebufferTexture = nullptr;
decltype(glFramebufferTexture1D) glFramebufferTexture1D = nullptr;
decltype(glFramebufferTexture2D) glFramebufferTexture2D = nullptr;
decltype(glFramebufferTexture3D) glFramebufferTexture3D = nullptr;
decltype(glFramebufferTextureLayer) glFramebufferTextureLayer = nullptr;
decltype(glFrontFace) glFrontFace = nullptr;
decltype(glGenBuffers) glGenBuffers = nullptr;
decltype(glGenFramebuffers) glGenFramebuffers = nullptr;
decltype(glGenProgramPipelines) glGenProgramPipelines = nullptr;
decltype(glGenQueries) glGenQueries = nullptr;
decltype(glGenRenderbuffers) glGenRenderbuffers = nullptr;
decltype(glGenSamplers) glGenSamplers = nullptr;
decltype(glGenTextures) glGenTextures = nullptr;
decltype(glGenTransformFeedbacks) glGenTransformFeedbacks = nullptr;
decltype(glGenVertexArrays) glGenVertexArrays = nullptr;
decltype(glGenerateMipmap) glGenerateMipmap = nullptr;
decltype(glGenerateTextureMipmap) glGenerateTextureMipmap = nullptr;
decltype(glGetActiveAtomicCounterBufferiv) glGetActiveAtomicCounterBufferiv = nullptr;
decltype(glGetActiveAttrib) glGetActiveAttrib = nullptr;
decltype(glGetActiveSubroutineName) glGetActiveSubroutineName = nullptr;
decltype(glGetActiveSubroutineUniformName) glGetActiveSubroutineUniformName = nullptr;
decltype(glGetActiveSubroutineUniformiv) glGetActiveSubroutineUniformiv = nullptr;
decltype(glGetActiveUniform) glGetActiveUniform = nullptr;
decltype(glGetActiveUniformBlockName) glGetActiveUniformBlockName = nullptr;
decltype(glGetActiveUniformBlockiv) glGetActiveUniformBlockiv = nullptr;
decltype(glGetActiveUniformName) glGetActiveUniformName = nullptr;
decltype(glGetActiveUniformsiv) glGetActiveUniformsiv = nullptr;
decltype(glGetAttachedShaders) glGetAttachedShaders = nullptr;
decltype(glGetAttribLocation) glGetAttribLocation = nullptr;
decltype(glGetBooleani_v) glGetBooleani_v = nullptr;
decltype(glGetBooleanv) glGetBooleanv = nullptr;
decltype(glGetBufferParameteri64v) glGetBufferParameteri64v = nullptr;
decltype(glGetBufferParameteriv) glGetBufferParameteriv = nullptr;
decltype(glGetBufferPointerv) glGetBufferPointerv = nullptr;
decltype(glGetBufferSubData) glGetBufferSubData = nullptr;
decltype(glGetCompressedTexImage) glGetCompressedTexImage = nullptr;
decltype(glGetCompressedTextureImage) glGetCompressedTextureImage = nullptr;
decltype(glGetCompressedTextureSubImage) glGetCompressedTextureSubImage = nullptr;
decltype(glGetDebugMessageLog) glGetDebugMessageLog = nullptr;
decltype(glGetDoublei_v) glGetDoublei_v = nullptr;
decltype(glGetDoublev) glGetDoublev = nullptr;
decltype(glGetError) glGetError = nullptr;
decltype(glGetFloati_v) glGetFloati_v = nullptr;
decltype(glGetFloatv) glGetFloatv = nullptr;
decltype(glGetFragDataIndex) glGetFragDataIndex = nullptr;
decltype(glGetFragDataLocation) glGetFragDataLocation = nullptr;
decltype(glGetFramebufferAttachmentParameteriv) glGetFramebufferAttachmentParameteriv = nullptr;
decltype(glGetFramebufferParameteriv) glGetFramebufferParameteriv = nullptr;
decltype(glGetGraphicsResetStatus) glGetGraphicsResetStatus = nullptr;
decltype(glGetInteger64i_v) glGetInteger64i_v = nullptr;
decltype(glGetInteger64v) glGetInteger64v = nullptr;
decltype(glGetIntegeri_v) glGetIntegeri_v = nullptr;
decltype(glGetIntegerv) glGetIntegerv = nullptr;
decltype(glGetInternalformati64v) glGetInternalformati64v = nullptr;
decltype(glGetInternalformativ) glGetInternalformativ = nullptr;
decltype(glGetMultisamplefv) glGetMultisamplefv = nullptr;
decltype(glGetNamedBufferParameteri64v) glGetNamedBufferParameteri64v = nullptr;
decltype(glGetNamedBufferParameteriv) glGetNamedBufferParameteriv = nullptr;
decltype(glGetNamedBufferPointerv) glGetNamedBufferPointerv = nullptr;
decltype(glGetNamedBufferSubData) glGetNamedBufferSubData = nullptr;
decltype(glGetNamedFramebufferAttachmentParameteriv) glGetNamedFramebufferAttachmentParameteriv = nullptr;
decltype(glGetNamedFramebufferParameteriv) glGetNamedFramebufferParameteriv = nullptr;
decltype(glGetNamedRenderbufferParameteriv) glGetNamedRenderbufferParameteriv = nullptr;
decltype(glGetObjectLabel) glGetObjectLabel = nullptr;
decltype(glGetObjectPtrLabel) glGetObjectPtrLabel = nullptr;
decltype(glGetPointerv) glGetPointerv = nullptr;
decltype(glGetProgramBinary) glGetProgramBinary = nullptr;
decltype(glGetProgramInfoLog) glGetProgramInfoLog = nullptr;
decltype(glGetProgramInterfaceiv) glGetProgramInterfaceiv = nullptr;
decltype(glGetProgramPipelineInfoLog) glGetProgramPipelineInfoLog = nullptr;
decltype(glGetProgramPipelineiv) glGetProgramPipelineiv = nullptr;
decltype(glGetProgramResourceIndex) glGetProgramResourceIndex = nullptr;
decltype(glGetProgramResourceLocation) glGetProgramResourceLocation = nullptr;
decltype(glGetProgramResourceLocationIndex) glGetProgramResourceLocationIndex = nullptr;
decltype(glGetProgramResourceName) glGetProgramResourceName = nullptr;
decltype(glGetProgramResourceiv) glGetProgramResourceiv = nullptr;
decltype(glGetProgramStageiv) glGetProgramStageiv = nullptr;
decltype(glGetProgramiv) glGetProgramiv = nullptr;
decltype(glGetQueryBufferObjecti64v) glGetQueryBufferObjecti64v = nullptr;
decltype(glGetQueryBufferObjectiv) glGetQueryBufferObjectiv = nullptr;
decltype(glGetQueryBufferObjectui64v) glGetQueryBufferObjectui64v = nullptr;
decltype(glGetQueryBufferObjectuiv) glGetQueryBufferObjectuiv = nullptr;
decltype(glGetQueryIndexediv) glGetQueryIndexediv = nullptr;
decltype(glGetQueryObjecti64v) glGetQueryObjecti64v = nullptr;
decltype(glGetQueryObjectiv) glGetQueryObjectiv = nullptr;
decltype(glGetQueryObjectui64v) glGetQueryObjectui64v = nullptr;
decltype(glGetQueryObjectuiv) glGetQueryObjectuiv = nullptr;
decltype(glGetQueryiv) glGetQueryiv = nullptr;
decltype(glGetRenderbufferParameteriv) glGetRenderbufferParameteriv = nullptr;
decltype(glGetSamplerParameterIiv) glGetSamplerParameterIiv = nullptr;
decltype(glGetSamplerParameterIuiv) glGetSamplerParameterIuiv = nullptr;
decltype(glGetSamplerParameterfv) glGetSamplerParameterfv = nullptr;
decltype(glGetSamplerParameteriv) glGetSamplerParameteriv = nullptr;
decltype(glGetShaderInfoLog) glGetShaderInfoLog = nullptr;
decltype(glGetShaderPrecisionFormat) glGetShaderPrecisionFormat = nullptr;
decltype(glGetShaderSource) glGetShaderSource = nullptr;
decltype(glGetShaderiv) glGetShaderiv = nullptr;
decltype(glGetString) glGetString = nullptr;
decltype(glGetStringi) glGetStringi = nullptr;
decltype(glGetSubroutineIndex) glGetSubroutineIndex = nullptr;
decltype(glGetSubroutineUniformLocation) glGetSubroutineUniformLocation = nullptr;
decltype(glGetSynciv) glGetSynciv = nullptr;
decltype(glGetTexImage) glGetTexImage = nullptr;
decltype(glGetTexLevelParameterfv) glGetTexLevelParameterfv = nullptr;
decltype(glGetTexLevelParameteriv) glGetTexLevelParameteriv = nullptr;
decltype(glGetTexParameterIiv) glGetTexParameterIiv = nullptr;
decltype(glGetTexParameterIuiv) glGetTexParameterIuiv = nullptr;
decltype(glGetTexParameterfv) glGetTexParameterfv = nullptr;
decltype(glGetTexParameteriv) glGetTexParameteriv = nullptr;
decltype(glGetTextureImage) glGetTextureImage = nullptr;
decltype(glGetTextureLevelParameterfv) glGetTextureLevelParameterfv = nullptr;
decltype(glGetTextureLevelParameteriv) glGetTextureLevelParameteriv = nullptr;
decltype(glGetTextureParameterIiv) glGetTextureParameterIiv = nullptr;
decltype(glGetTextureParameterIuiv) glGetTextureParameterIuiv = nullptr;
decltype(glGetTextureParameterfv) glGetTextureParameterfv = nullptr;
decltype(glGetTextureParameteriv) glGetTextureParameteriv = nullptr;
decltype(glGetTextureSubImage) glGetTextureSubImage = nullptr;
decltype(glGetTransformFeedbackVarying) glGetTransformFeedbackVarying = nullptr;
decltype(glGetTransformFeedbacki64_v) glGetTransformFeedbacki64_v = nullptr;
decltype(glGetTransformFeedbacki_v) glGetTransformFeedbacki_v = nullptr;
decltype(glGetTransformFeedbackiv) glGetTransformFeedbackiv = nullptr;
decltype(glGetUniformBlockIndex) glGetUniformBlockIndex = nullptr;
decltype(glGetUniformIndices) glGetUniformIndices = nullptr;
decltype(glGetUniformLocation) glGetUniformLocation = nullptr;
decltype(glGetUniformSubroutineuiv) glGetUniformSubroutineuiv = nullptr;
decltype(glGetUniformdv) glGetUniformdv = nullptr;
decltype(glGetUniformfv) glGetUniformfv = nullptr;
decltype(glGetUniformiv) glGetUniformiv = nullptr;
decltype(glGetUniformuiv) glGetUniformuiv = nullptr;
decltype(glGetVertexArrayIndexed64iv) glGetVertexArrayIndexed64iv = nullptr;
decltype(glGetVertexArrayIndexediv) glGetVertexArrayIndexediv = nullptr;
decltype(glGetVertexArrayiv) glGetVertexArrayiv = nullptr;
decltype(glGetVertexAttribIiv) glGetVertexAttribIiv = nullptr;
decltype(glGetVertexAttribIuiv) glGetVertexAttribIuiv = nullptr;
decltype(glGetVertexAttribLdv) glGetVertexAttribLdv = nullptr;
decltype(glGetVertexAttribPointerv) glGetVertexAttribPointerv = nullptr;
decltype(glGetVertexAttribdv) glGetVertexAttribdv = nullptr;
decltype(glGetVertexAttribfv) glGetVertexAttribfv = nullptr;
decltype(glGetVertexAttribiv) glGetVertexAttribiv = nullptr;
decltype(glGetnColorTable) glGetnColorTable = nullptr;
decltype(glGetnCompressedTexImage) glGetnCompressedTexImage = nullptr;
decltype(glGetnConvolutionFilter) glGetnConvolutionFilter = nullptr;
decltype(glGetnHistogram) glGetnHistogram = nullptr;
decltype(glGetnMapdv) glGetnMapdv = nullptr;
decltype(glGetnMapfv) glGetnMapfv = nullptr;
decltype(glGetnMapiv) glGetnMapiv = nullptr;
decltype(glGetnMinmax) glGetnMinmax = nullptr;
decltype(glGetnPixelMapfv) glGetnPixelMapfv = nullptr;
decltype(glGetnPixelMapuiv) glGetnPixelMapuiv = nullptr;
decltype(glGetnPixelMapusv) glGetnPixelMapusv = nullptr;
decltype(glGetnPolygonStipple) glGetnPolygonStipple = nullptr;
decltype(glGetnSeparableFilter) glGetnSeparableFilter = nullptr;
decltype(glGetnTexImage) glGetnTexImage = nullptr;
decltype(glGetnUniformdv) glGetnUniformdv = nullptr;
decltype(glGetnUniformfv) glGetnUniformfv = nullptr;
decltype(glGetnUniformiv) glGetnUniformiv = nullptr;
decltype(glGetnUniformuiv) glGetnUniformuiv = nullptr;
decltype(glHint) glHint = nullptr;
decltype(glInvalidateBufferData) glInvalidateBufferData = nullptr;
decltype(glInvalidateBufferSubData) glInvalidateBufferSubData = nullptr;
decltype(glInvalidateFramebuffer) glInvalidateFramebuffer = nullptr;
decltype(glInvalidateNamedFramebufferData) glInvalidateNamedFramebufferData = nullptr;
decltype(glInvalidateNamedFramebufferSubData) glInvalidateNamedFramebufferSubData = nullptr;
decltype(glInvalidateSubFramebuffer) glInvalidateSubFramebuffer = nullptr;
decltype(glInvalidateTexImage) glInvalidateTexImage = nullptr;
decltype(glInvalidateTexSubImage) glInvalidateTexSubImage = nullptr;
decltype(glIsBuffer) glIsBuffer = nullptr;
decltype(glIsEnabled) glIsEnabled = nullptr;
decltype(glIsEnabledi) glIsEnabledi = nullptr;
decltype(glIsFramebuffer) glIsFramebuffer = nullptr;
decltype(glIsProgram) glIsProgram = nullptr;
decltype(glIsProgramPipeline) glIsProgramPipeline = nullptr;
decltype(glIsQuery) glIsQuery = nullptr;
decltype(glIsRenderbuffer) glIsRenderbuffer = nullptr;
decltype(glIsSampler) glIsSampler = nullptr;
decltype(glIsShader) glIsShader = nullptr;
decltype(glIsSync) glIsSync = nullptr;
decltype(glIsTexture) glIsTexture = nullptr;
decltype(glIsTransformFeedback) glIsTransformFeedback = nullptr;
decltype(glIsVertexArray) glIsVertexArray = nullptr;
decltype(glLineWidth) glLineWidth = nullptr;
decltype(glLinkProgram) glLinkProgram = nullptr;
decltype(glLogicOp) glLogicOp = nullptr;
decltype(glMapBuffer) glMapBuffer = nullptr;
decltype(glMapBufferRange) glMapBufferRange = nullptr;
decltype(glMapNamedBuffer) glMapNamedBuffer = nullptr;
decltype(glMapNamedBufferRange) glMapNamedBufferRange = nullptr;
decltype(glMemoryBarrier) glMemoryBarrier = nullptr;
decltype(glMemoryBarrierByRegion) glMemoryBarrierByRegion = nullptr;
decltype(glMinSampleShading) glMinSampleShading = nullptr;
decltype(glMultiDrawArrays) glMultiDrawArrays = nullptr;
decltype(glMultiDrawArraysIndirect) glMultiDrawArraysIndirect = nullptr;
decltype(glMultiDrawElements) glMultiDrawElements = nullptr;
decltype(glMultiDrawElementsBaseVertex) glMultiDrawElementsBaseVertex = nullptr;
decltype(glMultiDrawElementsIndirect) glMultiDrawElementsIndirect = nullptr;
decltype(glMultiTexCoordP1ui) glMultiTexCoordP1ui = nullptr;
decltype(glMultiTexCoordP1uiv) glMultiTexCoordP1uiv = nullptr;
decltype(glMultiTexCoordP2ui) glMultiTexCoordP2ui = nullptr;
decltype(glMultiTexCoordP2uiv) glMultiTexCoordP2uiv = nullptr;
decltype(glMultiTexCoordP3ui) glMultiTexCoordP3ui = nullptr;
decltype(glMultiTexCoordP3uiv) glMultiTexCoordP3uiv = nullptr;
decltype(glMultiTexCoordP4ui) glMultiTexCoordP4ui = nullptr;
decltype(glMultiTexCoordP4uiv) glMultiTexCoordP4uiv = nullptr;
decltype(glNamedBufferData) glNamedBufferData = nullptr;
decltype(glNamedBufferStorage) glNamedBufferStorage = nullptr;
decltype(glNamedBufferSubData) glNamedBufferSubData = nullptr;
decltype(glNamedFramebufferDrawBuffer) glNamedFramebufferDrawBuffer = nullptr;
decltype(glNamedFramebufferDrawBuffers) glNamedFramebufferDrawBuffers = nullptr;
decltype(glNamedFramebufferParameteri) glNamedFramebufferParameteri = nullptr;
decltype(glNamedFramebufferReadBuffer) glNamedFramebufferReadBuffer = nullptr;
decltype(glNamedFramebufferRenderbuffer) glNamedFramebufferRenderbuffer = nullptr;
decltype(glNamedFramebufferTexture) glNamedFramebufferTexture = nullptr;
decltype(glNamedFramebufferTextureLayer) glNamedFramebufferTextureLayer = nullptr;
decltype(glNamedRenderbufferStorage) glNamedRenderbufferStorage = nullptr;
decltype(glNamedRenderbufferStorageMultisample) glNamedRenderbufferStorageMultisample = nullptr;
decltype(glNormalP3ui) glNormalP3ui = nullptr;
decltype(glNormalP3uiv) glNormalP3uiv = nullptr;
decltype(glObjectLabel) glObjectLabel = nullptr;
decltype(glObjectPtrLabel) glObjectPtrLabel = nullptr;
decltype(glPatchParameterfv) glPatchParameterfv = nullptr;
decltype(glPatchParameteri) glPatchParameteri = nullptr;
decltype(glPauseTransformFeedback) glPauseTransformFeedback = nullptr;
decltype(glPixelStoref) glPixelStoref = nullptr;
decltype(glPixelStorei) glPixelStorei = nullptr;
decltype(glPointParameterf) glPointParameterf = nullptr;
decltype(glPointParameterfv) glPointParameterfv = nullptr;
decltype(glPointParameteri) glPointParameteri = nullptr;
decltype(glPointParameteriv) glPointParameteriv = nullptr;
decltype(glPointSize) glPointSize = nullptr;
decltype(glPolygonMode) glPolygonMode = nullptr;
decltype(glPolygonOffset) glPolygonOffset = nullptr;
decltype(glPopDebugGroup) glPopDebugGroup = nullptr;
decltype(glPrimitiveRestartIndex) glPrimitiveRestartIndex = nullptr;
decltype(glProgramBinary) glProgramBinary = nullptr;
decltype(glProgramParameteri) glProgramParameteri = nullptr;
decltype(glProgramUniform1d) glProgramUniform1d = nullptr;
decltype(glProgramUniform1dv) glProgramUniform1dv = nullptr;
decltype(glProgramUniform1f) glProgramUniform1f = nullptr;
decltype(glProgramUniform1fv) glProgramUniform1fv = nullptr;
decltype(glProgramUniform1i) glProgramUniform1i = nullptr;
decltype(glProgramUniform1iv) glProgramUniform1iv = nullptr;
decltype(glProgramUniform1ui) glProgramUniform1ui = nullptr;
decltype(glProgramUniform1uiv) glProgramUniform1uiv = nullptr;
decltype(glProgramUniform2d) glProgramUniform2d = nullptr;
decltype(glProgramUniform2dv) glProgramUniform2dv = nullptr;
decltype(glProgramUniform2f) glProgramUniform2f = nullptr;
decltype(glProgramUniform2fv) glProgramUniform2fv = nullptr;
decltype(glProgramUniform2i) glProgramUniform2i = nullptr;
decltype(glProgramUniform2iv) glProgramUniform2iv = nullptr;
decltype(glProgramUniform2ui) glProgramUniform2ui = nullptr;
decltype(glProgramUniform2uiv) glProgramUniform2uiv = nullptr;
decltype(glProgramUniform3d) glProgramUniform3d = nullptr;
decltype(glProgramUniform3dv) glProgramUniform3dv = nullptr;
decltype(glProgramUniform3f) glProgramUniform3f = nullptr;
decltype(glProgramUniform3fv) glProgramUniform3fv = nullptr;
decltype(glProgramUniform3i) glProgramUniform3i = nullptr;
decltype(glProgramUniform3iv) glProgramUniform3iv = nullptr;
decltype(glProgramUniform3ui) glProgramUniform3ui = nullptr;
decltype(glProgramUniform3uiv) glProgramUniform3uiv = nullptr;
decltype(glProgramUniform4d) glProgramUniform4d = nullptr;
decltype(glProgramUniform4dv) glProgramUniform4dv = nullptr;
decltype(glProgramUniform4f) glProgramUniform4f = nullptr;
decltype(glProgramUniform4fv) glProgramUniform4fv = nullptr;
decltype(glProgramUniform4i) glProgramUniform4i = nullptr;
decltype(glProgramUniform4iv) glProgramUniform4iv = nullptr;
decltype(glProgramUniform4ui) glProgramUniform4ui = nullptr;
decltype(glProgramUniform4uiv) glProgramUniform4uiv = nullptr;
decltype(glProgramUniformMatrix2dv) glProgramUniformMatrix2dv = nullptr;
decltype(glProgramUniformMatrix2fv) glProgramUniformMatrix2fv = nullptr;
decltype(glProgramUniformMatrix2x3dv) glProgramUniformMatrix2x3dv = nullptr;
decltype(glProgramUniformMatrix2x3fv) glProgramUniformMatrix2x3fv = nullptr;
decltype(glProgramUniformMatrix2x4dv) glProgramUniformMatrix2x4dv = nullptr;
decltype(glProgramUniformMatrix2x4fv) glProgramUniformMatrix2x4fv = nullptr;
decltype(glProgramUniformMatrix3dv) glProgramUniformMatrix3dv = nullptr;
decltype(glProgramUniformMatrix3fv) glProgramUniformMatrix3fv = nullptr;
decltype(glProgramUniformMatrix3x2dv) glProgramUniformMatrix3x2dv = nullptr;
decltype(glProgramUniformMatrix3x2fv) glProgramUniformMatrix3x2fv = nullptr;
decltype(glProgramUniformMatrix3x4dv) glProgramUniformMatrix3x4dv = nullptr;
decltype(glProgramUniformMatrix3x4fv) glProgramUniformMatrix3x4fv = nullptr;
decltype(glProgramUniformMatrix4dv) glProgramUniformMatrix4dv = nullptr;
decltype(glProgramUniformMatrix4fv) glProgramUniformMatrix4fv = nullptr;
decltype(glProgramUniformMatrix4x2dv) glProgramUniformMatrix4x2dv = nullptr;
decltype(glProgramUniformMatrix4x2fv) glProgramUniformMatrix4x2fv = nullptr;
decltype(glProgramUniformMatrix4x3dv) glProgramUniformMatrix4x3dv = nullptr;
decltype(glProgramUniformMatrix4x3fv) glProgramUniformMatrix4x3fv = nullptr;
decltype(glProvokingVertex) glProvokingVertex = nullptr;
decltype(glPushDebugGroup) glPushDebugGroup = nullptr;
decltype(glQueryCounter) glQueryCounter = nullptr;
decltype(glReadBuffer) glReadBuffer = nullptr;
decltype(glReadPixels) glReadPixels = nullptr;
decltype(glReadnPixels) glReadnPixels = nullptr;
decltype(glReleaseShaderCompiler) glReleaseShaderCompiler = nullptr;
decltype(glRenderbufferStorage) glRenderbufferStorage = nullptr;
decltype(glRenderbufferStorageMultisample) glRenderbufferStorageMultisample = nullptr;
decltype(glResumeTransformFeedback) glResumeTransformFeedback = nullptr;
decltype(glSampleCoverage) glSampleCoverage = nullptr;
decltype(glSampleMaski) glSampleMaski = nullptr;
decltype(glSamplerParameterIiv) glSamplerParameterIiv = nullptr;
decltype(glSamplerParameterIuiv) glSamplerParameterIuiv = nullptr;
decltype(glSamplerParameterf) glSamplerParameterf = nullptr;
decltype(glSamplerParameterfv) glSamplerParameterfv = nullptr;
decltype(glSamplerParameteri) glSamplerParameteri = nullptr;
decltype(glSamplerParameteriv) glSamplerParameteriv = nullptr;
decltype(glScissor) glScissor = nullptr;
decltype(glScissorArrayv) glScissorArrayv = nullptr;
decltype(glScissorIndexed) glScissorIndexed = nullptr;
decltype(glScissorIndexedv) glScissorIndexedv = nullptr;
decltype(glSecondaryColorP3ui) glSecondaryColorP3ui = nullptr;
decltype(glSecondaryColorP3uiv) glSecondaryColorP3uiv = nullptr;
decltype(glShaderBinary) glShaderBinary = nullptr;
decltype(glShaderSource) glShaderSource = nullptr;
decltype(glShaderStorageBlockBinding) glShaderStorageBlockBinding = nullptr;
decltype(glStencilFunc) glStencilFunc = nullptr;
decltype(glStencilFuncSeparate) glStencilFuncSeparate = nullptr;
decltype(glStencilMask) glStencilMask = nullptr;
decltype(glStencilMaskSeparate) glStencilMaskSeparate = nullptr;
decltype(glStencilOp) glStencilOp = nullptr;
decltype(glStencilOpSeparate) glStencilOpSeparate = nullptr;
decltype(glTexBuffer) glTexBuffer = nullptr;
decltype(glTexBufferRange) glTexBufferRange = nullptr;
decltype(glTexCoordP1ui) glTexCoordP1ui = nullptr;
decltype(glTexCoordP1uiv) glTexCoordP1uiv = nullptr;
decltype(glTexCoordP2ui) glTexCoordP2ui = nullptr;
decltype(glTexCoordP2uiv) glTexCoordP2uiv = nullptr;
decltype(glTexCoordP3ui) glTexCoordP3ui = nullptr;
decltype(glTexCoordP3uiv) glTexCoordP3uiv = nullptr;
decltype(glTexCoordP4ui) glTexCoordP4ui = nullptr;
decltype(glTexCoordP4uiv) glTexCoordP4uiv = nullptr;
decltype(glTexImage1D) glTexImage1D = nullptr;
decltype(glTexImage2D) glTexImage2D = nullptr;
decltype(glTexImage2DMultisample) glTexImage2DMultisample = nullptr;
decltype(glTexImage3D) glTexImage3D = nullptr;
decltype(glTexImage3DMultisample) glTexImage3DMultisample = nullptr;
decltype(glTexParameterIiv) glTexParameterIiv = nullptr;
decltype(glTexParameterIuiv) glTexParameterIuiv = nullptr;
decltype(glTexParameterf) glTexParameterf = nullptr;
decltype(glTexParameterfv) glTexParameterfv = nullptr;
decltype(glTexParameteri) glTexParameteri = nullptr;
decltype(glTexParameteriv) glTexParameteriv = nullptr;
decltype(glTexStorage1D) glTexStorage1D = nullptr;
decltype(glTexStorage2D) glTexStorage2D = nullptr;
decltype(glTexStorage2DMultisample) glTexStorage2DMultisample = nullptr;
decltype(glTexStorage3D) glTexStorage3D = nullptr;
decltype(glTexStorage3DMultisample) glTexStorage3DMultisample = nullptr;
decltype(glTexSubImage1D) glTexSubImage1D = nullptr;
decltype(glTexSubImage2D) glTexSubImage2D = nullptr;
decltype(glTexSubImage3D) glTexSubImage3D = nullptr;
decltype(glTextureBarrier) glTextureBarrier = nullptr;
decltype(glTextureBuffer) glTextureBuffer = nullptr;
decltype(glTextureBufferRange) glTextureBufferRange = nullptr;
decltype(glTextureParameterIiv) glTextureParameterIiv = nullptr;
decltype(glTextureParameterIuiv) glTextureParameterIuiv = nullptr;
decltype(glTextureParameterf) glTextureParameterf = nullptr;
decltype(glTextureParameterfv) glTextureParameterfv = nullptr;
decltype(glTextureParameteri) glTextureParameteri = nullptr;
decltype(glTextureParameteriv) glTextureParameteriv = nullptr;
decltype(glTextureStorage1D) glTextureStorage1D = nullptr;
decltype(glTextureStorage2D) glTextureStorage2D = nullptr;
decltype(glTextureStorage2DMultisample) glTextureStorage2DMultisample = nullptr;
decltype(glTextureStorage3D) glTextureStorage3D = nullptr;
decltype(glTextureStorage3DMultisample) glTextureStorage3DMultisample = nullptr;
decltype(glTextureSubImage1D) glTextureSubImage1D = nullptr;
decltype(glTextureSubImage2D) glTextureSubImage2D = nullptr;
decltype(glTextureSubImage3D) glTextureSubImage3D = nullptr;
decltype(glTextureView) glTextureView = nullptr;
decltype(glTransformFeedbackBufferBase) glTransformFeedbackBufferBase = nullptr;
decltype(glTransformFeedbackBufferRange) glTransformFeedbackBufferRange = nullptr;
decltype(glTransformFeedbackVaryings) glTransformFeedbackVaryings = nullptr;
decltype(glUniform1d) glUniform1d = nullptr;
decltype(glUniform1dv) glUniform1dv = nullptr;
decltype(glUniform1f) glUniform1f = nullptr;
decltype(glUniform1fv) glUniform1fv = nullptr;
decltype(glUniform1i) glUniform1i = nullptr;
decltype(glUniform1iv) glUniform1iv = nullptr;
decltype(glUniform1ui) glUniform1ui = nullptr;
decltype(glUniform1uiv) glUniform1uiv = nullptr;
decltype(glUniform2d) glUniform2d = nullptr;
decltype(glUniform2dv) glUniform2dv = nullptr;
decltype(glUniform2f) glUniform2f = nullptr;
decltype(glUniform2fv) glUniform2fv = nullptr;
decltype(glUniform2i) glUniform2i = nullptr;
decltype(glUniform2iv) glUniform2iv = nullptr;
decltype(glUniform2ui) glUniform2ui = nullptr;
decltype(glUniform2uiv) glUniform2uiv = nullptr;
decltype(glUniform3d) glUniform3d = nullptr;
decltype(glUniform3dv) glUniform3dv = nullptr;
decltype(glUniform3f) glUniform3f = nullptr;
decltype(glUniform3fv) glUniform3fv = nullptr;
decltype(glUniform3i) glUniform3i = nullptr;
decltype(glUniform3iv) glUniform3iv = nullptr;
decltype(glUniform3ui) glUniform3ui = nullptr;
decltype(glUniform3uiv) glUniform3uiv = nullptr;
decltype(glUniform4d) glUniform4d = nullptr;
decltype(glUniform4dv) glUniform4dv = nullptr;
decltype(glUniform4f) glUniform4f = nullptr;
decltype(glUniform4fv) glUniform4fv = nullptr;
decltype(glUniform4i) glUniform4i = nullptr;
decltype(glUniform4iv) glUniform4iv = nullptr;
decltype(glUniform4ui) glUniform4ui = nullptr;
decltype(glUniform4uiv) glUniform4uiv = nullptr;
decltype(glUniformBlockBinding) glUniformBlockBinding = nullptr;
decltype(glUniformMatrix2dv) glUniformMatrix2dv = nullptr;
decltype(glUniformMatrix2fv) glUniformMatrix2fv = nullptr;
decltype(glUniformMatrix2x3dv) glUniformMatrix2x3dv = nullptr;
decltype(glUniformMatrix2x3fv) glUniformMatrix2x3fv = nullptr;
decltype(glUniformMatrix2x4dv) glUniformMatrix2x4dv = nullptr;
decltype(glUniformMatrix2x4fv) glUniformMatrix2x4fv = nullptr;
decltype(glUniformMatrix3dv) glUniformMatrix3dv = nullptr;
decltype(glUniformMatrix3fv) glUniformMatrix3fv = nullptr;
decltype(glUniformMatrix3x2dv) glUniformMatrix3x2dv = nullptr;
decltype(glUniformMatrix3x2fv) glUniformMatrix3x2fv = nullptr;
decltype(glUniformMatrix3x4dv) glUniformMatrix3x4dv = nullptr;
decltype(glUniformMatrix3x4fv) glUniformMatrix3x4fv = nullptr;
decltype(glUniformMatrix4dv) glUniformMatrix4dv = nullptr;
decltype(glUniformMatrix4fv) glUniformMatrix4fv = nullptr;
decltype(glUniformMatrix4x2dv) glUniformMatrix4x2dv = nullptr;
decltype(glUniformMatrix4x2fv) glUniformMatrix4x2fv = nullptr;
decltype(glUniformMatrix4x3dv) glUniformMatrix4x3dv = nullptr;
decltype(glUniformMatrix4x3fv) glUniformMatrix4x3fv = nullptr;
decltype(glUniformSubroutinesuiv) glUniformSubroutinesuiv = nullptr;
decltype(glUnmapBuffer) glUnmapBuffer = nullptr;
decltype(glUnmapNamedBuffer) glUnmapNamedBuffer = nullptr;
decltype(glUseProgram) glUseProgram = nullptr;
decltype(glUseProgramStages) glUseProgramStages = nullptr;
decltype(glValidateProgram) glValidateProgram = nullptr;
decltype(glValidateProgramPipeline) glValidateProgramPipeline = nullptr;
decltype(glVertexArrayAttribBinding) glVertexArrayAttribBinding = nullptr;
decltype(glVertexArrayAttribFormat) glVertexArrayAttribFormat = nullptr;
decltype(glVertexArrayAttribIFormat) glVertexArrayAttribIFormat = nullptr;
decltype(glVertexArrayAttribLFormat) glVertexArrayAttribLFormat = nullptr;
decltype(glVertexArrayBindingDivisor) glVertexArrayBindingDivisor = nullptr;
decltype(glVertexArrayElementBuffer) glVertexArrayElementBuffer = nullptr;
decltype(glVertexArrayVertexBuffer) glVertexArrayVertexBuffer = nullptr;
decltype(glVertexArrayVertexBuffers) glVertexArrayVertexBuffers = nullptr;
decltype(glVertexAttrib1d) glVertexAttrib1d = nullptr;
decltype(glVertexAttrib1dv) glVertexAttrib1dv = nullptr;
decltype(glVertexAttrib1f) glVertexAttrib1f = nullptr;
decltype(glVertexAttrib1fv) glVertexAttrib1fv = nullptr;
decltype(glVertexAttrib1s) glVertexAttrib1s = nullptr;
decltype(glVertexAttrib1sv) glVertexAttrib1sv = nullptr;
decltype(glVertexAttrib2d) glVertexAttrib2d = nullptr;
decltype(glVertexAttrib2dv) glVertexAttrib2dv = nullptr;
decltype(glVertexAttrib2f) glVertexAttrib2f = nullptr;
decltype(glVertexAttrib2fv) glVertexAttrib2fv = nullptr;
decltype(glVertexAttrib2s) glVertexAttrib2s = nullptr;
decltype(glVertexAttrib2sv) glVertexAttrib2sv = nullptr;
decltype(glVertexAttrib3d) glVertexAttrib3d = nullptr;
decltype(glVertexAttrib3dv) glVertexAttrib3dv = nullptr;
decltype(glVertexAttrib3f) glVertexAttrib3f = nullptr;
decltype(glVertexAttrib3fv) glVertexAttrib3fv = nullptr;
decltype(glVertexAttrib3s) glVertexAttrib3s = nullptr;
decltype(glVertexAttrib3sv) glVertexAttrib3sv = nullptr;
decltype(glVertexAttrib4Nbv) glVertexAttrib4Nbv = nullptr;
decltype(glVertexAttrib4Niv) glVertexAttrib4Niv = nullptr;
decltype(glVertexAttrib4Nsv) glVertexAttrib4Nsv = nullptr;
decltype(glVertexAttrib4Nub) glVertexAttrib4Nub = nullptr;
decltype(glVertexAttrib4Nubv) glVertexAttrib4Nubv = nullptr;
decltype(glVertexAttrib4Nuiv) glVertexAttrib4Nuiv = nullptr;
decltype(glVertexAttrib4Nusv) glVertexAttrib4Nusv = nullptr;
decltype(glVertexAttrib4bv) glVertexAttrib4bv = nullptr;
decltype(glVertexAttrib4d) glVertexAttrib4d = nullptr;
decltype(glVertexAttrib4dv) glVertexAttrib4dv = nullptr;
decltype(glVertexAttrib4f) glVertexAttrib4f = nullptr;
decltype(glVertexAttrib4fv) glVertexAttrib4fv = nullptr;
decltype(glVertexAttrib4iv) glVertexAttrib4iv = nullptr;
decltype(glVertexAttrib4s) glVertexAttrib4s = nullptr;
decltype(glVertexAttrib4sv) glVertexAttrib4sv = nullptr;
decltype(glVertexAttrib4ubv) glVertexAttrib4ubv = nullptr;
decltype(glVertexAttrib4uiv) glVertexAttrib4uiv = nullptr;
decltype(glVertexAttrib4usv) glVertexAttrib4usv = nullptr;
decltype(glVertexAttribBinding) glVertexAttribBinding = nullptr;
decltype(glVertexAttribDivisor) glVertexAttribDivisor = nullptr;
decltype(glVertexAttribFormat) glVertexAttribFormat = nullptr;
decltype(glVertexAttribI1i) glVertexAttribI1i = nullptr;
decltype(glVertexAttribI1iv) glVertexAttribI1iv = nullptr;
decltype(glVertexAttribI1ui) glVertexAttribI1ui = nullptr;
decltype(glVertexAttribI1uiv) glVertexAttribI1uiv = nullptr;
decltype(glVertexAttribI2i) glVertexAttribI2i = nullptr;
decltype(glVertexAttribI2iv) glVertexAttribI2iv = nullptr;
decltype(glVertexAttribI2ui) glVertexAttribI2ui = nullptr;
decltype(glVertexAttribI2uiv) glVertexAttribI2uiv = nullptr;
decltype(glVertexAttribI3i) glVertexAttribI3i = nullptr;
decltype(glVertexAttribI3iv) glVertexAttribI3iv = nullptr;
decltype(glVertexAttribI3ui) glVertexAttribI3ui = nullptr;
decltype(glVertexAttribI3uiv) glVertexAttribI3uiv = nullptr;
decltype(glVertexAttribI4bv) glVertexAttribI4bv = nullptr;
decltype(glVertexAttribI4i) glVertexAttribI4i = nullptr;
decltype(glVertexAttribI4iv) glVertexAttribI4iv = nullptr;
decltype(glVertexAttribI4sv) glVertexAttribI4sv = nullptr;
decltype(glVertexAttribI4ubv) glVertexAttribI4ubv = nullptr;
decltype(glVertexAttribI4ui) glVertexAttribI4ui = nullptr;
decltype(glVertexAttribI4uiv) glVertexAttribI4uiv = nullptr;
decltype(glVertexAttribI4usv) glVertexAttribI4usv = nullptr;
decltype(glVertexAttribIFormat) glVertexAttribIFormat = nullptr;
decltype(glVertexAttribIPointer) glVertexAttribIPointer = nullptr;
decltype(glVertexAttribL1d) glVertexAttribL1d = nullptr;
decltype(glVertexAttribL1dv) glVertexAttribL1dv = nullptr;
decltype(glVertexAttribL2d) glVertexAttribL2d = nullptr;
decltype(glVertexAttribL2dv) glVertexAttribL2dv = nullptr;
decltype(glVertexAttribL3d) glVertexAttribL3d = nullptr;
decltype(glVertexAttribL3dv) glVertexAttribL3dv = nullptr;
decltype(glVertexAttribL4d) glVertexAttribL4d = nullptr;
decltype(glVertexAttribL4dv) glVertexAttribL4dv = nullptr;
decltype(glVertexAttribLFormat) glVertexAttribLFormat = nullptr;
decltype(glVertexAttribLPointer) glVertexAttribLPointer = nullptr;
decltype(glVertexAttribP1ui) glVertexAttribP1ui = nullptr;
decltype(glVertexAttribP1uiv) glVertexAttribP1uiv = nullptr;
decltype(glVertexAttribP2ui) glVertexAttribP2ui = nullptr;
decltype(glVertexAttribP2uiv) glVertexAttribP2uiv = nullptr;
decltype(glVertexAttribP3ui) glVertexAttribP3ui = nullptr;
decltype(glVertexAttribP3uiv) glVertexAttribP3uiv = nullptr;
decltype(glVertexAttribP4ui) glVertexAttribP4ui = nullptr;
decltype(glVertexAttribP4uiv) glVertexAttribP4uiv = nullptr;
decltype(glVertexAttribPointer) glVertexAttribPointer = nullptr;
decltype(glVertexBindingDivisor) glVertexBindingDivisor = nullptr;
decltype(glVertexP2ui) glVertexP2ui = nullptr;
decltype(glVertexP2uiv) glVertexP2uiv = nullptr;
decltype(glVertexP3ui) glVertexP3ui = nullptr;
decltype(glVertexP3uiv) glVertexP3uiv = nullptr;
decltype(glVertexP4ui) glVertexP4ui = nullptr;
decltype(glVertexP4uiv) glVertexP4uiv = nullptr;
decltype(glViewport) glViewport = nullptr;
decltype(glViewportArrayv) glViewportArrayv = nullptr;
decltype(glViewportIndexedf) glViewportIndexedf = nullptr;
decltype(glViewportIndexedfv) glViewportIndexedfv = nullptr;
decltype(glWaitSync) glWaitSync = nullptr;

} // namespace sq::gl::detail

//============================================================================//

#if defined(SQEE_LINUX)

#include <GL/glx.h>

#define LOAD_FUNCTION(Version, Name) \
  gl::detail::Name = reinterpret_cast<decltype(gl::detail::Name)>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(#Name))); \
  if (gl::detail::Name == nullptr) return "GLX: could not load version " #Version;

#elif defined(SQEE_WINDOWS)

#include <windows.h>

#define LOAD_FUNCTION(Version, Name) \
  gl::detail::Name = reinterpret_cast<decltype(gl::detail::Name)>(wglGetProcAddress(#Name)); \
  if (gl::detail::Name == nullptr) return "WGL: could not load version " #Version;

#endif

//============================================================================//

bool sq::load_opengl_core45_functions()
{
    /*#ifdef SQEE_WINDOWS

    // open

//    HMODULE gl32dll = LoadLibraryA("opengl32.dll");
//    if (gl32dll == NULL) return "could not load opengl32.dll";

    const auto load = [&](const char* name) -> void(*)()
    {
        auto result = wglGetProcAddress(name);

    };

    // close

//    FreeLibrary(HMODULE gl32dll);
//    gl32dll = NULL;
    #endif*/

    LOAD_FUNCTION(1.0, glCullFace)
    LOAD_FUNCTION(1.0, glFrontFace)
    LOAD_FUNCTION(1.0, glHint)
    LOAD_FUNCTION(1.0, glLineWidth)
    LOAD_FUNCTION(1.0, glPointSize)
    LOAD_FUNCTION(1.0, glPolygonMode)
    LOAD_FUNCTION(1.0, glScissor)
    LOAD_FUNCTION(1.0, glTexParameterf)
    LOAD_FUNCTION(1.0, glTexParameterfv)
    LOAD_FUNCTION(1.0, glTexParameteri)
    LOAD_FUNCTION(1.0, glTexParameteriv)
    LOAD_FUNCTION(1.0, glTexImage1D)
    LOAD_FUNCTION(1.0, glTexImage2D)
    LOAD_FUNCTION(1.0, glDrawBuffer)
    LOAD_FUNCTION(1.0, glClear)
    LOAD_FUNCTION(1.0, glClearColor)
    LOAD_FUNCTION(1.0, glClearStencil)
    LOAD_FUNCTION(1.0, glClearDepth)
    LOAD_FUNCTION(1.0, glStencilMask)
    LOAD_FUNCTION(1.0, glColorMask)
    LOAD_FUNCTION(1.0, glDepthMask)
    LOAD_FUNCTION(1.0, glDisable)
    LOAD_FUNCTION(1.0, glEnable)
    LOAD_FUNCTION(1.0, glFinish)
    LOAD_FUNCTION(1.0, glFlush)
    LOAD_FUNCTION(1.0, glBlendFunc)
    LOAD_FUNCTION(1.0, glLogicOp)
    LOAD_FUNCTION(1.0, glStencilFunc)
    LOAD_FUNCTION(1.0, glStencilOp)
    LOAD_FUNCTION(1.0, glDepthFunc)
    LOAD_FUNCTION(1.0, glPixelStoref)
    LOAD_FUNCTION(1.0, glPixelStorei)
    LOAD_FUNCTION(1.0, glReadBuffer)
    LOAD_FUNCTION(1.0, glReadPixels)
    LOAD_FUNCTION(1.0, glGetBooleanv)
    LOAD_FUNCTION(1.0, glGetDoublev)
    LOAD_FUNCTION(1.0, glGetError)
    LOAD_FUNCTION(1.0, glGetFloatv)
    LOAD_FUNCTION(1.0, glGetIntegerv)
    LOAD_FUNCTION(1.0, glGetString)
    LOAD_FUNCTION(1.0, glGetTexImage)
    LOAD_FUNCTION(1.0, glGetTexParameterfv)
    LOAD_FUNCTION(1.0, glGetTexParameteriv)
    LOAD_FUNCTION(1.0, glGetTexLevelParameterfv)
    LOAD_FUNCTION(1.0, glGetTexLevelParameteriv)
    LOAD_FUNCTION(1.0, glIsEnabled)
    LOAD_FUNCTION(1.0, glDepthRange)
    LOAD_FUNCTION(1.0, glViewport)

    LOAD_FUNCTION(1.1, glDrawArrays)
    LOAD_FUNCTION(1.1, glDrawElements)
    LOAD_FUNCTION(1.1, glPolygonOffset)
    LOAD_FUNCTION(1.1, glCopyTexImage1D)
    LOAD_FUNCTION(1.1, glCopyTexImage2D)
    LOAD_FUNCTION(1.1, glCopyTexSubImage1D)
    LOAD_FUNCTION(1.1, glCopyTexSubImage2D)
    LOAD_FUNCTION(1.1, glTexSubImage1D)
    LOAD_FUNCTION(1.1, glTexSubImage2D)
    LOAD_FUNCTION(1.1, glBindTexture)
    LOAD_FUNCTION(1.1, glDeleteTextures)
    LOAD_FUNCTION(1.1, glGenTextures)
    LOAD_FUNCTION(1.1, glIsTexture)

    LOAD_FUNCTION(1.2, glDrawRangeElements)
    LOAD_FUNCTION(1.2, glTexImage3D)
    LOAD_FUNCTION(1.2, glTexSubImage3D)
    LOAD_FUNCTION(1.2, glCopyTexSubImage3D)

    LOAD_FUNCTION(1.3, glActiveTexture)
    LOAD_FUNCTION(1.3, glSampleCoverage)
    LOAD_FUNCTION(1.3, glCompressedTexImage3D)
    LOAD_FUNCTION(1.3, glCompressedTexImage2D)
    LOAD_FUNCTION(1.3, glCompressedTexImage1D)
    LOAD_FUNCTION(1.3, glCompressedTexSubImage3D)
    LOAD_FUNCTION(1.3, glCompressedTexSubImage2D)
    LOAD_FUNCTION(1.3, glCompressedTexSubImage1D)
    LOAD_FUNCTION(1.3, glGetCompressedTexImage)

    LOAD_FUNCTION(1.4, glBlendFuncSeparate)
    LOAD_FUNCTION(1.4, glMultiDrawArrays)
    LOAD_FUNCTION(1.4, glMultiDrawElements)
    LOAD_FUNCTION(1.4, glPointParameterf)
    LOAD_FUNCTION(1.4, glPointParameterfv)
    LOAD_FUNCTION(1.4, glPointParameteri)
    LOAD_FUNCTION(1.4, glPointParameteriv)
    LOAD_FUNCTION(1.4, glBlendColor)
    LOAD_FUNCTION(1.4, glBlendEquation)

    LOAD_FUNCTION(1.5, glGenQueries)
    LOAD_FUNCTION(1.5, glDeleteQueries)
    LOAD_FUNCTION(1.5, glIsQuery)
    LOAD_FUNCTION(1.5, glBeginQuery)
    LOAD_FUNCTION(1.5, glEndQuery)
    LOAD_FUNCTION(1.5, glGetQueryiv)
    LOAD_FUNCTION(1.5, glGetQueryObjectiv)
    LOAD_FUNCTION(1.5, glGetQueryObjectuiv)
    LOAD_FUNCTION(1.5, glBindBuffer)
    LOAD_FUNCTION(1.5, glDeleteBuffers)
    LOAD_FUNCTION(1.5, glGenBuffers)
    LOAD_FUNCTION(1.5, glIsBuffer)
    LOAD_FUNCTION(1.5, glBufferData)
    LOAD_FUNCTION(1.5, glBufferSubData)
    LOAD_FUNCTION(1.5, glGetBufferSubData)
    LOAD_FUNCTION(1.5, glMapBuffer)
    LOAD_FUNCTION(1.5, glUnmapBuffer)
    LOAD_FUNCTION(1.5, glGetBufferParameteriv)
    LOAD_FUNCTION(1.5, glGetBufferPointerv)

    LOAD_FUNCTION(2.0, glBlendEquationSeparate)
    LOAD_FUNCTION(2.0, glDrawBuffers)
    LOAD_FUNCTION(2.0, glStencilOpSeparate)
    LOAD_FUNCTION(2.0, glStencilFuncSeparate)
    LOAD_FUNCTION(2.0, glStencilMaskSeparate)
    LOAD_FUNCTION(2.0, glAttachShader)
    LOAD_FUNCTION(2.0, glBindAttribLocation)
    LOAD_FUNCTION(2.0, glCompileShader)
    LOAD_FUNCTION(2.0, glCreateProgram)
    LOAD_FUNCTION(2.0, glCreateShader)
    LOAD_FUNCTION(2.0, glDeleteProgram)
    LOAD_FUNCTION(2.0, glDeleteShader)
    LOAD_FUNCTION(2.0, glDetachShader)
    LOAD_FUNCTION(2.0, glDisableVertexAttribArray)
    LOAD_FUNCTION(2.0, glEnableVertexAttribArray)
    LOAD_FUNCTION(2.0, glGetActiveAttrib)
    LOAD_FUNCTION(2.0, glGetActiveUniform)
    LOAD_FUNCTION(2.0, glGetAttachedShaders)
    LOAD_FUNCTION(2.0, glGetAttribLocation)
    LOAD_FUNCTION(2.0, glGetProgramiv)
    LOAD_FUNCTION(2.0, glGetProgramInfoLog)
    LOAD_FUNCTION(2.0, glGetShaderiv)
    LOAD_FUNCTION(2.0, glGetShaderInfoLog)
    LOAD_FUNCTION(2.0, glGetShaderSource)
    LOAD_FUNCTION(2.0, glGetUniformLocation)
    LOAD_FUNCTION(2.0, glGetUniformfv)
    LOAD_FUNCTION(2.0, glGetUniformiv)
    LOAD_FUNCTION(2.0, glGetVertexAttribdv)
    LOAD_FUNCTION(2.0, glGetVertexAttribfv)
    LOAD_FUNCTION(2.0, glGetVertexAttribiv)
    LOAD_FUNCTION(2.0, glGetVertexAttribPointerv)
    LOAD_FUNCTION(2.0, glIsProgram)
    LOAD_FUNCTION(2.0, glIsShader)
    LOAD_FUNCTION(2.0, glLinkProgram)
    LOAD_FUNCTION(2.0, glShaderSource)
    LOAD_FUNCTION(2.0, glUseProgram)
    LOAD_FUNCTION(2.0, glUniform1f)
    LOAD_FUNCTION(2.0, glUniform2f)
    LOAD_FUNCTION(2.0, glUniform3f)
    LOAD_FUNCTION(2.0, glUniform4f)
    LOAD_FUNCTION(2.0, glUniform1i)
    LOAD_FUNCTION(2.0, glUniform2i)
    LOAD_FUNCTION(2.0, glUniform3i)
    LOAD_FUNCTION(2.0, glUniform4i)
    LOAD_FUNCTION(2.0, glUniform1fv)
    LOAD_FUNCTION(2.0, glUniform2fv)
    LOAD_FUNCTION(2.0, glUniform3fv)
    LOAD_FUNCTION(2.0, glUniform4fv)
    LOAD_FUNCTION(2.0, glUniform1iv)
    LOAD_FUNCTION(2.0, glUniform2iv)
    LOAD_FUNCTION(2.0, glUniform3iv)
    LOAD_FUNCTION(2.0, glUniform4iv)
    LOAD_FUNCTION(2.0, glUniformMatrix2fv)
    LOAD_FUNCTION(2.0, glUniformMatrix3fv)
    LOAD_FUNCTION(2.0, glUniformMatrix4fv)
    LOAD_FUNCTION(2.0, glValidateProgram)
    LOAD_FUNCTION(2.0, glVertexAttrib1d)
    LOAD_FUNCTION(2.0, glVertexAttrib1dv)
    LOAD_FUNCTION(2.0, glVertexAttrib1f)
    LOAD_FUNCTION(2.0, glVertexAttrib1fv)
    LOAD_FUNCTION(2.0, glVertexAttrib1s)
    LOAD_FUNCTION(2.0, glVertexAttrib1sv)
    LOAD_FUNCTION(2.0, glVertexAttrib2d)
    LOAD_FUNCTION(2.0, glVertexAttrib2dv)
    LOAD_FUNCTION(2.0, glVertexAttrib2f)
    LOAD_FUNCTION(2.0, glVertexAttrib2fv)
    LOAD_FUNCTION(2.0, glVertexAttrib2s)
    LOAD_FUNCTION(2.0, glVertexAttrib2sv)
    LOAD_FUNCTION(2.0, glVertexAttrib3d)
    LOAD_FUNCTION(2.0, glVertexAttrib3dv)
    LOAD_FUNCTION(2.0, glVertexAttrib3f)
    LOAD_FUNCTION(2.0, glVertexAttrib3fv)
    LOAD_FUNCTION(2.0, glVertexAttrib3s)
    LOAD_FUNCTION(2.0, glVertexAttrib3sv)
    LOAD_FUNCTION(2.0, glVertexAttrib4Nbv)
    LOAD_FUNCTION(2.0, glVertexAttrib4Niv)
    LOAD_FUNCTION(2.0, glVertexAttrib4Nsv)
    LOAD_FUNCTION(2.0, glVertexAttrib4Nub)
    LOAD_FUNCTION(2.0, glVertexAttrib4Nubv)
    LOAD_FUNCTION(2.0, glVertexAttrib4Nuiv)
    LOAD_FUNCTION(2.0, glVertexAttrib4Nusv)
    LOAD_FUNCTION(2.0, glVertexAttrib4bv)
    LOAD_FUNCTION(2.0, glVertexAttrib4d)
    LOAD_FUNCTION(2.0, glVertexAttrib4dv)
    LOAD_FUNCTION(2.0, glVertexAttrib4f)
    LOAD_FUNCTION(2.0, glVertexAttrib4fv)
    LOAD_FUNCTION(2.0, glVertexAttrib4iv)
    LOAD_FUNCTION(2.0, glVertexAttrib4s)
    LOAD_FUNCTION(2.0, glVertexAttrib4sv)
    LOAD_FUNCTION(2.0, glVertexAttrib4ubv)
    LOAD_FUNCTION(2.0, glVertexAttrib4uiv)
    LOAD_FUNCTION(2.0, glVertexAttrib4usv)
    LOAD_FUNCTION(2.0, glVertexAttribPointer)

    LOAD_FUNCTION(2.1, glUniformMatrix2x3fv)
    LOAD_FUNCTION(2.1, glUniformMatrix3x2fv)
    LOAD_FUNCTION(2.1, glUniformMatrix2x4fv)
    LOAD_FUNCTION(2.1, glUniformMatrix4x2fv)
    LOAD_FUNCTION(2.1, glUniformMatrix3x4fv)
    LOAD_FUNCTION(2.1, glUniformMatrix4x3fv)

    LOAD_FUNCTION(3.0, glColorMaski)
    LOAD_FUNCTION(3.0, glGetBooleani_v)
    LOAD_FUNCTION(3.0, glGetIntegeri_v)
    LOAD_FUNCTION(3.0, glEnablei)
    LOAD_FUNCTION(3.0, glDisablei)
    LOAD_FUNCTION(3.0, glIsEnabledi)
    LOAD_FUNCTION(3.0, glBeginTransformFeedback)
    LOAD_FUNCTION(3.0, glEndTransformFeedback)
    LOAD_FUNCTION(3.0, glBindBufferRange)
    LOAD_FUNCTION(3.0, glBindBufferBase)
    LOAD_FUNCTION(3.0, glTransformFeedbackVaryings)
    LOAD_FUNCTION(3.0, glGetTransformFeedbackVarying)
    LOAD_FUNCTION(3.0, glClampColor)
    LOAD_FUNCTION(3.0, glBeginConditionalRender)
    LOAD_FUNCTION(3.0, glEndConditionalRender)
    LOAD_FUNCTION(3.0, glVertexAttribIPointer)
    LOAD_FUNCTION(3.0, glGetVertexAttribIiv)
    LOAD_FUNCTION(3.0, glGetVertexAttribIuiv)
    LOAD_FUNCTION(3.0, glVertexAttribI1i)
    LOAD_FUNCTION(3.0, glVertexAttribI2i)
    LOAD_FUNCTION(3.0, glVertexAttribI3i)
    LOAD_FUNCTION(3.0, glVertexAttribI4i)
    LOAD_FUNCTION(3.0, glVertexAttribI1ui)
    LOAD_FUNCTION(3.0, glVertexAttribI2ui)
    LOAD_FUNCTION(3.0, glVertexAttribI3ui)
    LOAD_FUNCTION(3.0, glVertexAttribI4ui)
    LOAD_FUNCTION(3.0, glVertexAttribI1iv)
    LOAD_FUNCTION(3.0, glVertexAttribI2iv)
    LOAD_FUNCTION(3.0, glVertexAttribI3iv)
    LOAD_FUNCTION(3.0, glVertexAttribI4iv)
    LOAD_FUNCTION(3.0, glVertexAttribI1uiv)
    LOAD_FUNCTION(3.0, glVertexAttribI2uiv)
    LOAD_FUNCTION(3.0, glVertexAttribI3uiv)
    LOAD_FUNCTION(3.0, glVertexAttribI4uiv)
    LOAD_FUNCTION(3.0, glVertexAttribI4bv)
    LOAD_FUNCTION(3.0, glVertexAttribI4sv)
    LOAD_FUNCTION(3.0, glVertexAttribI4ubv)
    LOAD_FUNCTION(3.0, glVertexAttribI4usv)
    LOAD_FUNCTION(3.0, glGetUniformuiv)
    LOAD_FUNCTION(3.0, glBindFragDataLocation)
    LOAD_FUNCTION(3.0, glGetFragDataLocation)
    LOAD_FUNCTION(3.0, glUniform1ui)
    LOAD_FUNCTION(3.0, glUniform2ui)
    LOAD_FUNCTION(3.0, glUniform3ui)
    LOAD_FUNCTION(3.0, glUniform4ui)
    LOAD_FUNCTION(3.0, glUniform1uiv)
    LOAD_FUNCTION(3.0, glUniform2uiv)
    LOAD_FUNCTION(3.0, glUniform3uiv)
    LOAD_FUNCTION(3.0, glUniform4uiv)
    LOAD_FUNCTION(3.0, glTexParameterIiv)
    LOAD_FUNCTION(3.0, glTexParameterIuiv)
    LOAD_FUNCTION(3.0, glGetTexParameterIiv)
    LOAD_FUNCTION(3.0, glGetTexParameterIuiv)
    LOAD_FUNCTION(3.0, glClearBufferiv)
    LOAD_FUNCTION(3.0, glClearBufferuiv)
    LOAD_FUNCTION(3.0, glClearBufferfv)
    LOAD_FUNCTION(3.0, glClearBufferfi)
    LOAD_FUNCTION(3.0, glGetStringi)
    LOAD_FUNCTION(3.0, glIsRenderbuffer)
    LOAD_FUNCTION(3.0, glBindRenderbuffer)
    LOAD_FUNCTION(3.0, glDeleteRenderbuffers)
    LOAD_FUNCTION(3.0, glGenRenderbuffers)
    LOAD_FUNCTION(3.0, glRenderbufferStorage)
    LOAD_FUNCTION(3.0, glGetRenderbufferParameteriv)
    LOAD_FUNCTION(3.0, glIsFramebuffer)
    LOAD_FUNCTION(3.0, glBindFramebuffer)
    LOAD_FUNCTION(3.0, glDeleteFramebuffers)
    LOAD_FUNCTION(3.0, glGenFramebuffers)
    LOAD_FUNCTION(3.0, glCheckFramebufferStatus)
    LOAD_FUNCTION(3.0, glFramebufferTexture1D)
    LOAD_FUNCTION(3.0, glFramebufferTexture2D)
    LOAD_FUNCTION(3.0, glFramebufferTexture3D)
    LOAD_FUNCTION(3.0, glFramebufferRenderbuffer)
    LOAD_FUNCTION(3.0, glGetFramebufferAttachmentParameteriv)
    LOAD_FUNCTION(3.0, glGenerateMipmap)
    LOAD_FUNCTION(3.0, glBlitFramebuffer)
    LOAD_FUNCTION(3.0, glRenderbufferStorageMultisample)
    LOAD_FUNCTION(3.0, glFramebufferTextureLayer)
    LOAD_FUNCTION(3.0, glMapBufferRange)
    LOAD_FUNCTION(3.0, glFlushMappedBufferRange)
    LOAD_FUNCTION(3.0, glBindVertexArray)
    LOAD_FUNCTION(3.0, glDeleteVertexArrays)
    LOAD_FUNCTION(3.0, glGenVertexArrays)
    LOAD_FUNCTION(3.0, glIsVertexArray)

    LOAD_FUNCTION(3.1, glDrawArraysInstanced)
    LOAD_FUNCTION(3.1, glDrawElementsInstanced)
    LOAD_FUNCTION(3.1, glTexBuffer)
    LOAD_FUNCTION(3.1, glPrimitiveRestartIndex)
    LOAD_FUNCTION(3.1, glCopyBufferSubData)
    LOAD_FUNCTION(3.1, glGetUniformIndices)
    LOAD_FUNCTION(3.1, glGetActiveUniformsiv)
    LOAD_FUNCTION(3.1, glGetActiveUniformName)
    LOAD_FUNCTION(3.1, glGetUniformBlockIndex)
    LOAD_FUNCTION(3.1, glGetActiveUniformBlockiv)
    LOAD_FUNCTION(3.1, glGetActiveUniformBlockName)
    LOAD_FUNCTION(3.1, glUniformBlockBinding)
    LOAD_FUNCTION(3.1, glBindBufferRange)
    LOAD_FUNCTION(3.1, glBindBufferBase)
    LOAD_FUNCTION(3.1, glGetIntegeri_v)

    LOAD_FUNCTION(3.2, glDrawElementsBaseVertex)
    LOAD_FUNCTION(3.2, glDrawRangeElementsBaseVertex)
    LOAD_FUNCTION(3.2, glDrawElementsInstancedBaseVertex)
    LOAD_FUNCTION(3.2, glMultiDrawElementsBaseVertex)
    LOAD_FUNCTION(3.2, glProvokingVertex)
    LOAD_FUNCTION(3.2, glFenceSync)
    LOAD_FUNCTION(3.2, glIsSync)
    LOAD_FUNCTION(3.2, glDeleteSync)
    LOAD_FUNCTION(3.2, glClientWaitSync)
    LOAD_FUNCTION(3.2, glWaitSync)
    LOAD_FUNCTION(3.2, glGetInteger64v)
    LOAD_FUNCTION(3.2, glGetSynciv)
    LOAD_FUNCTION(3.2, glGetInteger64i_v)
    LOAD_FUNCTION(3.2, glGetBufferParameteri64v)
    LOAD_FUNCTION(3.2, glFramebufferTexture)
    LOAD_FUNCTION(3.2, glTexImage2DMultisample)
    LOAD_FUNCTION(3.2, glTexImage3DMultisample)
    LOAD_FUNCTION(3.2, glGetMultisamplefv)
    LOAD_FUNCTION(3.2, glSampleMaski)

    LOAD_FUNCTION(3.3, glBindFragDataLocationIndexed)
    LOAD_FUNCTION(3.3, glGetFragDataIndex)
    LOAD_FUNCTION(3.3, glGenSamplers)
    LOAD_FUNCTION(3.3, glDeleteSamplers)
    LOAD_FUNCTION(3.3, glIsSampler)
    LOAD_FUNCTION(3.3, glBindSampler)
    LOAD_FUNCTION(3.3, glSamplerParameteri)
    LOAD_FUNCTION(3.3, glSamplerParameteriv)
    LOAD_FUNCTION(3.3, glSamplerParameterf)
    LOAD_FUNCTION(3.3, glSamplerParameterfv)
    LOAD_FUNCTION(3.3, glSamplerParameterIiv)
    LOAD_FUNCTION(3.3, glSamplerParameterIuiv)
    LOAD_FUNCTION(3.3, glGetSamplerParameteriv)
    LOAD_FUNCTION(3.3, glGetSamplerParameterIiv)
    LOAD_FUNCTION(3.3, glGetSamplerParameterfv)
    LOAD_FUNCTION(3.3, glGetSamplerParameterIuiv)
    LOAD_FUNCTION(3.3, glQueryCounter)
    LOAD_FUNCTION(3.3, glGetQueryObjecti64v)
    LOAD_FUNCTION(3.3, glGetQueryObjectui64v)
    LOAD_FUNCTION(3.3, glVertexAttribDivisor)
    LOAD_FUNCTION(3.3, glVertexAttribP1ui)
    LOAD_FUNCTION(3.3, glVertexAttribP1uiv)
    LOAD_FUNCTION(3.3, glVertexAttribP2ui)
    LOAD_FUNCTION(3.3, glVertexAttribP2uiv)
    LOAD_FUNCTION(3.3, glVertexAttribP3ui)
    LOAD_FUNCTION(3.3, glVertexAttribP3uiv)
    LOAD_FUNCTION(3.3, glVertexAttribP4ui)
    LOAD_FUNCTION(3.3, glVertexAttribP4uiv)
    LOAD_FUNCTION(3.3, glVertexP2ui)
    LOAD_FUNCTION(3.3, glVertexP2uiv)
    LOAD_FUNCTION(3.3, glVertexP3ui)
    LOAD_FUNCTION(3.3, glVertexP3uiv)
    LOAD_FUNCTION(3.3, glVertexP4ui)
    LOAD_FUNCTION(3.3, glVertexP4uiv)
    LOAD_FUNCTION(3.3, glTexCoordP1ui)
    LOAD_FUNCTION(3.3, glTexCoordP1uiv)
    LOAD_FUNCTION(3.3, glTexCoordP2ui)
    LOAD_FUNCTION(3.3, glTexCoordP2uiv)
    LOAD_FUNCTION(3.3, glTexCoordP3ui)
    LOAD_FUNCTION(3.3, glTexCoordP3uiv)
    LOAD_FUNCTION(3.3, glTexCoordP4ui)
    LOAD_FUNCTION(3.3, glTexCoordP4uiv)
    LOAD_FUNCTION(3.3, glMultiTexCoordP1ui)
    LOAD_FUNCTION(3.3, glMultiTexCoordP1uiv)
    LOAD_FUNCTION(3.3, glMultiTexCoordP2ui)
    LOAD_FUNCTION(3.3, glMultiTexCoordP2uiv)
    LOAD_FUNCTION(3.3, glMultiTexCoordP3ui)
    LOAD_FUNCTION(3.3, glMultiTexCoordP3uiv)
    LOAD_FUNCTION(3.3, glMultiTexCoordP4ui)
    LOAD_FUNCTION(3.3, glMultiTexCoordP4uiv)
    LOAD_FUNCTION(3.3, glNormalP3ui)
    LOAD_FUNCTION(3.3, glNormalP3uiv)
    LOAD_FUNCTION(3.3, glColorP3ui)
    LOAD_FUNCTION(3.3, glColorP3uiv)
    LOAD_FUNCTION(3.3, glColorP4ui)
    LOAD_FUNCTION(3.3, glColorP4uiv)
    LOAD_FUNCTION(3.3, glSecondaryColorP3ui)
    LOAD_FUNCTION(3.3, glSecondaryColorP3uiv)

    LOAD_FUNCTION(4.0, glMinSampleShading)
    LOAD_FUNCTION(4.0, glBlendEquationi)
    LOAD_FUNCTION(4.0, glBlendEquationSeparatei)
    LOAD_FUNCTION(4.0, glBlendFunci)
    LOAD_FUNCTION(4.0, glBlendFuncSeparatei)
    LOAD_FUNCTION(4.0, glDrawArraysIndirect)
    LOAD_FUNCTION(4.0, glDrawElementsIndirect)
    LOAD_FUNCTION(4.0, glUniform1d)
    LOAD_FUNCTION(4.0, glUniform2d)
    LOAD_FUNCTION(4.0, glUniform3d)
    LOAD_FUNCTION(4.0, glUniform4d)
    LOAD_FUNCTION(4.0, glUniform1dv)
    LOAD_FUNCTION(4.0, glUniform2dv)
    LOAD_FUNCTION(4.0, glUniform3dv)
    LOAD_FUNCTION(4.0, glUniform4dv)
    LOAD_FUNCTION(4.0, glUniformMatrix2dv)
    LOAD_FUNCTION(4.0, glUniformMatrix3dv)
    LOAD_FUNCTION(4.0, glUniformMatrix4dv)
    LOAD_FUNCTION(4.0, glUniformMatrix2x3dv)
    LOAD_FUNCTION(4.0, glUniformMatrix2x4dv)
    LOAD_FUNCTION(4.0, glUniformMatrix3x2dv)
    LOAD_FUNCTION(4.0, glUniformMatrix3x4dv)
    LOAD_FUNCTION(4.0, glUniformMatrix4x2dv)
    LOAD_FUNCTION(4.0, glUniformMatrix4x3dv)
    LOAD_FUNCTION(4.0, glGetUniformdv)
    LOAD_FUNCTION(4.0, glGetSubroutineUniformLocation)
    LOAD_FUNCTION(4.0, glGetSubroutineIndex)
    LOAD_FUNCTION(4.0, glGetActiveSubroutineUniformiv)
    LOAD_FUNCTION(4.0, glGetActiveSubroutineUniformName)
    LOAD_FUNCTION(4.0, glGetActiveSubroutineName)
    LOAD_FUNCTION(4.0, glUniformSubroutinesuiv)
    LOAD_FUNCTION(4.0, glGetUniformSubroutineuiv)
    LOAD_FUNCTION(4.0, glGetProgramStageiv)
    LOAD_FUNCTION(4.0, glPatchParameteri)
    LOAD_FUNCTION(4.0, glPatchParameterfv)
    LOAD_FUNCTION(4.0, glBindTransformFeedback)
    LOAD_FUNCTION(4.0, glDeleteTransformFeedbacks)
    LOAD_FUNCTION(4.0, glGenTransformFeedbacks)
    LOAD_FUNCTION(4.0, glIsTransformFeedback)
    LOAD_FUNCTION(4.0, glPauseTransformFeedback)
    LOAD_FUNCTION(4.0, glResumeTransformFeedback)
    LOAD_FUNCTION(4.0, glDrawTransformFeedback)
    LOAD_FUNCTION(4.0, glDrawTransformFeedbackStream)
    LOAD_FUNCTION(4.0, glBeginQueryIndexed)
    LOAD_FUNCTION(4.0, glEndQueryIndexed)
    LOAD_FUNCTION(4.0, glGetQueryIndexediv)

    LOAD_FUNCTION(4.1, glReleaseShaderCompiler)
    LOAD_FUNCTION(4.1, glShaderBinary)
    LOAD_FUNCTION(4.1, glGetShaderPrecisionFormat)
    LOAD_FUNCTION(4.1, glDepthRangef)
    LOAD_FUNCTION(4.1, glClearDepthf)
    LOAD_FUNCTION(4.1, glGetProgramBinary)
    LOAD_FUNCTION(4.1, glProgramBinary)
    LOAD_FUNCTION(4.1, glProgramParameteri)
    LOAD_FUNCTION(4.1, glUseProgramStages)
    LOAD_FUNCTION(4.1, glActiveShaderProgram)
    LOAD_FUNCTION(4.1, glCreateShaderProgramv)
    LOAD_FUNCTION(4.1, glBindProgramPipeline)
    LOAD_FUNCTION(4.1, glDeleteProgramPipelines)
    LOAD_FUNCTION(4.1, glGenProgramPipelines)
    LOAD_FUNCTION(4.1, glIsProgramPipeline)
    LOAD_FUNCTION(4.1, glGetProgramPipelineiv)
    LOAD_FUNCTION(4.1, glProgramParameteri)
    LOAD_FUNCTION(4.1, glProgramUniform1i)
    LOAD_FUNCTION(4.1, glProgramUniform1iv)
    LOAD_FUNCTION(4.1, glProgramUniform1f)
    LOAD_FUNCTION(4.1, glProgramUniform1fv)
    LOAD_FUNCTION(4.1, glProgramUniform1d)
    LOAD_FUNCTION(4.1, glProgramUniform1dv)
    LOAD_FUNCTION(4.1, glProgramUniform1ui)
    LOAD_FUNCTION(4.1, glProgramUniform1uiv)
    LOAD_FUNCTION(4.1, glProgramUniform2i)
    LOAD_FUNCTION(4.1, glProgramUniform2iv)
    LOAD_FUNCTION(4.1, glProgramUniform2f)
    LOAD_FUNCTION(4.1, glProgramUniform2fv)
    LOAD_FUNCTION(4.1, glProgramUniform2d)
    LOAD_FUNCTION(4.1, glProgramUniform2dv)
    LOAD_FUNCTION(4.1, glProgramUniform2ui)
    LOAD_FUNCTION(4.1, glProgramUniform2uiv)
    LOAD_FUNCTION(4.1, glProgramUniform3i)
    LOAD_FUNCTION(4.1, glProgramUniform3iv)
    LOAD_FUNCTION(4.1, glProgramUniform3f)
    LOAD_FUNCTION(4.1, glProgramUniform3fv)
    LOAD_FUNCTION(4.1, glProgramUniform3d)
    LOAD_FUNCTION(4.1, glProgramUniform3dv)
    LOAD_FUNCTION(4.1, glProgramUniform3ui)
    LOAD_FUNCTION(4.1, glProgramUniform3uiv)
    LOAD_FUNCTION(4.1, glProgramUniform4i)
    LOAD_FUNCTION(4.1, glProgramUniform4iv)
    LOAD_FUNCTION(4.1, glProgramUniform4f)
    LOAD_FUNCTION(4.1, glProgramUniform4fv)
    LOAD_FUNCTION(4.1, glProgramUniform4d)
    LOAD_FUNCTION(4.1, glProgramUniform4dv)
    LOAD_FUNCTION(4.1, glProgramUniform4ui)
    LOAD_FUNCTION(4.1, glProgramUniform4uiv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix2fv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix3fv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix4fv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix2dv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix3dv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix4dv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix2x3fv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix3x2fv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix2x4fv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix4x2fv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix3x4fv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix4x3fv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix2x3dv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix3x2dv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix2x4dv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix4x2dv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix3x4dv)
    LOAD_FUNCTION(4.1, glProgramUniformMatrix4x3dv)
    LOAD_FUNCTION(4.1, glValidateProgramPipeline)
    LOAD_FUNCTION(4.1, glGetProgramPipelineInfoLog)
    LOAD_FUNCTION(4.1, glVertexAttribL1d)
    LOAD_FUNCTION(4.1, glVertexAttribL2d)
    LOAD_FUNCTION(4.1, glVertexAttribL3d)
    LOAD_FUNCTION(4.1, glVertexAttribL4d)
    LOAD_FUNCTION(4.1, glVertexAttribL1dv)
    LOAD_FUNCTION(4.1, glVertexAttribL2dv)
    LOAD_FUNCTION(4.1, glVertexAttribL3dv)
    LOAD_FUNCTION(4.1, glVertexAttribL4dv)
    LOAD_FUNCTION(4.1, glVertexAttribLPointer)
    LOAD_FUNCTION(4.1, glGetVertexAttribLdv)
    LOAD_FUNCTION(4.1, glViewportArrayv)
    LOAD_FUNCTION(4.1, glViewportIndexedf)
    LOAD_FUNCTION(4.1, glViewportIndexedfv)
    LOAD_FUNCTION(4.1, glScissorArrayv)
    LOAD_FUNCTION(4.1, glScissorIndexed)
    LOAD_FUNCTION(4.1, glScissorIndexedv)
    LOAD_FUNCTION(4.1, glDepthRangeArrayv)
    LOAD_FUNCTION(4.1, glDepthRangeIndexed)
    LOAD_FUNCTION(4.1, glGetFloati_v)
    LOAD_FUNCTION(4.1, glGetDoublei_v)

    LOAD_FUNCTION(4.2, glDrawArraysInstancedBaseInstance)
    LOAD_FUNCTION(4.2, glDrawElementsInstancedBaseInstance)
    LOAD_FUNCTION(4.2, glDrawElementsInstancedBaseVertexBaseInstance)
    LOAD_FUNCTION(4.2, glGetInternalformativ)
    LOAD_FUNCTION(4.2, glGetActiveAtomicCounterBufferiv)
    LOAD_FUNCTION(4.2, glBindImageTexture)
    LOAD_FUNCTION(4.2, glMemoryBarrier)
    LOAD_FUNCTION(4.2, glTexStorage1D)
    LOAD_FUNCTION(4.2, glTexStorage2D)
    LOAD_FUNCTION(4.2, glTexStorage3D)
    LOAD_FUNCTION(4.2, glDrawTransformFeedbackInstanced)
    LOAD_FUNCTION(4.2, glDrawTransformFeedbackStreamInstanced)

    LOAD_FUNCTION(4.3, glClearBufferData)
    LOAD_FUNCTION(4.3, glClearBufferSubData)
    LOAD_FUNCTION(4.3, glDispatchCompute)
    LOAD_FUNCTION(4.3, glDispatchComputeIndirect)
    LOAD_FUNCTION(4.3, glCopyImageSubData)
    LOAD_FUNCTION(4.3, glFramebufferParameteri)
    LOAD_FUNCTION(4.3, glGetFramebufferParameteriv)
    LOAD_FUNCTION(4.3, glGetInternalformati64v)
    LOAD_FUNCTION(4.3, glInvalidateTexSubImage)
    LOAD_FUNCTION(4.3, glInvalidateTexImage)
    LOAD_FUNCTION(4.3, glInvalidateBufferSubData)
    LOAD_FUNCTION(4.3, glInvalidateBufferData)
    LOAD_FUNCTION(4.3, glInvalidateFramebuffer)
    LOAD_FUNCTION(4.3, glInvalidateSubFramebuffer)
    LOAD_FUNCTION(4.3, glMultiDrawArraysIndirect)
    LOAD_FUNCTION(4.3, glMultiDrawElementsIndirect)
    LOAD_FUNCTION(4.3, glGetProgramInterfaceiv)
    LOAD_FUNCTION(4.3, glGetProgramResourceIndex)
    LOAD_FUNCTION(4.3, glGetProgramResourceName)
    LOAD_FUNCTION(4.3, glGetProgramResourceiv)
    LOAD_FUNCTION(4.3, glGetProgramResourceLocation)
    LOAD_FUNCTION(4.3, glGetProgramResourceLocationIndex)
    LOAD_FUNCTION(4.3, glShaderStorageBlockBinding)
    LOAD_FUNCTION(4.3, glTexBufferRange)
    LOAD_FUNCTION(4.3, glTexStorage2DMultisample)
    LOAD_FUNCTION(4.3, glTexStorage3DMultisample)
    LOAD_FUNCTION(4.3, glTextureView)
    LOAD_FUNCTION(4.3, glBindVertexBuffer)
    LOAD_FUNCTION(4.3, glVertexAttribFormat)
    LOAD_FUNCTION(4.3, glVertexAttribIFormat)
    LOAD_FUNCTION(4.3, glVertexAttribLFormat)
    LOAD_FUNCTION(4.3, glVertexAttribBinding)
    LOAD_FUNCTION(4.3, glVertexBindingDivisor)
    LOAD_FUNCTION(4.3, glDebugMessageControl)
    LOAD_FUNCTION(4.3, glDebugMessageInsert)
    LOAD_FUNCTION(4.3, glDebugMessageCallback)
    LOAD_FUNCTION(4.3, glGetDebugMessageLog)
    LOAD_FUNCTION(4.3, glPushDebugGroup)
    LOAD_FUNCTION(4.3, glPopDebugGroup)
    LOAD_FUNCTION(4.3, glObjectLabel)
    LOAD_FUNCTION(4.3, glGetObjectLabel)
    LOAD_FUNCTION(4.3, glObjectPtrLabel)
    LOAD_FUNCTION(4.3, glGetObjectPtrLabel)
    LOAD_FUNCTION(4.3, glGetPointerv)

    LOAD_FUNCTION(4.4, glBufferStorage)
    LOAD_FUNCTION(4.4, glClearTexImage)
    LOAD_FUNCTION(4.4, glClearTexSubImage)
    LOAD_FUNCTION(4.4, glBindBuffersBase)
    LOAD_FUNCTION(4.4, glBindBuffersRange)
    LOAD_FUNCTION(4.4, glBindTextures)
    LOAD_FUNCTION(4.4, glBindSamplers)
    LOAD_FUNCTION(4.4, glBindImageTextures)
    LOAD_FUNCTION(4.4, glBindVertexBuffers)

    LOAD_FUNCTION(4.5, glClipControl)
    LOAD_FUNCTION(4.5, glCreateTransformFeedbacks)
    LOAD_FUNCTION(4.5, glTransformFeedbackBufferBase)
    LOAD_FUNCTION(4.5, glTransformFeedbackBufferRange)
    LOAD_FUNCTION(4.5, glGetTransformFeedbackiv)
    LOAD_FUNCTION(4.5, glGetTransformFeedbacki_v)
    LOAD_FUNCTION(4.5, glGetTransformFeedbacki64_v)
    LOAD_FUNCTION(4.5, glCreateBuffers)
    LOAD_FUNCTION(4.5, glNamedBufferStorage)
    LOAD_FUNCTION(4.5, glNamedBufferData)
    LOAD_FUNCTION(4.5, glNamedBufferSubData)
    LOAD_FUNCTION(4.5, glCopyNamedBufferSubData)
    LOAD_FUNCTION(4.5, glClearNamedBufferData)
    LOAD_FUNCTION(4.5, glClearNamedBufferSubData)
    LOAD_FUNCTION(4.5, glMapNamedBuffer)
    LOAD_FUNCTION(4.5, glMapNamedBufferRange)
    LOAD_FUNCTION(4.5, glUnmapNamedBuffer)
    LOAD_FUNCTION(4.5, glFlushMappedNamedBufferRange)
    LOAD_FUNCTION(4.5, glGetNamedBufferParameteriv)
    LOAD_FUNCTION(4.5, glGetNamedBufferParameteri64v)
    LOAD_FUNCTION(4.5, glGetNamedBufferPointerv)
    LOAD_FUNCTION(4.5, glGetNamedBufferSubData)
    LOAD_FUNCTION(4.5, glCreateFramebuffers)
    LOAD_FUNCTION(4.5, glNamedFramebufferRenderbuffer)
    LOAD_FUNCTION(4.5, glNamedFramebufferParameteri)
    LOAD_FUNCTION(4.5, glNamedFramebufferTexture)
    LOAD_FUNCTION(4.5, glNamedFramebufferTextureLayer)
    LOAD_FUNCTION(4.5, glNamedFramebufferDrawBuffer)
    LOAD_FUNCTION(4.5, glNamedFramebufferDrawBuffers)
    LOAD_FUNCTION(4.5, glNamedFramebufferReadBuffer)
    LOAD_FUNCTION(4.5, glInvalidateNamedFramebufferData)
    LOAD_FUNCTION(4.5, glInvalidateNamedFramebufferSubData)
    LOAD_FUNCTION(4.5, glClearNamedFramebufferiv)
    LOAD_FUNCTION(4.5, glClearNamedFramebufferuiv)
    LOAD_FUNCTION(4.5, glClearNamedFramebufferfv)
    LOAD_FUNCTION(4.5, glClearNamedFramebufferfi)
    LOAD_FUNCTION(4.5, glBlitNamedFramebuffer)
    LOAD_FUNCTION(4.5, glCheckNamedFramebufferStatus)
    LOAD_FUNCTION(4.5, glGetNamedFramebufferParameteriv)
    LOAD_FUNCTION(4.5, glGetNamedFramebufferAttachmentParameteriv)
    LOAD_FUNCTION(4.5, glCreateRenderbuffers)
    LOAD_FUNCTION(4.5, glNamedRenderbufferStorage)
    LOAD_FUNCTION(4.5, glNamedRenderbufferStorageMultisample)
    LOAD_FUNCTION(4.5, glGetNamedRenderbufferParameteriv)
    LOAD_FUNCTION(4.5, glCreateTextures)
    LOAD_FUNCTION(4.5, glTextureBuffer)
    LOAD_FUNCTION(4.5, glTextureBufferRange)
    LOAD_FUNCTION(4.5, glTextureStorage1D)
    LOAD_FUNCTION(4.5, glTextureStorage2D)
    LOAD_FUNCTION(4.5, glTextureStorage3D)
    LOAD_FUNCTION(4.5, glTextureStorage2DMultisample)
    LOAD_FUNCTION(4.5, glTextureStorage3DMultisample)
    LOAD_FUNCTION(4.5, glTextureSubImage1D)
    LOAD_FUNCTION(4.5, glTextureSubImage2D)
    LOAD_FUNCTION(4.5, glTextureSubImage3D)
    LOAD_FUNCTION(4.5, glCompressedTextureSubImage1D)
    LOAD_FUNCTION(4.5, glCompressedTextureSubImage2D)
    LOAD_FUNCTION(4.5, glCompressedTextureSubImage3D)
    LOAD_FUNCTION(4.5, glCopyTextureSubImage1D)
    LOAD_FUNCTION(4.5, glCopyTextureSubImage2D)
    LOAD_FUNCTION(4.5, glCopyTextureSubImage3D)
    LOAD_FUNCTION(4.5, glTextureParameterf)
    LOAD_FUNCTION(4.5, glTextureParameterfv)
    LOAD_FUNCTION(4.5, glTextureParameteri)
    LOAD_FUNCTION(4.5, glTextureParameterIiv)
    LOAD_FUNCTION(4.5, glTextureParameterIuiv)
    LOAD_FUNCTION(4.5, glTextureParameteriv)
    LOAD_FUNCTION(4.5, glGenerateTextureMipmap)
    LOAD_FUNCTION(4.5, glBindTextureUnit)
    LOAD_FUNCTION(4.5, glGetTextureImage)
    LOAD_FUNCTION(4.5, glGetCompressedTextureImage)
    LOAD_FUNCTION(4.5, glGetTextureLevelParameterfv)
    LOAD_FUNCTION(4.5, glGetTextureLevelParameteriv)
    LOAD_FUNCTION(4.5, glGetTextureParameterfv)
    LOAD_FUNCTION(4.5, glGetTextureParameterIiv)
    LOAD_FUNCTION(4.5, glGetTextureParameterIuiv)
    LOAD_FUNCTION(4.5, glGetTextureParameteriv)
    LOAD_FUNCTION(4.5, glCreateVertexArrays)
    LOAD_FUNCTION(4.5, glDisableVertexArrayAttrib)
    LOAD_FUNCTION(4.5, glEnableVertexArrayAttrib)
    LOAD_FUNCTION(4.5, glVertexArrayElementBuffer)
    LOAD_FUNCTION(4.5, glVertexArrayVertexBuffer)
    LOAD_FUNCTION(4.5, glVertexArrayVertexBuffers)
    LOAD_FUNCTION(4.5, glVertexArrayAttribBinding)
    LOAD_FUNCTION(4.5, glVertexArrayAttribFormat)
    LOAD_FUNCTION(4.5, glVertexArrayAttribIFormat)
    LOAD_FUNCTION(4.5, glVertexArrayAttribLFormat)
    LOAD_FUNCTION(4.5, glVertexArrayBindingDivisor)
    LOAD_FUNCTION(4.5, glGetVertexArrayiv)
    LOAD_FUNCTION(4.5, glGetVertexArrayIndexediv)
    LOAD_FUNCTION(4.5, glGetVertexArrayIndexed64iv)
    LOAD_FUNCTION(4.5, glCreateSamplers)
    LOAD_FUNCTION(4.5, glCreateProgramPipelines)
    LOAD_FUNCTION(4.5, glCreateQueries)
    LOAD_FUNCTION(4.5, glGetQueryBufferObjecti64v)
    LOAD_FUNCTION(4.5, glGetQueryBufferObjectiv)
    LOAD_FUNCTION(4.5, glGetQueryBufferObjectui64v)
    LOAD_FUNCTION(4.5, glGetQueryBufferObjectuiv)
    LOAD_FUNCTION(4.5, glMemoryBarrierByRegion)
    LOAD_FUNCTION(4.5, glGetTextureSubImage)
    LOAD_FUNCTION(4.5, glGetCompressedTextureSubImage)
    LOAD_FUNCTION(4.5, glGetGraphicsResetStatus)
    LOAD_FUNCTION(4.5, glGetnCompressedTexImage)
    LOAD_FUNCTION(4.5, glGetnTexImage)
    LOAD_FUNCTION(4.5, glGetnUniformdv)
    LOAD_FUNCTION(4.5, glGetnUniformfv)
    LOAD_FUNCTION(4.5, glGetnUniformiv)
    LOAD_FUNCTION(4.5, glGetnUniformuiv)
    LOAD_FUNCTION(4.5, glReadnPixels)
    LOAD_FUNCTION(4.5, glGetnMapdv)
    LOAD_FUNCTION(4.5, glGetnMapfv)
    LOAD_FUNCTION(4.5, glGetnMapiv)
    LOAD_FUNCTION(4.5, glGetnPixelMapfv)
    LOAD_FUNCTION(4.5, glGetnPixelMapuiv)
    LOAD_FUNCTION(4.5, glGetnPixelMapusv)
    LOAD_FUNCTION(4.5, glGetnPolygonStipple)
    LOAD_FUNCTION(4.5, glGetnColorTable)
    LOAD_FUNCTION(4.5, glGetnConvolutionFilter)
    LOAD_FUNCTION(4.5, glGetnSeparableFilter)
    LOAD_FUNCTION(4.5, glGetnHistogram)
    LOAD_FUNCTION(4.5, glGetnMinmax)
    LOAD_FUNCTION(4.5, glTextureBarrier)

    return true; // hooray!
}

//static void find_coreGL()
//{
//    const char* const version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
//    if (version == nullptr) return;

//    int major, minor;

//    #ifdef _MSC_VER
//      sscanf_s(version, "%d.%d", &major, &minor);
//    #else
//      sscanf(version, "%d.%d", &major, &minor);
//    #endif

//    GLVersion.major = major;
//    GLVersion.minor = minor;

//    if (GLVersion.major > 4 || (GLVersion.major >= 4 && GLVersion.minor >= 5))
//    {
//        max_loaded_major = 4;
//        max_loaded_minor = 5;
//    }
//}

//int gladLoadGLLoader(GLADloadproc load)
//{
//    GLVersion.major = 0; GLVersion.minor = 0;
//    glGetString = (PFNGLGETSTRINGPROC)load("glGetString"));
//    if(glGetString == NULL) return 0;
//    if(glGetString(GL_VERSION) == NULL) return 0;
//    find_coreGL();
//    load_GL_VERSION_1_0(load);
//    load_GL_VERSION_1_1(load);
//    load_GL_VERSION_1_2(load);
//    load_GL_VERSION_1_3(load);
//    load_GL_VERSION_1_4(load);
//    load_GL_VERSION_1_5(load);
//    load_GL_VERSION_2_0(load);
//    load_GL_VERSION_2_1(load);
//    load_GL_VERSION_3_0(load);
//    load_GL_VERSION_3_1(load);
//    load_GL_VERSION_3_2(load);
//    load_GL_VERSION_3_3(load);
//    load_GL_VERSION_4_0(load);
//    load_GL_VERSION_4_1(load);
//    load_GL_VERSION_4_2(load);
//    load_GL_VERSION_4_3(load);
//    load_GL_VERSION_4_4(load);
//    load_GL_VERSION_4_5(load);

//    if (!find_extensionsGL()) return 0;
//    return GLVersion.major != 0 || GLVersion.minor != 0;
//}

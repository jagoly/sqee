#include <fstream>

#include <extra/gl.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <extra/helpers.hpp>

namespace sqe {

std::string get_shader_error(const GLuint& shader) {
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetShaderInfoLog(shader, max_length, &actual_length, log);
    return log;
}


bool create_shader(std::string vertPath, std::string fragPath, GLuint& prog) {
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertStr = sqe::load_from_file(vertPath);
    const char* vertSrc = vertStr.c_str();
    int vertSize = vertStr.size();
    glShaderSource(vert, 1, &vertSrc, &vertSize);
    glCompileShader(vert);
    std::cout << sqe::get_shader_error(vert) << std::endl;

    std::string fragStr = sqe::load_from_file("res/shaders/cube_fs.glsl");
    const char* fragSrc = fragStr.c_str();
    int fragSize = fragStr.size();
    glShaderSource(frag, 1, &fragSrc, &fragSize);
    glCompileShader(frag);
    std::cout << sqe::get_shader_error(frag) << std::endl;

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    std::cout << sqe::get_shader_error(prog) << std::endl;
}

bool load_mesh(std::string filePath, GLuint& vao, int& pointCount) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);
    const aiMesh* mesh = scene->mMeshes[0];

    pointCount = mesh->mNumVertices;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat* points = NULL;
    GLfloat* normals = NULL;
    GLfloat* texcoords = NULL;

    if (mesh->HasPositions()) {
        points = (GLfloat*)malloc(pointCount * 3 * sizeof(GLfloat));
        for (int i = 0; i < pointCount; i++) {
            const aiVector3D* vp = &(mesh->mVertices[i]);
            points[i * 3] = GLfloat(vp->x);
            points[i * 3 + 1] = GLfloat(vp->y);
            points[i * 3 + 2] = GLfloat(vp->z);
        }

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * pointCount * sizeof(GLfloat), points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        free(points);
    }

    if (1) {
        normals = (GLfloat*)malloc(pointCount * 3 * sizeof(GLfloat));
        for (int i = 0; i < pointCount; i++) {
            const aiVector3D* vn = &(mesh->mNormals[i]);
            normals[i * 3] = GLfloat(vn->x);
            normals[i * 3 + 1] = GLfloat(vn->y);
            normals[i * 3 + 2] = GLfloat(vn->z);
        }

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * pointCount * sizeof(GLfloat), normals, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
        free(normals);
    }

    if (mesh->HasTextureCoords(0)) {
        texcoords = (GLfloat*)malloc(pointCount * 2 * sizeof(GLfloat));
        for (int i = 0; i < pointCount; i++) {
            const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
            texcoords[i * 2] = (GLfloat)vt->x;
            texcoords[i * 2 + 1] = (GLfloat)vt->y;
        }

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * pointCount * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
        free(texcoords);
    }

    return true;
}

}

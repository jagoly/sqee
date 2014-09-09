#include <fstream>
#include <sstream>

#include <misc/files.hpp>
#include <gl/gl.hpp>

namespace sq {

Vertex::Vertex(float _x, float _y, float _z, float _nx, float _ny, float _nz, float _s, float _t) {
    x = _x; y = _y; z = _z; nx = _nx; ny = _ny; nz = _nz; s = _s; t = _t;

    glm::vec3 tangent = get_tangent({nx, ny, nz});
    tx = tangent.x; ty = tangent.y; tz = tangent.z;
}

Json::Value get_json_from_file(std::string _path) {
    std::ifstream src(_path);

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(src, root)) {
        //std::cout << "ERROR: Failed to load json from \"" << _path << "\"\n"
        //          << "-------------------------\n"
        //          << reader.getFormattedErrorMessages()
        //          << "-------------------------" << std::endl;
    }
    src.close();

    return root;
}

std::string get_string_from_file(std::string _path) {
    std::ifstream src;
    src.open(_path);
    std::stringstream sstr;
    sstr << src.rdbuf();
    src.close();
    return sstr.str();
}

bool load_ply_from_file(std::string _path, std::vector<Vertex>& _vertVec, std::vector<glm::uvec3>& _faceVec) {
    std::ifstream src;
    std::string line;

    src.open(_path);

    bool error = false;
    std::getline(src, line);
    if (line != "ply") error = true;
    std::getline(src, line);
    if (line != "format ascii 1.0") error = true;
    if (error) {
        std::cout << "ERROR: Header of \"" << _path << "\" is invalid for an ascii PLY file" << std::endl;
        return true;
    }

    bool comment = true;
    while (comment) {
        std::getline(src, line);
        if (line.substr(0, 7) != "comment") comment = false;
    }

    if (line.substr(0, 15) != "element vertex ") {
        std::cout << "ERROR: Expected a vertex count in \"" << _path << "\" header" << std::endl;
        return true;
    }
    int vCount = std::atoi(line.substr(15, std::string::npos).c_str());

    std::getline(src, line);
    if (line != "property float x") error = true;
    std::getline(src, line);
    if (line != "property float y") error = true;
    std::getline(src, line);
    if (line != "property float z") error = true;
    std::getline(src, line);
    if (line != "property float nx") error = true;
    std::getline(src, line);
    if (line != "property float ny") error = true;
    std::getline(src, line);
    if (line != "property float nz") error = true;
    std::getline(src, line);
    if (line != "property float s") error = true;
    std::getline(src, line);
    if (line != "property float t") error = true;

    std::getline(src, line);
    if (line.substr(0, 13) != "element face ") {
        std::cout << "ERROR: Expected a face count in \"" << _path << "\" header" << std::endl;
        return true;
    }
    int fCount = std::atoi(line.substr(13, std::string::npos).c_str());

    std::getline(src, line);
    if (line != "property list uchar uint vertex_indices") {
        std::cout << "ERROR: Unexpected face list type in \"" << _path << "\" header" << std::endl;
        return true;
    }

    std::getline(src, line);
    if (line != "end_header") {
        std::cout << "ERROR: Expected end_header line in \"" << _path << "\"" << std::endl;
        return true;
    }

    float X, Y, Z, NX, NY, NZ, S, T;
    for (int i = 0; i < vCount; i++) {
        std::getline(src, line);

        int endPos = line.find_first_of(" ", 0);
        X = std::atof(line.substr(0, endPos).c_str());

        int startPos = line.find_first_not_of(" ", endPos);
        endPos = line.find_first_of(" ", startPos);
        Y = std::atof(line.substr(startPos, endPos).c_str());

        startPos = line.find_first_not_of(" ", endPos);
        endPos = line.find_first_of(" ", startPos);
        Z = std::atof(line.substr(startPos, endPos).c_str());

        startPos = line.find_first_not_of(" ", endPos);
        endPos = line.find_first_of(" ", startPos);
        NX = std::atof(line.substr(startPos, endPos).c_str());

        startPos = line.find_first_not_of(" ", endPos);
        endPos = line.find_first_of(" ", startPos);
        NY = std::atof(line.substr(startPos, endPos).c_str());

        startPos = line.find_first_not_of(" ", endPos);
        endPos = line.find_first_of(" ", startPos);
        NZ = std::atof(line.substr(startPos, endPos).c_str());

        startPos = line.find_first_not_of(" ", endPos);
        endPos = line.find_first_of(" ", startPos);
        S = std::atof(line.substr(startPos, endPos).c_str());

        startPos = line.find_first_not_of(" ", endPos);
        T = std::atof(line.substr(startPos, std::string::npos).c_str());

        _vertVec.push_back(Vertex(X, Y, Z, NX, NY, NZ, S, T));
    }

    int a, b, c;
    for (int i = 0; i < fCount; i++) {
        std::getline(src, line);

        int endPos = line.find_first_of(" ", 2);
        a = std::atoi(line.substr(2, endPos).c_str());

        int startPos = line.find_first_not_of(" ", endPos);
        endPos = line.find_first_of(" ", startPos);
        b = std::atoi(line.substr(startPos, endPos).c_str());

        startPos = line.find_first_not_of(" ", endPos);
        c = std::atoi(line.substr(startPos, std::string::npos).c_str());

        _faceVec.push_back({a, b, c});
    }

    return false;
}

}

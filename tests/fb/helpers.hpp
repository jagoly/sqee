#pragma once
// INLINE HEADER

#include <vector>

#include <SFML/Graphics.hpp>

namespace sqt {

enum class SIDE {
    north = 0,
    east = 1,
    south = 2,
    west = 3
};

enum class CORNER {
    nw = 0,
    ne = 1,
    se = 2,
    sw = 3
};

struct LineSeg {
    LineSeg(float _sX, float _sY, float _eX, float _eY) {
        sX = _sX;
        sY = _sY;
        eX = _eX;
        eY = _eY;
    }

    float sX, sY, eX, eY;
};

struct Rect {
    Rect(float _x, float _y, float _w, float _h) {
        x = _x;
        y = _y;
        w = _w;
        h = _h;

        co[0][0] = _x;
        co[0][1] = _y;
        co[1][0] = _x + _w;
        co[1][1] = _y;
        co[2][0] = _x;
        co[2][1] = _y + _h;
        co[3][0] = _x + _w;
        co[3][1] = _y + _h;

        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                ct[x][y] = co[x][y];

        rot = 0.d;
    }

    float x, y, w, h, rot, xo, yo;
    float tx, ty, tw, th;
    float co[4][2];
    float ct[4][2];

    void rotate_fresh(float _rot, float _xo, float _yo) {
        rot = _rot;
        float rs = sinf(rot);
        float rc = cosf(rot);

        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                ct[x][y] = co[x][y];

        for (float* p : ct) {
            p[0] -= _xo;
            p[1] -= _yo;

            float xnew = p[0] * rc - p[1] * rs;
            float ynew = p[0] * rs + p[1] * rc;

            p[0] = xnew + _xo;
            p[1] = ynew + _yo;
        }
    }

    sf::Vector2f get_vertex(CORNER _corner) {
        switch (_corner) {
        case CORNER::nw:
            return {ct[0][0], ct[0][1]}; break;
        case CORNER::ne:
            return {ct[1][0], ct[1][1]}; break;
        case CORNER::sw:
            return {ct[2][0], ct[2][1]}; break;
        case CORNER::se:
            return {ct[3][0], ct[3][1]}; break;
        }
    }

    LineSeg get_lineSeg(SIDE _side) {
        switch (_side) {
        case SIDE::north:
            return LineSeg(ct[0][0], ct[0][1], ct[1][0], ct[1][1]); break;
        case SIDE::east:
            return LineSeg(ct[1][0], ct[1][1], ct[2][0], ct[2][1]); break;
        case SIDE::south:
            return LineSeg(ct[2][0], ct[2][1], ct[3][0], ct[3][1]); break;
        case SIDE::west:
            return LineSeg(ct[3][0], ct[3][1], ct[0][0], ct[0][1]); break;
        }
    throw;
    }
};

bool lineSeg_intersects_rect(LineSeg lineSeg, Rect rect) {
    float minX = lineSeg.sX;
    float maxX = lineSeg.eX;

    if (lineSeg.sX > lineSeg.eX) {
        minX = lineSeg.eX;
        maxX = lineSeg.sX;
    }

    if (maxX > rect.x + rect.w) maxX = rect.x + rect.w;
    if (minX < rect.x) minX = rect.x;
    if (minX > maxX) return false;

    float minY = lineSeg.sY;
    float maxY = lineSeg.eY;
    float dx = lineSeg.eX - lineSeg.sX;

    if (abs(dx) > 0.0000001f) {
        float a = (lineSeg.eY - lineSeg.sY) / dx;
        float b = lineSeg.sY - a * lineSeg.sX;
        minY = a * minX + b;
        maxY = a * maxX + b;
    }

    if (minY > maxY) {
        float tmp = maxY;
        maxY = minY;
        minY = tmp;
    }

    if (maxY > rect.y + rect.h) maxY = rect.y + rect.h;
    if (minY < rect.y) minY = rect.y;
    if (minY > maxY) return false;

    return true;
}

bool oRect_intersects_rect(Rect oRect, Rect rect) {
    if (lineSeg_intersects_rect(oRect.get_lineSeg(SIDE::north), rect)) return true;
    if (lineSeg_intersects_rect(oRect.get_lineSeg(SIDE::east), rect)) return true;
    if (lineSeg_intersects_rect(oRect.get_lineSeg(SIDE::south), rect)) return true;
    if (lineSeg_intersects_rect(oRect.get_lineSeg(SIDE::west), rect)) return true;
    return false;
}

}

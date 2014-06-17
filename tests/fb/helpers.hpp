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
    a = 0,
    b = 1,
    c = 2,
    d = 3
};

struct LineSeg {
    LineSeg(double _sX, double _sY, double _eX, double _eY) {
        sX = _sX;
        sY = _sY;
        eX = _eX;
        eY = _eY;
    }

    double sX, sY, eX, eY;
};

struct Rect {
    Rect(double _x, double _y, double _w, double _h) {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
    }

    double x, y, w, h;

    double get_minX() {return x;}
    double get_minY() {return y;}
    double get_maxX() {return x+w;}
    double get_maxY() {return y+h;}
};

struct ORect {
    ORect(double _x, double _y, double _w, double _h) {
        x = _x;
        y = _y;
        w = _w;
        h = _h;

        c[0][0] = _x;
        c[0][1] = _y;
        c[1][0] = _x + _w;
        c[1][1] = _y;
        c[2][0] = _x;
        c[2][1] = _y + _h;
        c[3][0] = _x + _w;
        c[3][1] = _y + _h;

        rot = 0.d;
    }

    double x, y, w, h, rot, xo, yo;
    double tx, ty, tw, th;
    double c[4][2];

    void rotate(double _rot, double _xo, double _yo) {
        for (double* p : c) {
            p[0] = ((p[0] - _xo) * cos(_rot)) - ((_yo - p[1]) * sin(_rot)) + _xo;
            p[1] = ((p[1] - _yo) * cos(_rot)) - ((_xo - p[0]) * sin(_rot)) + _yo;
        }
    }

    LineSeg get_lineSeg(SIDE _side) {
        switch (_side) {
        case SIDE::north:
            return LineSeg(c[0][0], c[0][1], c[1][0], c[1][1]); break;
        case SIDE::east:
            return LineSeg(c[1][0], c[1][1], c[2][0], c[2][1]); break;
        case SIDE::south:
            return LineSeg(c[2][0], c[2][1], c[3][0], c[3][1]); break;
        case SIDE::west:
            return LineSeg(c[3][0], c[3][1], c[0][0], c[0][1]); break;
        }
    }
};

bool lineSeg_intersects_rect(LineSeg lineSeg, Rect rect) {
    // Find min and max X for the segment
    double minX = lineSeg.sX;
    double maxX = lineSeg.eX;

    if (lineSeg.sX > lineSeg.eX) {
        minX = lineSeg.eX;
        maxX = lineSeg.sX;
    }

    // Find the intersection of the segment's and rectangle's x-projections
    if (maxX > rect.get_maxX()) maxX = rect.get_maxX();
    if (minX < rect.get_minX()) minX = rect.get_minX();

    // If their projections do not intersect return false
    if (minX > maxX) return false;

    // Find corresponding min and max Y for min and max X we found before
    double minY = lineSeg.sY;
    double maxY = lineSeg.eY;

    double dx = lineSeg.eX - lineSeg.sX;

    if (abs(dx) > 0.0000001d) {
        double a = (lineSeg.eY - lineSeg.sY) / dx;
        double b = lineSeg.sY - a * lineSeg.sX;
        minY = a * minX + b;
        maxY = a * maxX + b;
    }

    if (minY > maxY) {
        double tmp = maxY;
        maxY = minY;
        minY = tmp;
    }

    // Find the intersection of the segment's and rectangle's y-projections
    if (maxY > rect.get_maxY()) maxY = rect.get_maxY();
    if (minY < rect.get_minY()) minY = rect.get_minY();

    // If Y-projections do not intersect return false
    if (minY > maxY) return false;

    return true;
}

bool oRect_intersects_rect(ORect oRect, Rect rect) {
    if (lineSeg_intersects_rect(oRect.get_lineSeg(SIDE::north), rect)) return true;
    if (lineSeg_intersects_rect(oRect.get_lineSeg(SIDE::east), rect)) return true;
    if (lineSeg_intersects_rect(oRect.get_lineSeg(SIDE::south), rect)) return true;
    if (lineSeg_intersects_rect(oRect.get_lineSeg(SIDE::west), rect)) return true;
    return false;
}

}

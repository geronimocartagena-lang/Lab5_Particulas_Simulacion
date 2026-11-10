#ifndef VECTOR2D_H
#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

struct Vector2D {
    double x;
    double y;

    Vector2D(double _x = 0.0, double _y = 0.0) : x(_x), y(_y) {}

    Vector2D operator+(const Vector2D& v) const {
        return Vector2D(x + v.x, y + v.y);
    }

    Vector2D operator-(const Vector2D& v) const {
        return Vector2D(x - v.x, y - v.y);
    }

    Vector2D operator*(double escalar) const {
        return Vector2D(x * escalar, y * escalar);
    }

    Vector2D operator/(double escalar) const {
        return Vector2D(x / escalar, y / escalar);
    }

    double magnitud() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2D normalizar() const {
        double mag = magnitud();
        if (mag == 0) return Vector2D(0, 0);
        return *this / mag;
    }
};

#endif
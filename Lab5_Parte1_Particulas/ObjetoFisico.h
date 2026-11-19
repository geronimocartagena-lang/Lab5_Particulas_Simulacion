#ifndef OBJETOFISICO_H
#define OBJETOFISICO_H

#include "Vector2D.h"

class ObjetoFisico {
protected:
    Vector2D posicion;

public:
    ObjetoFisico(Vector2D pos = Vector2D(0, 0)) : posicion(pos) {}
    virtual ~ObjetoFisico() {}

    Vector2D getPosicion() const { return posicion; }
    void setPosicion(const Vector2D& pos) { posicion = pos; }
};

#endif
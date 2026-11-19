#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "ObjetoFisico.h"
#include "Particula.h"

class Obstaculo : public ObjetoFisico {
private:
    double ancho;
    double alto;
    double e;

public:
    Obstaculo(Vector2D pos, double w, double h, double coefRest)
        : ObjetoFisico(pos), ancho(w), alto(h), e(coefRest) {}

    double getAncho() const { return ancho; }
    double getAlto() const { return alto; }
    double getE() const { return e; }

    bool detectarColision(const Particula& p) const {
        Vector2D pPos = p.getPosicion();
        double r = p.getRadio();

        return (pPos.x + r >= posicion.x && pPos.x - r <= posicion.x + ancho &&
                pPos.y + r >= posicion.y && pPos.y - r <= posicion.y + alto);
    }
};

#endif
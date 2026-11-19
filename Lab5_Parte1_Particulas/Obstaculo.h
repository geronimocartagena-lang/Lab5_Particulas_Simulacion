#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "ObjetoFisico.h"
#include "Particula.h"
#include <algorithm>
#include <cmath>

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

    // 0: Sin colisión | 1: Choque lateral (izq/der) | 2: Choque vertical (arriba/abajo)
    int detectarLadoColision(const Particula& p) const {
        Vector2D pPos = p.getPosicion();
        double r = p.getRadio();

        // Punto del rectángulo más cercano al centro de la partícula
        double cercanoX = std::max(posicion.x, std::min(pPos.x, posicion.x + ancho));
        double cercanoY = std::max(posicion.y, std::min(pPos.y, posicion.y + alto));

        double distX = pPos.x - cercanoX;
        double distY = pPos.y - cercanoY;

        if ((distX * distX + distY * distY) <= (r * r)) {
            if (std::abs(distX) > std::abs(distY)) return 1; // Impacto en pared vertical del cuadro
            else return 2;                                   // Impacto en pared horizontal del cuadro
        }
        return 0;
    }
};

#endif
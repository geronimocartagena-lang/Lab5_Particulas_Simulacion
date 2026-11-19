#ifndef PARTICULA_H
#define PARTICULA_H

#include "ObjetoFisico.h"
#include <cmath>

class Particula : public ObjetoFisico {
private:
    Vector2D velocidad;
    double masa;
    double radio;
    bool activa;

public:
    Particula(Vector2D pos, Vector2D vel, double m, double r)
        : ObjetoFisico(pos), velocidad(vel), masa(m), radio(r), activa(true) {}

    Vector2D getVelocidad() const { return velocidad; }
    void setVelocidad(const Vector2D& vel) { velocidad = vel; }

    double getMasa() const { return masa; }
    double getRadio() const { return radio; }
    bool isActiva() const { return activa; }
    void desactivar() { activa = false; }

    void actualizarPosicion(double dt) {
        posicion.x += velocidad.x * dt;
        posicion.y += velocidad.y * dt;
    }

    Particula fusionarCon(const Particula& otra) {
        double masaTotal = masa + otra.masa;
        Vector2D velCentroMasa = (velocidad * masa + otra.velocidad * otra.masa) / masaTotal;
        Vector2D posCentroMasa = (posicion * masa + otra.posicion * otra.masa) / masaTotal;
        double radioNuevo = std::sqrt(radio * radio + otra.radio * otra.radio);

        return Particula(posCentroMasa, velCentroMasa, masaTotal, radioNuevo);
    }
};

#endif
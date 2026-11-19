#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "ObjetoJuego.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Proyectil : public ObjetoJuego {
private:
    Vector2D velocidad;
    double masa;
    double radio;
    bool activo;

public:
    Proyectil(Vector2D pos, double m, double r, QGraphicsItem* parent = nullptr)
        : ObjetoJuego(pos, r * 2, r * 2, parent), velocidad(0, 0), masa(m), radio(r), activo(false) {}

    bool isActivo() const { return activo; }
    void desactivar() { activo = false; setVisible(false); }
    double getMasa() const { return masa; }
    double getRadio() const { return radio; }
    Vector2D getVelocidad() const { return velocidad; }

    void lanzar(Vector2D posInicial, double anguloGrados, double v0, bool haciaDerecha) {
        posicion = posInicial;
        double rad = anguloGrados * M_PI / 180.0;
        double dirX = haciaDerecha ? 1.0 : -1.0;

        velocidad.x = dirX * v0 * std::cos(rad);
        velocidad.y = -v0 * std::sin(rad);

        activo = true;
        setVisible(true);
        actualizarGrafico();
    }

    void actualizarPosicion(double dt, double g) {
        if (!activo) return;
        posicion.x += velocidad.x * dt;
        posicion.y += velocidad.y * dt + 0.5 * g * dt * dt;
        velocidad.y += g * dt;
        actualizarGrafico();
    }

    double calcularMomento() const {
        return masa * velocidad.magnitud();
    }

    bool rebotarBorde(double anchoEscenario, double altoSuelo) {
        if (posicion.x <= 0) {
            posicion.x = 0;
            velocidad.x = -velocidad.x;
        } else if (posicion.x + ancho >= anchoEscenario) {
            posicion.x = anchoEscenario - ancho;
            velocidad.x = -velocidad.x;
        }

        if (posicion.y <= 0) {
            posicion.y = 0;
            velocidad.y = -velocidad.y;
        } else if (posicion.y + alto >= altoSuelo) {
            posicion.y = altoSuelo - alto;
            velocidad.y = -velocidad.y * 0.75;
            velocidad.x *= 0.85;
            if (velocidad.magnitud() < 20.0) {
                return true;
            }
        }
        actualizarGrafico();
        return false;
    }

    void rebotarObstaculo(int lado, double e) {
        if (lado == 1) {
            velocidad.x = -e * velocidad.x;
            posicion.x += (velocidad.x > 0) ? 5.0 : -5.0;
        } else if (lado == 2) {
            velocidad.y = -e * velocidad.y;
            posicion.y += (velocidad.y > 0) ? 5.0 : -5.0;
        }
        actualizarGrafico();
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        if (!activo) return;
        // Diseño tipo esfera metálica oscura con brillo
        QRadialGradient grad(ancho * 0.35, alto * 0.35, ancho * 0.7);
        grad.setColorAt(0, QColor(255, 140, 0));
        grad.setColorAt(0.4, QColor(210, 40, 30));
        grad.setColorAt(1, QColor(40, 40, 45));

        painter->setBrush(grad);
        painter->setPen(QPen(Qt::black, 1.5));
        painter->drawEllipse(boundingRect());
    }
};

#endif // PROYECTIL_H
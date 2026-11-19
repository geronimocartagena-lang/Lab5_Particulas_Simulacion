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

        // En Qt el eje Y crece hacia abajo, por eso Vy inicial es negativa
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

    // Momento lineal: p = m * |v|
    double calcularMomento() const {
        return masa * velocidad.magnitud();
    }

    // Colisión elástica contra los límites del escenario (caja)
    // Retorna true si tocó el suelo demasiado lento (fin del turno)
    bool rebotarBorde(double anchoEscenario, double altoEscenario) {
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
        } else if (posicion.y + alto >= altoEscenario) {
            posicion.y = altoEscenario - alto;
            velocidad.y = -velocidad.y * 0.85; // Ligera amortiguación en suelo para evitar rebotes infinitos
            velocidad.x *= 0.90;
            if (velocidad.magnitud() < 15.0) {
                return true; // Detener proyectil si casi no se mueve
            }
        }
        actualizarGrafico();
        return false;
    }

    // Rebote inelástico contra infraestructura[cite: 4]
    void rebotarObstaculo(int lado, double e) {
        if (lado == 1) { // Impacto lateral (izq/der)
            velocidad.x = -e * velocidad.x;
            posicion.x += (velocidad.x > 0) ? 4.0 : -4.0;
        } else if (lado == 2) { // Impacto vertical (arriba/abajo)
            velocidad.y = -e * velocidad.y;
            posicion.y += (velocidad.y > 0) ? 4.0 : -4.0;
        }
        actualizarGrafico();
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        if (!activo) return;
        painter->setBrush(Qt::red);
        painter->setPen(QPen(Qt::darkRed, 2));
        painter->drawEllipse(boundingRect());
    }
};

#endif // PROYECTIL_H
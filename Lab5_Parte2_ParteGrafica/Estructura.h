#ifndef ESTRUCTURA_H
#define ESTRUCTURA_H

#include "ObjetoJuego.h"
#include "Proyectil.h"
#include <algorithm>
#include <cmath>

class Estructura : public ObjetoJuego {
private:
    double resistencia;
    double e;
    int jugadorDuenio;
    bool destruida;

public:
    Estructura(Vector2D pos, double w, double h, double res, double coefRest, int duenio, QGraphicsItem* parent = nullptr)
        : ObjetoJuego(pos, w, h, parent), resistencia(res), e(coefRest), jugadorDuenio(duenio), destruida(false) {}

    bool estaDestruida() const { return destruida; }
    double getE() const { return e; }
    int getJugadorDuenio() const { return jugadorDuenio; }

    // Fórmula de daño: Daño = factor_constante * masa * V_impacto[cite: 4]
    void recibirDanio(double momentoImpacto, double factor) {
        if (destruida) return;
        double danio = factor * momentoImpacto;
        resistencia -= danio;

        if (resistencia <= 0) {
            resistencia = 0;
            destruida = true;
            setVisible(false);
        }
        update();
    }

    // 0: Sin colisión | 1: Choque lateral | 2: Choque vertical
    int detectarColision(const Proyectil& p) const {
        if (destruida || !p.isActivo()) return 0;

        double r = p.getRadio();
        double cx = p.getPosicion().x + r;
        double cy = p.getPosicion().y + r;

        double cercanoX = std::max(posicion.x, std::min(cx, posicion.x + ancho));
        double cercanoY = std::max(posicion.y, std::min(cy, posicion.y + alto));

        double distX = cx - cercanoX;
        double distY = cy - cercanoY;

        if ((distX * distX + distY * distY) <= (r * r)) {
            return (std::abs(distX) > std::abs(distY)) ? 1 : 2;
        }
        return 0;
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        if (destruida) return;

        // Color diferente según resistencia inicial
        QColor colorRelleno = (resistencia > 100) ? QColor(255, 224, 189) : QColor(235, 235, 235);
        QColor colorBorde = (resistencia > 100) ? QColor(218, 165, 32) : Qt::darkGray;

        painter->setBrush(colorRelleno);
        painter->setPen(QPen(colorBorde, 2));
        painter->drawRect(boundingRect());

        // Mostrar el valor numérico de resistencia centrado
        painter->setPen(Qt::black);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(static_cast<int>(resistencia)));
    }
};

#endif // ESTRUCTURA_H
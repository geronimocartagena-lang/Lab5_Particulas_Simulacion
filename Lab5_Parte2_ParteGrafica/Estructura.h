#ifndef ESTRUCTURA_H
#define ESTRUCTURA_H

#include "ObjetoJuego.h"
#include "Proyectil.h"
#include <algorithm>
#include <cmath>

class Estructura : public ObjetoJuego {
private:
    double resistencia;
    double resistenciaMax;
    double e;
    int jugadorDuenio;
    bool destruida;

public:
    Estructura(Vector2D pos, double w, double h, double res, double coefRest, int duenio, QGraphicsItem* parent = nullptr)
        : ObjetoJuego(pos, w, h, parent), resistencia(res), resistenciaMax(res), e(coefRest), jugadorDuenio(duenio), destruida(false) {}

    bool estaDestruida() const { return destruida; }
    double getE() const { return e; }
    int getJugadorDuenio() const { return jugadorDuenio; }

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

        // Diseño renovado: Búnker azul para J1 y Búnker carmesí/piedra para J2
        QColor baseColor = (jugadorDuenio == 1) ? QColor(52, 101, 164) : QColor(164, 46, 46);
        if (resistenciaMax <= 100) baseColor = baseColor.lighter(125);

        painter->setBrush(baseColor);
        painter->setPen(QPen(QColor(30, 30, 35), 2));
        painter->drawRoundedRect(boundingRect(), 4, 4);

        // Textura de líneas internas tipo blindaje
        painter->setPen(QPen(QColor(255, 255, 255, 45), 1));
        painter->drawLine(4, alto / 2, ancho - 4, alto / 2);

        // Barra de salud superior
        double porcentaje = resistencia / resistenciaMax;
        painter->setBrush(Qt::darkGray);
        painter->setPen(Qt::NoPen);
        painter->drawRect(6, 6, ancho - 12, 5);
        painter->setBrush(porcentaje > 0.4 ? QColor(46, 204, 113) : QColor(231, 76, 60));
        painter->drawRect(6, 6, (ancho - 12) * porcentaje, 5);

        // Texto de resistencia en blanco con sombra
        painter->setPen(Qt::white);
        painter->setFont(QFont("Sans", 9, QFont::Bold));
        painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(static_cast<int>(std::ceil(resistencia))));
    }
};

#endif // ESTRUCTURA_H
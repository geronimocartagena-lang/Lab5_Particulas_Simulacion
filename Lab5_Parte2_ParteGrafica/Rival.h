#ifndef RIVAL_H
#define RIVAL_H

#include "ObjetoJuego.h"
#include "Proyectil.h"

class Rival : public ObjetoJuego {
private:
    int jugadorDuenio;
    bool eliminado;

public:
    Rival(Vector2D pos, double w, double h, int duenio, QGraphicsItem* parent = nullptr)
        : ObjetoJuego(pos, w, h, parent), jugadorDuenio(duenio), eliminado(false) {}

    bool estaEliminado() const { return eliminado; }
    int getJugadorDuenio() const { return jugadorDuenio; }

    void recibirImpacto() {
        eliminado = true;
        update();
    }

    bool detectarColision(const Proyectil& p) const {
        if (eliminado || !p.isActivo()) return false;
        QRectF rectRival(posicion.x, posicion.y, ancho, alto);
        QRectF rectProyectil(p.getPosicion().x, p.getPosicion().y, p.getAncho(), p.getAlto());
        return rectRival.intersects(rectProyectil);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setPen(QPen(eliminado ? Qt::red : Qt::black, 2));

        // Dibujo esquemático del muñeco (Stickman) como en la Figura 2
        double midX = ancho / 2.0;
        painter->drawEllipse(QPointF(midX, 12), 8, 8);               // Cabeza
        painter->drawLine(QPointF(midX, 20), QPointF(midX, 42));     // Torso
        painter->drawLine(QPointF(midX - 12, 28), QPointF(midX + 12, 28)); // Brazos
        painter->drawLine(QPointF(midX, 42), QPointF(midX - 10, 58)); // Pierna izq
        painter->drawLine(QPointF(midX, 42), QPointF(midX + 10, 58)); // Pierna der

        painter->setFont(QFont("Arial", 8));
        painter->drawText(QRectF(0, 60, ancho, 15), Qt::AlignCenter, eliminado ? "ELIMINADO" : "Rival");
    }
};

#endif // RIVAL_H
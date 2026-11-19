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
        double midX = ancho / 2.0;
        QColor colorTraje = (jugadorDuenio == 1) ? QColor(26, 95, 180) : QColor(192, 28, 40);

        painter->setBrush(eliminado ? Qt::gray : colorTraje);
        painter->setPen(QPen(Qt::black, 1.5));

        // Torso
        painter->drawRoundedRect(QRectF(midX - 9, 28, 18, 26), 4, 4);
        // Cabeza + Casco
        painter->setBrush(QColor(245, 203, 167));
        painter->drawEllipse(QPointF(midX, 18), 7, 7);
        painter->setBrush(eliminado ? Qt::darkGray : colorTraje.darker(120));
        painter->drawChord(QRectF(midX - 8, 10, 16, 12), 0, 180 * 16);

        // Piernas
        painter->setPen(QPen(Qt::black, 2.5));
        painter->drawLine(QPointF(midX - 4, 54), QPointF(midX - 6, 74));
        painter->drawLine(QPointF(midX + 4, 54), QPointF(midX + 6, 74));
    }
};

#endif // RIVAL_H
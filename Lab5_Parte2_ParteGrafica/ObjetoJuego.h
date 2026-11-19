#ifndef OBJETOJUEGO_H
#define OBJETOJUEGO_H

#include <QGraphicsItem>
#include <QPainter>
#include "Vector2D.h"

class ObjetoJuego : public QGraphicsItem {
protected:
    Vector2D posicion;
    double ancho;
    double alto;

public:
    ObjetoJuego(Vector2D pos, double w, double h, QGraphicsItem* parent = nullptr)
        : QGraphicsItem(parent), posicion(pos), ancho(w), alto(h) {
        setPos(posicion.x, posicion.y);
    }

    virtual ~ObjetoJuego() {}

    Vector2D getPosicion() const { return posicion; }
    double getAncho() const { return ancho; }
    double getAlto() const { return alto; }

    void setPosicion(const Vector2D& pos) {
        posicion = pos;
        actualizarGrafico();
    }

    void actualizarGrafico() {
        setPos(posicion.x, posicion.y);
        update();
    }

    QRectF boundingRect() const override {
        return QRectF(0, 0, ancho, alto);
    }
};

#endif // OBJETOJUEGO_H
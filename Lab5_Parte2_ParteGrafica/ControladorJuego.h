#ifndef CONTROLADORJUEGO_H
#define CONTROLADORJUEGO_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <vector>
#include "Proyectil.h"
#include "Estructura.h"
#include "Rival.h"

class ControladorJuego : public QObject {
    Q_OBJECT
private:
    QGraphicsScene* escena;
    QTimer* timer;

    int turnoActual;
    double dt;
    double gravedad;
    double factorDanio;
    double tiempoVuelo;
    double cooldownGolpe;

    double anchoEscenario;
    double altoEscenario;
    double nivelSuelo;

    Proyectil* proyectilActivo;
    QGraphicsRectItem* canonJ1;
    QGraphicsRectItem* canonJ2;
    std::vector<Estructura*> estructuras;
    std::vector<Rival*> rivales;

    void construirEscenario();

public:
    explicit ControladorJuego(QGraphicsScene* scene, QObject* parent = nullptr);

    int getTurnoActual() const { return turnoActual; }
    bool hayDisparoEnCurso() const { return proyectilActivo && proyectilActivo->isActivo(); }

    void actualizarAnguloCanon(double angulo);
    void iniciarTurno(double angulo, double velocidad);
    void cambiarTurno();
    int verificarVictoria() const;
    void reiniciarJuego();

private slots:
    void actualizarFisica();

signals:
    void turnoCambiado(int nuevoTurno);
    void juegoTerminado(int ganador);
};

#endif // CONTROLADORJUEGO_H
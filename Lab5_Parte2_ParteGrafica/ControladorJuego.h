#ifndef CONTROLADORJUEGO_H
#define CONTROLADORJUEGO_H

#include <QObject>
#include <QGraphicsScene>
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

    int turnoActual; // 1 = Jugador Izquierda | 2 = Jugador Derecha
    double dt;
    double gravedad;
    double factorDanio;

    double anchoEscenario;
    double altoEscenario;

    Proyectil* proyectilActivo;
    std::vector<Estructura*> estructuras;
    std::vector<Rival*> rivales;

    void construirEscenario();

public:
    explicit ControladorJuego(QGraphicsScene* scene, QObject* parent = nullptr);

    int getTurnoActual() const { return turnoActual; }
    bool hayDisparoEnCurso() const { return proyectilActivo->isActivo(); }

    void iniciarTurno(double angulo, double velocidad);
    void cambiarTurno();
    int verificarVictoria() const;

private slots:
    void actualizarFisica();

signals:
    void turnoCambiado(int nuevoTurno);
    void juegoTerminado(int ganador);
};

#endif // CONTROLADORJUEGO_H
#include "ControladorJuego.h"
#include <QLinearGradient>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ControladorJuego::ControladorJuego(QGraphicsScene* scene, QObject* parent)
    : QObject(parent), escena(scene), turnoActual(1), dt(0.02), gravedad(180.0),
    factorDanio(0.03), tiempoVuelo(0.0), cooldownGolpe(0.0),
    anchoEscenario(920.0), altoEscenario(470.0), nivelSuelo(435.0),
    proyectilActivo(nullptr), canonJ1(nullptr), canonJ2(nullptr) {

    escena->setSceneRect(0, 0, anchoEscenario, altoEscenario);
    construirEscenario();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ControladorJuego::actualizarFisica);
    timer->start(16);
}

void ControladorJuego::construirEscenario() {
    escena->clear();
    estructuras.clear();
    rivales.clear();

    // Fondo cielo + terreno
    QLinearGradient cielo(0, 0, 0, nivelSuelo);
    cielo.setColorAt(0, QColor(225, 240, 255));
    cielo.setColorAt(1, QColor(248, 251, 255));
    escena->addRect(0, 0, anchoEscenario, nivelSuelo, QPen(Qt::NoPen), QBrush(cielo));

    escena->addRect(0, nivelSuelo, anchoEscenario, altoEscenario - nivelSuelo,
                    QPen(QColor(60, 90, 50), 2), QBrush(QColor(115, 165, 95)));
    escena->addRect(0, 0, anchoEscenario, altoEscenario, QPen(QColor(40, 45, 55), 3));

    // Torres laterales
    escena->addRect(5, 205, 38, nivelSuelo - 205, QPen(Qt::black, 1.5), QBrush(QColor(130, 140, 150)));
    escena->addRect(anchoEscenario - 43, 205, 38, nivelSuelo - 205, QPen(Qt::black, 1.5), QBrush(QColor(130, 140, 150)));

    // Cañones orientables (tubo largo con pivote en la base)
    canonJ1 = escena->addRect(0, -6, 36, 12, QPen(Qt::black, 1.5), QBrush(QColor(26, 95, 180)));
    canonJ1->setPos(24, 192);
    canonJ1->setRotation(-45.0);

    canonJ2 = escena->addRect(-36, -6, 36, 12, QPen(Qt::black, 1.5), QBrush(QColor(192, 28, 40)));
    canonJ2->setPos(anchoEscenario - 24, 192);
    canonJ2->setRotation(45.0);

    // Bases cuadradas fijas sobre las torres (pivote visual)
    escena->addRect(12, 181, 24, 24, QPen(Qt::black, 2), QBrush(QColor(45, 55, 70)));
    escena->addRect(anchoEscenario - 36, 181, 24, 24, QPen(Qt::black, 2), QBrush(QColor(45, 55, 70)));

    // --- FORTALEZA JUGADOR 1 ---
    estructuras.push_back(new Estructura(Vector2D(65, 305), 55, 130, 200, 0.6, 1));
    estructuras.push_back(new Estructura(Vector2D(225, 305), 55, 130, 200, 0.6, 1));
    estructuras.push_back(new Estructura(Vector2D(65, 260), 215, 45, 100, 0.6, 1));
    rivales.push_back(new Rival(Vector2D(152, 357), 40, 78, 1));

    // --- FORTALEZA JUGADOR 2 ---
    estructuras.push_back(new Estructura(Vector2D(640, 305), 55, 130, 200, 0.6, 2));
    estructuras.push_back(new Estructura(Vector2D(800, 305), 55, 130, 200, 0.6, 2));
    estructuras.push_back(new Estructura(Vector2D(640, 260), 215, 45, 100, 0.6, 2));
    rivales.push_back(new Rival(Vector2D(727, 357), 40, 78, 2));

    for (auto* est : estructuras) escena->addItem(est);
    for (auto* riv : rivales) escena->addItem(riv);

    proyectilActivo = new Proyectil(Vector2D(0, 0), 2.5, 10.0);
    escena->addItem(proyectilActivo);
}

void ControladorJuego::actualizarAnguloCanon(double angulo) {
    if (turnoActual == 1 && canonJ1) {
        canonJ1->setRotation(-angulo); // Negativo en Qt apunta hacia arriba
    } else if (turnoActual == 2 && canonJ2) {
        canonJ2->setRotation(angulo);  // Positivo en Qt apunta hacia arriba y a la izquierda
    }
}

void ControladorJuego::reiniciarJuego() {
    turnoActual = 1;
    tiempoVuelo = 0.0;
    construirEscenario();
    emit turnoCambiado(turnoActual);
}

void ControladorJuego::iniciarTurno(double angulo, double velocidad) {
    if (proyectilActivo->isActivo() || verificarVictoria() != 0) return;

    tiempoVuelo = 0.0;
    cooldownGolpe = 0.0;
    actualizarAnguloCanon(angulo);

    double rad = angulo * M_PI / 180.0;
    double largoTubo = 36.0;
    double r = proyectilActivo->getRadio();

    if (turnoActual == 1) {
        // Calcular punta exacta del cañón izquierdo
        double puntaX = 24.0 + largoTubo * std::cos(rad) - r;
        double puntaY = 192.0 - largoTubo * std::sin(rad) - r;
        proyectilActivo->lanzar(Vector2D(puntaX, puntaY), angulo, velocidad, true);
    } else {
        // Calcular punta exacta del cañón derecho
        double puntaX = (anchoEscenario - 24.0) - largoTubo * std::cos(rad) - r;
        double puntaY = 192.0 - largoTubo * std::sin(rad) - r;
        proyectilActivo->lanzar(Vector2D(puntaX, puntaY), angulo, velocidad, false);
    }
}

void ControladorJuego::actualizarFisica() {
    if (!proyectilActivo || !proyectilActivo->isActivo()) return;

    tiempoVuelo += dt;
    if (cooldownGolpe > 0) cooldownGolpe -= dt;

    if (tiempoVuelo >= 6.0) {
        proyectilActivo->desactivar();
        cambiarTurno();
        return;
    }

    proyectilActivo->actualizarPosicion(dt, gravedad);

    for (auto* riv : rivales) {
        if (riv->getJugadorDuenio() != turnoActual && riv->detectarColision(*proyectilActivo)) {
            riv->recibirImpacto();
            proyectilActivo->desactivar();
            int ganador = verificarVictoria();
            if (ganador != 0) {
                emit juegoTerminado(ganador);
                return;
            }
        }
    }

    for (auto* est : estructuras) {
        int lado = est->detectarColision(*proyectilActivo);
        if (lado != 0) {
            if (est->getJugadorDuenio() != turnoActual && cooldownGolpe <= 0) {
                double momento = proyectilActivo->calcularMomento();
                est->recibirDanio(momento, factorDanio);
                cooldownGolpe = 0.25;
            }
            proyectilActivo->rebotarObstaculo(lado, est->getE());
            break;
        }
    }

    bool detenido = proyectilActivo->rebotarBorde(anchoEscenario, nivelSuelo);
    if (detenido) {
        proyectilActivo->desactivar();
        cambiarTurno();
    }
}

void ControladorJuego::cambiarTurno() {
    turnoActual = (turnoActual == 1) ? 2 : 1;
    emit turnoCambiado(turnoActual);
}

int ControladorJuego::verificarVictoria() const {
    for (const auto* riv : rivales) {
        if (riv->estaEliminado()) {
            return (riv->getJugadorDuenio() == 1) ? 2 : 1;
        }
    }
    return 0;
}
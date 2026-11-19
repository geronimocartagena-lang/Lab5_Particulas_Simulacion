#include "ControladorJuego.h"
#include <QGraphicsRectItem>

ControladorJuego::ControladorJuego(QGraphicsScene* scene, QObject* parent)
    : QObject(parent), escena(scene), turnoActual(1), dt(0.02), gravedad(180.0),
    factorDanio(0.08), anchoEscenario(900.0), altoEscenario(450.0) {

    escena->setSceneRect(0, 0, anchoEscenario, altoEscenario);
    construirEscenario();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ControladorJuego::actualizarFisica);
    timer->start(16); // ~60 FPS
}

void ControladorJuego::construirEscenario() {
    // Borde exterior de la caja
    escena->addRect(0, 0, anchoEscenario, altoEscenario, QPen(Qt::black, 3));

    // Plataformas de los cañones (Izquierda y Derecha)
    escena->addRect(0, 210, 45, 12, QPen(Qt::black), QBrush(Qt::lightGray));
    escena->addRect(anchoEscenario - 45, 210, 45, 12, QPen(Qt::black), QBrush(Qt::lightGray));

    // --- BASE JUGADOR 1 (Izquierda) ---
    // Columnas laterales (Resistencia 200) y Techo (Resistencia 100)
    estructuras.push_back(new Estructura(Vector2D(60, 320), 60, 130, 200, 0.6, 1));
    estructuras.push_back(new Estructura(Vector2D(220, 320), 60, 130, 200, 0.6, 1));
    estructuras.push_back(new Estructura(Vector2D(60, 270), 220, 50, 100, 0.6, 1));
    rivales.push_back(new Rival(Vector2D(150, 370), 40, 80, 1));

    // --- BASE JUGADOR 2 (Derecha) ---
    estructuras.push_back(new Estructura(Vector2D(620, 320), 60, 130, 200, 0.6, 2));
    estructuras.push_back(new Estructura(Vector2D(780, 320), 60, 130, 200, 0.6, 2));
    estructuras.push_back(new Estructura(Vector2D(620, 270), 220, 50, 100, 0.6, 2));
    rivales.push_back(new Rival(Vector2D(710, 370), 40, 80, 2));

    for (auto* est : estructuras) escena->addItem(est);
    for (auto* riv : rivales) escena->addItem(riv);

    // Proyectil compartido
    proyectilActivo = new Proyectil(Vector2D(0, 0), 2.5, 10.0);
    escena->addItem(proyectilActivo);
}

void ControladorJuego::iniciarTurno(double angulo, double velocidad) {
    if (proyectilActivo->isActivo() || verificarVictoria() != 0) return;

    if (turnoActual == 1) {
        proyectilActivo->lanzar(Vector2D(35, 185), angulo, velocidad, true);
    } else {
        proyectilActivo->lanzar(Vector2D(anchoEscenario - 55, 185), angulo, velocidad, false);
    }
}

void ControladorJuego::actualizarFisica() {
    if (!proyectilActivo->isActivo()) return;

    proyectilActivo->actualizarPosicion(dt, gravedad);

    // 1. Verificar colisión contra rivales (Condición de victoria)[cite: 5]
    for (auto* riv : rivales) {
        if (riv->detectarColision(*proyectilActivo)) {
            riv->recibirImpacto();
            proyectilActivo->desactivar();
            int ganador = verificarVictoria();
            if (ganador != 0) {
                emit juegoTerminado(ganador);
                return;
            }
        }
    }

    // 2. Verificar colisión contra estructuras (Rebote inelástico + Daño por momento lineal)[cite: 4]
    for (auto* est : estructuras) {
        int lado = est->detectarColision(*proyectilActivo);
        if (lado != 0) {
            double momento = proyectilActivo->calcularMomento();
            est->recibirDanio(momento, factorDanio);
            proyectilActivo->rebotarObstaculo(lado, est->getE());
            break;
        }
    }

    // 3. Verificar colisión contra paredes de la caja (Rebote elástico)[cite: 4]
    bool detenido = proyectilActivo->rebotarBorde(anchoEscenario, altoEscenario);
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
            // Si murió el rival del jugador 1, gana el 2 (y viceversa)
            return (riv->getJugadorDuenio() == 1) ? 2 : 1;
        }
    }
    return 0;
}
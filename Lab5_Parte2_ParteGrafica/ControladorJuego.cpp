#include "ControladorJuego.h"
#include <QGraphicsRectItem>

ControladorJuego::ControladorJuego(QGraphicsScene* scene, QObject* parent)
    : QObject(parent), escena(scene), turnoActual(1), dt(0.02), gravedad(180.0),
    factorDanio(0.08), anchoEscenario(900.0), altoEscenario(450.0), proyectilActivo(nullptr) {

    escena->setSceneRect(0, 0, anchoEscenario, altoEscenario);
    escena->setBackgroundBrush(Qt::white); // 1. Fondo blanco
    construirEscenario();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ControladorJuego::actualizarFisica);
    timer->start(16);
}

void ControladorJuego::construirEscenario() {
    escena->clear();
    estructuras.clear();
    rivales.clear();

    // Borde exterior de la caja
    escena->addRect(0, 0, anchoEscenario, altoEscenario, QPen(Qt::black, 3));

    // Plataformas de los cañones
    escena->addRect(0, 210, 45, 12, QPen(Qt::black), QBrush(Qt::lightGray));
    escena->addRect(anchoEscenario - 45, 210, 45, 12, QPen(Qt::black), QBrush(Qt::lightGray));

    // 2. Cuadrados lanzadores (de donde sale la bola)
    escena->addRect(10, 185, 25, 25, QPen(Qt::black, 2), QBrush(QColor(26, 95, 180))); // Cuadrado Jugador 1
    escena->addRect(anchoEscenario - 35, 185, 25, 25, QPen(Qt::black, 2), QBrush(QColor(192, 28, 40))); // Cuadrado Jugador 2

    // --- BASE JUGADOR 1 (Izquierda) ---
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

    // Proyectil
    proyectilActivo = new Proyectil(Vector2D(0, 0), 2.5, 10.0);
    escena->addItem(proyectilActivo);
}

void ControladorJuego::reiniciarJuego() {
    turnoActual = 1;
    construirEscenario();
    emit turnoCambiado(turnoActual);
}

void ControladorJuego::iniciarTurno(double angulo, double velocidad) {
    if (proyectilActivo->isActivo() || verificarVictoria() != 0) return;

    if (turnoActual == 1) {
        // Sale desde el borde del cuadrado azul izquierdo
        proyectilActivo->lanzar(Vector2D(38, 185), angulo, velocidad, true);
    } else {
        // Sale desde el borde del cuadrado rojo derecho
        proyectilActivo->lanzar(Vector2D(anchoEscenario - 58, 185), angulo, velocidad, false);
    }
}

void ControladorJuego::actualizarFisica() {
    if (!proyectilActivo || !proyectilActivo->isActivo()) return;

    proyectilActivo->actualizarPosicion(dt, gravedad);

    // 1. Verificar colisión contra rivales (Solo daña al rival contrario)
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

    // 2. Verificar colisión contra estructuras
    for (auto* est : estructuras) {
        int lado = est->detectarColision(*proyectilActivo);
        if (lado != 0) {
            // 3. Solo inflige daño si la estructura pertenece al jugador rival
            if (est->getJugadorDuenio() != turnoActual) {
                double momento = proyectilActivo->calcularMomento();
                est->recibirDanio(momento, factorDanio);
            }
            proyectilActivo->rebotarObstaculo(lado, est->getE());
            break;
        }
    }

    // 3. Colisión contra paredes de la caja
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
            return (riv->getJugadorDuenio() == 1) ? 2 : 1;
        }
    }
    return 0;
}
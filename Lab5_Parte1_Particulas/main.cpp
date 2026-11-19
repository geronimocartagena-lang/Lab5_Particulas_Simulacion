#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "Particula.h"
#include "Obstaculo.h"

// Dimensiones del área cerrada (Caja)[cite: 3]
const double CAJA_ANCHO = 100.0;
const double CAJA_ALTO = 100.0;
const double DT = 0.01;      // Paso de tiempo en segundos[cite: 1, 2]
const double T_MAX = 10.0;    // Duración total de la simulación

// Rebote elástico contra paredes de la caja[cite: 3, 4]
void manejarColisionParedes(Particula& p) {
    Vector2D pos = p.getPosicion();
    Vector2D vel = p.getVelocidad();
    double r = p.getRadio();

    if (pos.x - r <= 0) {
        pos.x = r;
        vel.x = -vel.x;
    } else if (pos.x + r >= CAJA_ANCHO) {
        pos.x = CAJA_ANCHO - r;
        vel.x = -vel.x;
    }

    if (pos.y - r <= 0) {
        pos.y = r;
        vel.y = -vel.y;
    } else if (pos.y + r >= CAJA_ALTO) {
        pos.y = CAJA_ALTO - r;
        vel.y = -vel.y;
    }

    p.setPosicion(pos);
    p.setVelocidad(vel);
}

// Rebote inelástico contra obstáculos estáticos[cite: 3]
void manejarColisionObstaculos(Particula& p, const std::vector<Obstaculo>& obstaculos) {
    for (const auto& obs : obstaculos) {
        if (obs.detectarColision(p)) {
            Vector2D vel = p.getVelocidad();
            double e = obs.getE();

            // Inversión con pérdida de energía (coeficiente e)[cite: 3]
            vel.x = -e * vel.x;
            vel.y = -e * vel.y;
            p.setVelocidad(vel);
            break;
        }
    }
}

int main() {
    // 1. Inicializar los 4 obstáculos estáticos dentro de la caja[cite: 3, 4]
    std::vector<Obstaculo> obstaculos = {
        Obstaculo(Vector2D(20, 20), 10, 10, 0.8),
        Obstaculo(Vector2D(70, 20), 10, 10, 0.8),
        Obstaculo(Vector2D(20, 70), 10, 10, 0.8),
        Obstaculo(Vector2D(70, 70), 10, 10, 0.8)
    };

    // 2. Inicializar al menos 4 partículas simultáneas[cite: 4]
    std::vector<Particula> particulas = {
        Particula(Vector2D(10, 10), Vector2D(15, 12), 2.0, 1.5),
        Particula(Vector2D(90, 10), Vector2D(-10, 18), 1.5, 1.2),
        Particula(Vector2D(10, 90), Vector2D(18, -14), 3.0, 1.8),
        Particula(Vector2D(90, 90), Vector2D(-12, -15), 2.5, 1.6)
    };

    // 3. Crear archivo de salida TXT[cite: 1, 4]
    std::ofstream archivo("simulacion_particulas.txt");
    archivo << "# Tiempo | Particula_ID | Pos_X | Pos_Y\n";

    double t = 0.0;
    while (t <= T_MAX) {
        // Escribir estado actual en el archivo[cite: 4]
        for (size_t i = 0; i < particulas.size(); ++i) {
            if (particulas[i].isActiva()) {
                Vector2D pos = particulas[i].getPosicion();
                archivo << std::fixed << std::setprecision(3)
                        << t << " " << i << " " << pos.x << " " << pos.y << "\n";
            }
        }

        // Actualizar posiciones[cite: 2, 4]
        for (auto& p : particulas) {
            if (p.isActiva()) {
                p.actualizarPosicion(DT);
                manejarColisionParedes(p);
                manejarColisionObstaculos(p, obstaculos);
            }
        }

        // Verificar colisiones entre partículas (Completamente inelásticas -> Fusión)[cite: 3]
        for (size_t i = 0; i < particulas.size(); ++i) {
            if (!particulas[i].isActiva()) continue;
            for (size_t j = i + 1; j < particulas.size(); ++j) {
                if (!particulas[j].isActiva()) continue;

                Vector2D diff = particulas[i].getPosicion() - particulas[j].getPosicion();
                double dist = diff.magnitud();
                double sumaRadios = particulas[i].getRadio() + particulas[j].getRadio();

                if (dist <= sumaRadios) {
                    // Se fusionan produciendo una nueva partícula[cite: 3]
                    Particula nueva = particulas[i].fusionarCon(particulas[j]);
                    particulas[i].desactivar();
                    particulas[j].desactivar();
                    particulas.push_back(nueva);
                    std::cout << "[COLISIÓN PARTÍCULAS] Instante t=" << t
                              << "s: Fusión efectuada." << std::endl;
                    break;
                }
            }
        }

        t += DT;
    }

    archivo.close();
    std::cout << "Simulación finalizada. Datos guardados en 'simulacion_particulas.txt'." << std::endl;
    return 0;
}
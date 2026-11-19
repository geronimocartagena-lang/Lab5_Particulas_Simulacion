#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "Particula.h"
#include "Obstaculo.h"

const double CAJA_ANCHO = 100.0;
const double CAJA_ALTO = 100.0;
const double DT = 0.01;
const double T_MAX = 15.0; // Más tiempo para observar múltiples rebotes y fusiones

void manejarColisionParedes(Particula& p, size_t id, double t, std::ofstream& archivo) {
    Vector2D pos = p.getPosicion();
    Vector2D vel = p.getVelocidad();
    double r = p.getRadio();
    bool choco = false;

    if (pos.x - r <= 0) {
        pos.x = r;
        vel.x = -vel.x;
        choco = true;
    } else if (pos.x + r >= CAJA_ANCHO) {
        pos.x = CAJA_ANCHO - r;
        vel.x = -vel.x;
        choco = true;
    }

    if (pos.y - r <= 0) {
        pos.y = r;
        vel.y = -vel.y;
        choco = true;
    } else if (pos.y + r >= CAJA_ALTO) {
        pos.y = CAJA_ALTO - r;
        vel.y = -vel.y;
        choco = true;
    }

    if (choco) {
        p.setPosicion(pos);
        p.setVelocidad(vel);
        archivo << "# COLISION_PARED | t=" << t << "s | Particula=" << id << "\n";
    }
}

void manejarColisionObstaculos(Particula& p, size_t id, const std::vector<Obstaculo>& obstaculos, double t, std::ofstream& archivo) {
    for (size_t k = 0; k < obstaculos.size(); ++k) {
        int lado = obstaculos[k].detectarLadoColision(p);
        if (lado != 0) {
            Vector2D vel = p.getVelocidad();
            Vector2D pos = p.getPosicion();
            double e = obstaculos[k].getE();

            if (lado == 1) { // Cara izquierda o derecha -> solo invierte Vx
                vel.x = -e * vel.x;
                pos.x += (vel.x > 0) ? 0.5 : -0.5; // Evitar que quede atrapada adentro
            } else if (lado == 2) { // Cara superior o inferior -> solo invierte Vy
                vel.y = -e * vel.y;
                pos.y += (vel.y > 0) ? 0.5 : -0.5;
            }

            p.setPosicion(pos);
            p.setVelocidad(vel);
            archivo << "# COLISION_OBSTACULO | t=" << t << "s | Particula=" << id << " | Obstaculo=" << k << "\n";
            break;
        }
    }
}

int main() {
    std::vector<Obstaculo> obstaculos = {
        Obstaculo(Vector2D(25, 25), 10, 10, 0.8),
        Obstaculo(Vector2D(65, 25), 10, 10, 0.8),
        Obstaculo(Vector2D(25, 65), 10, 10, 0.8),
        Obstaculo(Vector2D(65, 65), 10, 10, 0.8)
    };

    // Velocidades iniciales configuradas para rebotar en ángulos y cruzarse en el centro
    std::vector<Particula> particulas = {
        Particula(Vector2D(10, 15), Vector2D(18, 12), 2.0, 1.5),
        Particula(Vector2D(90, 15), Vector2D(-16, 14), 1.5, 1.5),
        Particula(Vector2D(15, 90), Vector2D(14, -18), 3.0, 1.8),
        Particula(Vector2D(85, 85), Vector2D(-15, -13), 2.5, 1.6)
    };

    std::ofstream archivo("simulacion_particulas.txt");
    archivo << "# Tiempo | Particula_ID | Pos_X | Pos_Y\n";

    double t = 0.0;
    while (t <= T_MAX) {
        for (size_t i = 0; i < particulas.size(); ++i) {
            if (particulas[i].isActiva()) {
                Vector2D pos = particulas[i].getPosicion();
                archivo << std::fixed << std::setprecision(3)
                        << t << " " << i << " " << pos.x << " " << pos.y << "\n";
            }
        }

        for (size_t i = 0; i < particulas.size(); ++i) {
            if (particulas[i].isActiva()) {
                particulas[i].actualizarPosicion(DT);
                manejarColisionParedes(particulas[i], i, t, archivo);
                manejarColisionObstaculos(particulas[i], i, obstaculos, t, archivo);
            }
        }

        for (size_t i = 0; i < particulas.size(); ++i) {
            if (!particulas[i].isActiva()) continue;
            for (size_t j = i + 1; j < particulas.size(); ++j) {
                if (!particulas[j].isActiva()) continue;

                Vector2D diff = particulas[i].getPosicion() - particulas[j].getPosicion();
                if (diff.magnitud() <= (particulas[i].getRadio() + particulas[j].getRadio())) {
                    Particula nueva = particulas[i].fusionarCon(particulas[j]);
                    particulas[i].desactivar();
                    particulas[j].desactivar();
                    particulas.push_back(nueva);
                    archivo << "# COLISION_PARTICULAS (FUSION) | t=" << t << "s | P" << i << " + P" << j << " -> P" << (particulas.size() - 1) << "\n";
                    break;
                }
            }
        }
        t += DT;
    }

    archivo.close();
    std::cout << "Simulación completada." << std::endl;
    return 0;
}
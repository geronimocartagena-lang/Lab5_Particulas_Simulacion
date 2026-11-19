#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include "ControladorJuego.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    QGraphicsScene* escena;
    QGraphicsView* vista;
    ControladorJuego* controlador;

    QLabel* lblTurno;
    QDoubleSpinBox* spinAngulo;
    QDoubleSpinBox* spinVelocidad;
    QPushButton* btnDisparar;
    QPushButton* btnReiniciar; // 4. Nuevo botón de reinicio

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onDispararClicked();
    void onReiniciarClicked();
    void onTurnoCambiado(int turno);
    void onJuegoTerminado(int ganador);
};

#endif // MAINWINDOW_H
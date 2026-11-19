#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Práctica 5 - Juego de Estrategia por Turnos");
    resize(950, 560);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(central);
    QHBoxLayout* barraControles = new QHBoxLayout();

    lblTurno = new QLabel("Turno: JUGADOR 1 (Izquierda)");
    lblTurno->setStyleSheet("font-weight: bold; font-size: 14px; color: #1a5fb4;");

    QLabel* lblAng = new QLabel("Ángulo (°):");
    spinAngulo = new QDoubleSpinBox();
    spinAngulo->setRange(0.0, 89.0);
    spinAngulo->setValue(45.0);

    QLabel* lblVel = new QLabel("Velocidad Inicial:");
    spinVelocidad = new QDoubleSpinBox();
    spinVelocidad->setRange(50.0, 900.0);
    spinVelocidad->setValue(420.0);

    btnDisparar = new QPushButton("¡Lanzar Proyectil!");
    btnDisparar->setStyleSheet("background-color: #26a269; color: white; font-weight: bold; padding: 6px 14px;");

    btnReiniciar = new QPushButton("Reiniciar Partida");
    btnReiniciar->setStyleSheet("background-color: #e66100; color: white; font-weight: bold; padding: 6px 14px;");
    btnReiniciar->setVisible(false);

    barraControles->addWidget(lblTurno);
    barraControles->addStretch();
    barraControles->addWidget(lblAng);
    barraControles->addWidget(spinAngulo);
    barraControles->addWidget(lblVel);
    barraControles->addWidget(spinVelocidad);
    barraControles->addWidget(btnDisparar);
    barraControles->addWidget(btnReiniciar);

    escena = new QGraphicsScene(this);
    vista = new QGraphicsView(escena);
    vista->setRenderHint(QPainter::Antialiasing);

    layoutPrincipal->addLayout(barraControles);
    layoutPrincipal->addWidget(vista);

    controlador = new ControladorJuego(escena, this);

    // Conectar el cambio de ángulo en el SpinBox para rotar el cañón en tiempo real
    connect(spinAngulo, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            controlador, &ControladorJuego::actualizarAnguloCanon);

    connect(btnDisparar, &QPushButton::clicked, this, &MainWindow::onDispararClicked);
    connect(btnReiniciar, &QPushButton::clicked, this, &MainWindow::onReiniciarClicked);
    connect(controlador, &ControladorJuego::turnoCambiado, this, &MainWindow::onTurnoCambiado);
    connect(controlador, &ControladorJuego::juegoTerminado, this, &MainWindow::onJuegoTerminado);
}

MainWindow::~MainWindow() {}

void MainWindow::onDispararClicked() {
    if (!controlador->hayDisparoEnCurso()) {
        btnDisparar->setEnabled(false);
        controlador->iniciarTurno(spinAngulo->value(), spinVelocidad->value());
    }
}

void MainWindow::onReiniciarClicked() {
    btnReiniciar->setVisible(false);
    btnDisparar->setEnabled(true);
    spinAngulo->setValue(45.0);
    controlador->reiniciarJuego();
}

void MainWindow::onTurnoCambiado(int turno) {
    btnDisparar->setEnabled(true);
    controlador->actualizarAnguloCanon(spinAngulo->value());
    if (turno == 1) {
        lblTurno->setText("Turno: JUGADOR 1 (Izquierda)");
        lblTurno->setStyleSheet("font-weight: bold; font-size: 14px; color: #1a5fb4;");
    } else {
        lblTurno->setText("Turno: JUGADOR 2 (Derecha)");
        lblTurno->setStyleSheet("font-weight: bold; font-size: 14px; color: #c01c28;");
    }
}

void MainWindow::onJuegoTerminado(int ganador) {
    QMessageBox::information(this, "¡Victoria!",
                             QString("¡El JUGADOR %1 ha destruido al rival y ganado la partida!").arg(ganador));
    btnDisparar->setEnabled(false);
    btnReiniciar->setVisible(true);
}
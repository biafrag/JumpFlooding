#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->openGLWidget->setFixedSize(1200,800);
    setFixedSize(1220,800);
}

MainWindow::~MainWindow()
{
    delete ui;
}


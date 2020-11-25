#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->openGLWidget->setFixedSize(800,800);
}

MainWindow::~MainWindow()
{
    delete ui;
}


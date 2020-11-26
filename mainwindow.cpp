#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "renderer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->openGLWidget->setFixedSize(1300,900);
    setFixedSize(1320,900);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_radioButton_clicked()
{
    ui->openGLWidget->setMode(Renderer::MODE::SIMPLE_JFA);
}

void MainWindow::on_radioButton_2_clicked()
{
    ui->openGLWidget->setMode(Renderer::MODE::JFA_1);
}

void MainWindow::on_radioButton_3_clicked()
{
    ui->openGLWidget->setMode(Renderer::MODE::JFA_2_1);
}

void MainWindow::on_radioButton_4_clicked()
{
    ui->openGLWidget->setMode(Renderer::MODE::JFA_QUAD);

}

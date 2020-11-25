#pragma once

#include <QOpenGLFunctions>
#include<QOpenGLWidget>
#include<QOpenGLShaderProgram>
#include<QOpenGLBuffer>
#include<QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>

class Renderer:
        public QOpenGLWidget
        , protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    Renderer(QWidget *parent);
    ~Renderer();
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void generateGrid(unsigned int quantX, unsigned int quantY, float delta);

private:

    void findMinMax();

    QMatrix4x4 _model;
    QMatrix4x4 _proj;

    QOpenGLShaderProgram* _program{nullptr};
    QOpenGLVertexArrayObject _vao;
    QOpenGLVertexArrayObject _vao2;

    //QOpenGLBuffer _pointsBuffer;

    QVector3D _min;
    QVector3D _max;
    std::vector<QVector3D> _points;
    std::vector<QVector3D> _colors;
    std::vector<QVector3D> _pointsScreen;
    std::vector<unsigned int> _indexPoints;


    //Parte do GBuffer
    unsigned int _gBuffer;
    unsigned int _gSeeds;
    unsigned int _gColors;
    unsigned int _gSeeds2;
    unsigned int _gColors2;

    unsigned int _pointsBuffer = static_cast<unsigned int>(-1);
    unsigned int _colorsBuffer = static_cast<unsigned int>(-1);
    unsigned int _pointsScreenBuffer = static_cast<unsigned int>(-1);
    unsigned int _meshBuffer = static_cast<unsigned int>(-1);

private:
    void createVAO(); //Cria VAO
    void createVAO2(); //Cria VAO2
    QOpenGLShaderProgram* _programGB{nullptr};

    QOpenGLShaderProgram* _programQuad{nullptr};
    QOpenGLShaderProgram* _programJFA{nullptr};


    void createFrameBuffer();
    void updateFrameBuffer();
    bool _desenha = true;
};

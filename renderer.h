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
    void reconstructFBO();

    enum MODE
    {
        SIMPLE_JFA,
        JFA_1,
        JFA_2_1,
        JFA_QUAD
    };
    void setMode(MODE m);
    void setStep(int step);
    void generatePoints(unsigned int n);

private:

    void findMinMax();

    QMatrix4x4 _model;
    QMatrix4x4 _proj;

    QOpenGLShaderProgram* _program{nullptr};
    QOpenGLVertexArrayObject _vao;
    QOpenGLVertexArrayObject _vao2;
    QOpenGLVertexArrayObject _vao3;

    //QOpenGLBuffer _pointsBuffer;

    QVector3D _min;
    QVector3D _max;
    std::vector<QVector3D> _points;
    std::vector<QVector3D> _colors;
    std::vector<QVector3D> _seeds;
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
    unsigned int _seedsBuffer = static_cast<unsigned int>(-1);
    unsigned int _meshBuffer = static_cast<unsigned int>(-1);

private:
    void createVAO(); //Cria VAO
    void createVAO2(); //Cria VAO2
    void createVAO3(); //Cria VAO2

    QOpenGLShaderProgram* _programGB{nullptr};

    QOpenGLShaderProgram* _programQuad{nullptr};
    QOpenGLShaderProgram* _programJFA{nullptr};

    void preStep();
    void JFA();
    void JFA_Passo_1();
    void JFA_Passo_2E1();
    void JFA_Quad();
    void drawSeeds();

    void show();

    void createFrameBuffer();
    void updateFrameBuffer();

    int _readStatus = 1;
    bool _desenha = true;
    int _step = 1;
    bool _isSecondTime = false;

    MODE _mode = SIMPLE_JFA;
};

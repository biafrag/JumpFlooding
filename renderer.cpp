#include "renderer.h"
#include <iostream>
#include <fstream>
#include <QOpenGLFunctions>
#include<QOpenGLWidget>
#include<QOpenGLShaderProgram>
#include<QOpenGLBuffer>

Renderer::Renderer(QWidget* parent)
        :QOpenGLWidget(parent)
{
    //Criando pontos do quad da tela
   _pointsScreen = {
        QVector3D(1.0f, -1.0f, 0.0f),
         QVector3D(-1.0f, -1.0f, 0.0f),
         QVector3D(-1.0f,  1.0f, 0.0f),
         QVector3D(-1.0f,  1.0f, 0.0f),
         QVector3D(1.0f, -1.0f, 0.0f),
         QVector3D(1.0f,  1.0f, 0.0f),
    };
    generateGrid(8,8,10);
    findMinMax();
}



Renderer::~Renderer()
{
}



void Renderer::initializeGL()
{

    initializeOpenGLFunctions();
    makeCurrent();

    glLineWidth(4.f);
    glPointSize(6.0f);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0,0,0,1);

    createFrameBuffer();


    //Parte do frameBuffer
    _programGB = new QOpenGLShaderProgram();

    //Adicionando shaders ao programa

    _programGB->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/GBvertexShader.glsl");
    _programGB->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/GBfragmentShader.glsl");

    //Linka shaders que foram adicionados ao programa
    _programGB->link();

    if (!_programGB->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders de Gbuffer"<<std::endl;
    }

    _programGB->bind();

    createVAO();

    //Programa do JFA

    //Parte do frameBuffer
    _programJFA = new QOpenGLShaderProgram();

    //Adicionando shaders ao programa

    _programJFA->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/JFAVertexShader.glsl");
    _programJFA->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/JFAFragmentShader.glsl");

    //Linka shaders que foram adicionados ao programa
    _programJFA->link();

    if (!_programJFA->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders de Gbuffer"<<std::endl;
    }

    _programJFA->bind();

    _programQuad = new QOpenGLShaderProgram();
    //Adicionando shaders ao programa

    _programQuad->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexShader.glsl");
    _programQuad->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentShader.glsl");

    //Linka shaders que foram adicionados ao programa
    _programQuad->link();

    if (!_programQuad->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders de Quad"<<std::endl;
    }
    _programQuad->bind();

    createVAO2();

}



void Renderer::resizeGL(int w, int h)
{
    //Atualizar a viewport
    glViewport(0,0,w,h);
    updateFrameBuffer();
}



void Renderer::paintGL()
{

     //PRIMEIRA PASSADA
     _programGB->bind();
     _vao.bind();
     glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
     glViewport(0, 0, width(), height());
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      _proj.setToIdentity();
      float left = _min.x() - 10;
      float right = _max.x()  + 10;
      float bottom = _min.y() - 10;
      float up = _max.y() + 10;

      _proj.ortho(left, right, bottom, up, 0, 100);
      QMatrix4x4 mvp = _proj;
      _programGB->setUniformValue("mvp", mvp);


     glDrawElements(GL_POINTS, static_cast<GLsizei>(_indexPoints.size()), GL_UNSIGNED_INT, nullptr);

//     glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
//     glReadBuffer(GL_COLOR_ATTACHMENT2);
//     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//     glBlitFramebuffer(0, 0,  width(), height(),
//                       0, 0, width(), height(),
//                       GL_COLOR_BUFFER_BIT, GL_NEAREST);

//     return;


     //Passada do JFA
     _programJFA->bind();
     _vao2.bind();
     //glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
     //glViewport(0, 0, width(), height());
     //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     int gSeedsLocationRead = 1;
     unsigned int gSeedsLocation;
     for(int i = 0; i < 3; i++)
     {
         _programJFA->setUniformValue("read", gSeedsLocationRead);

         //Ativar e linkar a textura de tangente
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, _gSeeds);
         gSeedsLocation = glGetUniformLocation(_programJFA->programId(), "gSeedsSampler");
         glUniform1i(gSeedsLocation, 0);

         glActiveTexture(GL_TEXTURE1);
         glBindTexture(GL_TEXTURE_2D, _gSeeds2);
         gSeedsLocation = glGetUniformLocation(_programJFA->programId(), "gSeedsSampler2");
         glUniform1i(gSeedsLocation, 1);

//         glActiveTexture(GL_TEXTURE1);
//         glBindTexture(GL_TEXTURE_2D, _gColors);
//         unsigned int gColorsLocation = glGetUniformLocation(_programJFA->programId(), "gColors");
//         glUniform1i(gColorsLocation, 1);

         glDrawArrays(GL_TRIANGLES, 0, (int)_pointsScreen.size());

         if(gSeedsLocationRead == 1)
         {
             gSeedsLocationRead = 2;
         }
         else
         {
             gSeedsLocationRead = 1;
         }
     }



          glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
          glReadBuffer(GL_COLOR_ATTACHMENT2);
          glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
          glBlitFramebuffer(0, 0,  width(), height(),
                            0, 0, width(), height(),
                            GL_COLOR_BUFFER_BIT, GL_NEAREST);

          return;

      //SEGUNDA PASSADA
     //Dando bind no programa e no vao
     _programQuad->bind();
     _vao2.bind();
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glDrawBuffer(GL_BACK);
     glViewport(0, 0, width(), height());
     glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

     _proj.setToIdentity();
     left = -10;
     right = 10;
     bottom = -10;
     up = 10;
     _proj.ortho(left, right, bottom, up, 0, 100);

     _programQuad->setUniformValue("mvp", mvp);


     //Ativar e linkar a textura de tangente
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, _gSeeds2);
     gSeedsLocation = glGetUniformLocation(_programQuad->programId(), "gSeeds");
     glUniform1i(gSeedsLocation, 0);
     glDrawArrays(GL_TRIANGLES, 0, (int)_pointsScreen.size());

}



void Renderer::findMinMax()
{
    _min = _max = _points[0];
    for(auto point : _points)
    {
        if(point.x() > _max.x())
        {
            _max.setX(point.x());
        }
        else if (point.x() < _min.x())
        {
            _min.setX(point.x());
        }
        if(point.y() > _max.y())
        {
            _max.setY(point.y());
        }
        else if (point.y() < _min.y())
        {
            _min.setY(point.y());
        }
    }
}



void Renderer::createVAO()
{
    //Criando e configurando vao
        _vao.create();
        _vao.bind();

        //Criando buffer de pontos dos vértices
        glGenBuffers(1, &_pointsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _pointsBuffer);
        glBufferData(GL_ARRAY_BUFFER, _points.size()*sizeof(QVector3D), &_points[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        //Criando buffer de pontos dos vértices
        glGenBuffers(1, &_colorsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _colorsBuffer);
        glBufferData(GL_ARRAY_BUFFER, _colors.size()*sizeof(QVector3D), &_colors[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        //Criando buffers de indexPoints
        glGenBuffers(1, &_meshBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexPoints.size()*sizeof(int), _indexPoints.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);

}



void Renderer::createVAO2()
{
    //Criando e configurando vao
    _vao2.create();
    _vao2.bind();

    //Criando buffer de pontos dos vértices
    glGenBuffers(1, &_pointsScreenBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _pointsScreenBuffer);
    glBufferData(GL_ARRAY_BUFFER, _pointsScreen.size()*sizeof(QVector3D), &_pointsScreen[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}



void Renderer::createFrameBuffer()
{
        //Gerando GBuffer e dando bind nele
        glGenFramebuffers(1, &_gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

        // - Buffer de Posição
        glGenTextures(1, &_gSeeds);
        glBindTexture(GL_TEXTURE_2D, _gSeeds);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gSeeds, 0);

        glGenTextures(1, &_gColors);
        glBindTexture(GL_TEXTURE_2D, _gColors);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gColors, 0);


        glGenTextures(1, &_gSeeds2);
        glBindTexture(GL_TEXTURE_2D, _gSeeds2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gSeeds2, 0);

        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);


        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Erro no frame buffer\n");
        }
}



void Renderer::updateFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

    glBindTexture(GL_TEXTURE_2D, _gSeeds);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, _gSeeds2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, _gColors);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);
}



void Renderer::generateGrid(unsigned int quantX, unsigned int quantY, float delta)
{
    QVector3D point;
    int ind = 0;
    for(unsigned int j = 0; j < quantY; j++)
    {
        for(unsigned int i = 0; i < quantX; i++)
        {
            point = QVector3D(delta*i, delta*j, 0);
            _points.push_back(point);
            _indexPoints.push_back(ind);
            ind++;
        }
    }
    _colors.resize(_points.size(), QVector3D(1,1,1));
    _colors[22] = QVector3D(0,1,0);
    _colors[25] = QVector3D(1,0,0);
    _colors[44] = QVector3D(0,0,1);

}


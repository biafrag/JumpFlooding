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
//   _pointsScreen = {
//        QVector3D(1.0f, -1.0f, 0.0f),
//         QVector3D(-1.0f, -1.0f, 0.0f),
//         QVector3D(-1.0f,  1.0f, 0.0f),
//         QVector3D(-1.0f,  1.0f, 0.0f),
//         QVector3D(1.0f, -1.0f, 0.0f),
//         QVector3D(1.0f,  1.0f, 0.0f),
//    };

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

    generateGrid(8,8,1);
    _pointsScreen = _points;
    findMinMax();

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
     //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      _proj.setToIdentity();
      float left = _min.x();
      float right = _max.x();
      float bottom = _min.y() ;
      float up = _max.y();

      _proj.ortho(left, right, bottom, up, 0, 100);
      QMatrix4x4 mvp = _proj;
      _programGB->setUniformValue("mvp", mvp);


     glDrawElements(GL_POINTS, static_cast<GLsizei>(_indexPoints.size()), GL_UNSIGNED_INT, nullptr);

//     glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
//     glReadBuffer(GL_COLOR_ATTACHMENT1);
//     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//     glBlitFramebuffer(0, 0,  width(), height(),
//                       0, 0, width(), height(),
//                       GL_COLOR_BUFFER_BIT, GL_NEAREST);

//     return;

     //Passada do JFA
     _programJFA->bind();
     _vao2.bind();
     //_vao.bind();
     //glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
     //glViewport(0, 0, width(), height());
     //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     int gSeedsLocationRead = 1;
     unsigned int gSeedsLocation;
     int n = 8;
     //_programJFA->setUniformValue("min", QVector3D(0,0,0));
     //_programJFA->setUniformValue("max", QVector3D(width(),height(),0));

     _programJFA->setUniformValue("min", _min);
     _programJFA->setUniformValue("max", _max);

     _programGB->setUniformValue("mvp", mvp);

     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, _gSeeds);

     glActiveTexture(GL_TEXTURE1);
     glBindTexture(GL_TEXTURE_2D, _gSeeds2);

     glActiveTexture(GL_TEXTURE2);
     glBindTexture(GL_TEXTURE_2D, _gColors);

     glActiveTexture(GL_TEXTURE3);
     glBindTexture(GL_TEXTURE_2D, _gColors2);
     for(int p = n/2; p >= 1; p = p/2)
     {


         _programJFA->setUniformValue("read", gSeedsLocationRead);

         _programJFA->setUniformValue("pass", p);


         if(gSeedsLocationRead == 1)
         {
             //Ativar e linkar a textura de tangente
             glActiveTexture(GL_TEXTURE0);
             glBindTexture(GL_TEXTURE_2D, _gSeeds);
             gSeedsLocation = glGetUniformLocation(_programJFA->programId(), "gSeedsSampler");
             glUniform1i(gSeedsLocation, 0);


             glActiveTexture(GL_TEXTURE1);
             glBindTexture(GL_TEXTURE_2D, _gColors);
             gSeedsLocation = glGetUniformLocation(_programJFA->programId(), "gColorsSampler");
             glUniform1i(gSeedsLocation, 1);

             gSeedsLocationRead = 2;
         }
         else
         {

             glActiveTexture(GL_TEXTURE0);
             glBindTexture(GL_TEXTURE_2D, _gSeeds2);
             gSeedsLocation = glGetUniformLocation(_programJFA->programId(), "gSeedsSampler2");
             glUniform1i(gSeedsLocation, 0);

             glActiveTexture(GL_TEXTURE1);
             glBindTexture(GL_TEXTURE_2D, _gColors2);
             gSeedsLocation = glGetUniformLocation(_programJFA->programId(), "gColorsSampler2");
             glUniform1i(gSeedsLocation, 1);


             gSeedsLocationRead = 1;
         }
         glDrawArrays(GL_POINTS, 0, (int)_pointsScreen.size());
     }



          glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
          glReadBuffer(GL_COLOR_ATTACHMENT3);
          glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
          glBlitFramebuffer(0, 0,  width(), height(),
                            0, 0, width(), height(),
                            GL_COLOR_BUFFER_BIT, GL_NEAREST);
         //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //updateFrameBuffer();
         //reconstructFBO();
         //update();
          //return;

//      //SEGUNDA PASSADA
//     //Dando bind no programa e no vao
//     _programQuad->bind();
//     _vao2.bind();
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    // glDrawBuffer(GL_BACK);
//     glViewport(0, 0, width(), height());
//     glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

//     _proj.setToIdentity();
////     left = -10;
////     right = 10;
////     bottom = -10;
////     up = 10;
////     _proj.ortho(left, right, bottom, up, 0, 100);

//     _programQuad->setUniformValue("mvp", mvp);


//     //Ativar e linkar a textura de tangente
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, _gSeeds2);
//     gSeedsLocation = glGetUniformLocation(_programQuad->programId(), "gSeeds");
//     glUniform1i(gSeedsLocation, 0);
//     glDrawArrays(GL_TRIANGLES, 0, (int)_pointsScreen.size());

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
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gSeeds, 0);

        glGenTextures(1, &_gColors);
        glBindTexture(GL_TEXTURE_2D, _gColors);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gColors, 0);


        glGenTextures(1, &_gSeeds2);
        glBindTexture(GL_TEXTURE_2D, _gSeeds2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gSeeds2, 0);

        glGenTextures(1, &_gColors2);
        glBindTexture(GL_TEXTURE_2D, _gColors2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gColors2, 0);

        unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
        glDrawBuffers(4, attachments);


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

    glBindTexture(GL_TEXTURE_2D, _gColors2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width() , height(), 0, GL_RGB, GL_FLOAT, NULL);

    update();
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
    //_colors[22] = QVector3D(0,1,0);
    _colors[25] = QVector3D(1,0,0);
   // _colors[44] = QVector3D(0,0,1);

}



void Renderer::reconstructFBO()
{
    glDeleteTextures(1, &_gSeeds);
    glDeleteTextures(1, &_gSeeds2);
    glDeleteTextures(1, &_gColors);
    glDeleteTextures(1, &_gColors2);
    glDeleteFramebuffers(1, &_gBuffer);
    createFrameBuffer();
}


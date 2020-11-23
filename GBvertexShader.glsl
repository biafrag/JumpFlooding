#version 410 core
layout( location = 0 ) in vec3 vertexPos; //Posição do vértice

//Matrizes
uniform mat4 mvp; //Matriz model view projection
uniform mat4 mv; //Matriz model view projection

out vec3 fragPos; //Posição no espaço do olho

void main()
{
    //Posição do vértice no espaço de projeção
    gl_Position = mvp * vec4( vertexPos, 1 );

    fragPos = vertexPos;
}

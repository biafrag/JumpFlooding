#version 410 core
layout( location = 0 ) in vec3 vertexPos; //Posição do vértice
layout( location = 1 ) in vec3 color; //Posição do vértice

//Matrizes
uniform mat4 mvp; //Matriz model view projection

out vec3 fragPos; //Posição no espaço do olho
out vec3 fragColor; //Posição no espaço do olho

void main()
{
    //Posição do vértice no espaço de projeção
    gl_Position = mvp * vec4( vertexPos, 1 );

    fragPos = vertexPos;
    fragColor = color;
}

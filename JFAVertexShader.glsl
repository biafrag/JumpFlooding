#version 410 core

layout (location = 2) in vec3 vPos; //Posição dos vértices do quadrilátero

out vec3 fragPos;

uniform mat4 mvp; //Matriz model view projection

out vec3 fragMin;
out vec3 fragMax;
void main()
{
    //Posição do vértice no espaço de projeção
    gl_Position = mvp * vec4( vPos, 1 );

    fragPos = vPos;
}

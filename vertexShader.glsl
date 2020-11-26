#version 410 core

layout (location = 2) in vec3 vPos; //Posição dos vértices do quadrilátero
out vec2 UV; //Coordenadas de textura do quad
uniform mat4 mvp; //Matriz model view projection
uniform vec3 min; //Matriz model view projection
uniform vec3 max; //Matriz model view projection

void main()
{
    //Posição do vértice no espaço de projeção
    gl_Position = mvp * vec4( vPos, 1 );

    UV.x = (vPos.x - min.x) / (max.x - min.x);
    UV.y = (vPos.y - min.y) / (max.y - min.y);
}

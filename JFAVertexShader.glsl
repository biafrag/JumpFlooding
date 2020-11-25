#version 410 core

layout (location = 2) in vec3 vPos; //Posição dos vértices do quadrilátero

out vec2 UV; //Coordenadas de textura do quad
out vec3 fragPos;

uniform vec3 min;
uniform vec3 max;
uniform mat4 mvp; //Matriz model view projection

void main()
{
    //Posição do vértice no espaço de projeção
    gl_Position = mvp * vec4( vPos, 1 );

    //UV = (vPos.xy + vec2(1,1))/2.0;
    vec4 pos = mvp * vec4( vPos, 1 );
    UV.x = (vPos.x - min.x) / (max.x - min.x);
    UV.y = (vPos.y - min.y) / (max.y - min.y);

    //UV = (vPos.xy + vec2(1,1))/2.0;

    fragPos = vPos;

    //UV = mvp * vec4(vPos.xy,1);
}

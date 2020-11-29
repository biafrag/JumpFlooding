#version 410 core

layout (location = 0) in vec3 vPos; //Posição dos vértices do quadrilátero
uniform mat4 mvp; //Matriz model view projection
out vec3 fragPos;
void main()
{
    //Posição do vértice no espaço de projeção
    gl_Position = mvp * vec4( vPos, 1 );

    fragPos = vPos;
}

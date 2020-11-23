#version 410 core
layout (location = 0) out vec3 gSeeds; //Buffer de Seeds

in vec3 fragPos; // Posição no espaço do olho

void main()
{
    gSeeds = fragPos; //Passando informação de posição para buffer
}

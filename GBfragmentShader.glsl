#version 410 core
layout (location = 0) out vec3 gSeeds; //Buffer de Seeds
layout (location = 1) out vec3 gColors; //Buffer de Colors
layout (location = 2) out vec3 gSeeds2; //Buffer de Colors

in vec3 fragPos; // Posição no espaço do olho
in vec3 fragColor; // Posição no espaço do olho

void main()
{
    gSeeds = (fragPos - vec3(0,0,0)) / (vec3(7,7,0) - vec3(0,0,0)); //Passando informação de posição para buffer
    gColors = fragColor;
}

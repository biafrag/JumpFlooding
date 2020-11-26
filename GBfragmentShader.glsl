#version 410 core
layout (location = 0) out vec3 gSeeds; //Buffer de Seeds
layout (location = 1) out vec3 gColors; //Buffer de Colors

in vec3 fragPos; // Posição no espaço do olho
in vec3 fragColor; // Posição no espaço do olho
uniform vec3 min;
uniform vec3 max;
void main()
{
    gSeeds = (fragPos - min) / (max - min); //Guardando info de posição no espaço de (0,1)
    gColors = fragColor;
}

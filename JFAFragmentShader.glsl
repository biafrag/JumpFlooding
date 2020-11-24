#version 410 core
layout (location = 1) out vec3 gSeeds2; //Buffer de Seeds

in vec3 fragPos; // Posição no espaço do olho
uniform sampler2D gSeeds; //Textura de Seeds
in vec2 UV; //Coordenadas de textura do quad

void main()
{
    vec3 aux = texture(gSeeds, UV).rgb; //Passando informação de posição para buffer
    gSeeds2 = aux;

}

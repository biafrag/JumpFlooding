#version 410 core

//Variaveis de entrada

in vec2 UV; //Coordenadas de textura do quad
out vec3 finalColor; // Cor final do objeto

uniform sampler2D gSeeds2; //Textura de Seeds

void main()
{

    finalColor = texture(gSeeds2, UV).rgb;
}

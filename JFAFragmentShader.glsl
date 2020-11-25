#version 410 core
layout (location = 0) out vec3 gSeeds; //Buffer de Seeds
layout (location = 1) out vec3 gColors; //Buffer de Seeds
layout (location = 2) out vec3 gSeeds2; //Buffer de Seeds 2
layout (location = 3) out vec3 gColors2; //Buffer de Seeds

in vec3 fragPos; // Posição no espaço do olho
uniform sampler2D gSeedsSampler; //Textura de Seeds
uniform sampler2D gSeedsSampler2; //Textura de Seeds 2
uniform sampler2D gColorsSampler; //Textura de Seeds 2
uniform sampler2D gColorsSampler2; //Textura de Seeds 2

uniform int read;
in vec2 UV; //Coordenadas de textura do quad

void main()
{
    if(read == 1)
    {
        vec3 aux = texture(gSeedsSampler, UV).rgb; //Passando informação de posição para buffer
        gSeeds2 = aux;

        vec3 aux2 = texture(gColorsSampler, UV).rgb; //Passando informação de posição para buffer
        gColors2 = aux2;
    }
    else
    {
        vec3 aux = texture(gSeedsSampler2, UV).rgb; //Passando informação de posição para buffer
        gSeeds = aux;

        vec3 aux2 = texture(gColorsSampler2, UV).rgb; //Passando informação de posição para buffer
        gColors = aux2;
    }

}

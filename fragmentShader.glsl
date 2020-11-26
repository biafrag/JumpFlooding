#version 410 core

//Variaveis de entrada

in vec2 UV; //Coordenadas de textura do quad
out vec3 finalColor; // Cor final do objeto

uniform sampler2D gSeedsSampler; //Textura de Seeds
uniform sampler2D gSeedsSampler2; //Textura de Seeds 2
uniform sampler2D gColorsSampler; //Textura de Seeds 2
uniform sampler2D gColorsSampler2; //Textura de Seeds 2
uniform int read;

void main()
{
    if(read == 1)
    {
        finalColor = texture(gColorsSampler, UV).rgb;
        //finalColor = vec3(0,0,0);
        vec3 pos = texture(gSeedsSampler, UV).rgb;
        if((UV.x == pos.x) && (UV.y == pos.y))
        {
            finalColor = vec3(1,1,1);
        }

    }
    else
    {
        //finalColor = vec3(0,0,0);

        finalColor = texture(gColorsSampler2, UV).rgb;
        vec3 pos = texture(gSeedsSampler2, UV).rgb;
        if((UV.x == pos.x) &&  (UV.y == pos.y))
        {
            finalColor = vec3(1,1,1);
        }
    }
}

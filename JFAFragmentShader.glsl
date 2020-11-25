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
uniform int pass;
uniform int read;
in vec3 fragMin;
in vec3 fragMax;
in vec2 UV; //Coordenadas de textura do quad
float dist(vec2 p1, vec2 p2)
{
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

void main()
{
    if(read == 1)
    {
        vec3 seed = texture(gSeedsSampler, UV).rgb; //Passando informação de posição para buffer
        vec3 color = texture(gColorsSampler, UV).rgb; //Passando informação de posição para buffer

        for(int x = -pass; x <= pass; x = x + pass)
        {
            for(int y = -pass; y <= pass; y = y + pass)
            {
                //vec2 UVn =  UV.xy + vec2(x*dFdx(UV).x,y*dFdy(UV).y);
                vec2 pos = fragPos.xy + vec2(x,y);
                if((pos.x > fragMin.x && pos.y > fragMin.y) && (pos.x < fragMax.x && pos.y < fragMax.y))
                {
                        vec2 UVn = (pos - fragMin.xy) / (fragMax.xy - fragMin.xy);
                        vec3 seedN = texture(gSeedsSampler, UVn).rgb;
                        vec3 colorN = texture(gColorsSampler, UVn).rgb;

                        if(colorN != vec3(1))
                        {
                            if(color == vec3(1))
                            {
                                color = colorN;
                                seed = seedN;
                            }
                            else
                            {
                              if(dist(UV, seedN.xy) < dist(UV,seed.xy))
                              {
                                   color = colorN;
                                   seed = seedN;
                              }
                            }
                        }
                }

            }
        }

        gSeeds2 = seed;
        gColors2 = color;
        //gSeeds = texture(gSeedsSampler, UV).rgb;
        //gColors =texture(gColorsSampler, UV).rgb;
        //gColors2 = vec3(1,0,0);
    }
    else
    {
        vec3 seed = texture(gSeedsSampler2, UV).rgb; //Passando informação de posição para buffer
        vec3 color = texture(gColorsSampler2, UV).rgb; //Passando informação de posição para buffer

        for(int x = -pass; x <= pass; x = x + pass)
        {
            for(int y = -pass; y <= pass; y = y + pass)
            {
                //vec2 UVn =  UV.xy + vec2(x*dFdx(UV).x,y*dFdy(UV).y);
                vec2 pos = fragPos.xy + vec2(x,y);
                if((pos.x > fragMin.x && pos.y > fragMin.y) && (pos.x < fragMax.x && pos.y < fragMax.y))
                {
                        vec2 UVn = (pos - fragMin.xy) / (fragMax.xy - fragMin.xy);
                        vec3 seedN = texture(gSeedsSampler, UVn).rgb;
                        vec3 colorN = texture(gColorsSampler, UVn).rgb;

                        if(colorN != vec3(1))
                        {
                            if(color == vec3(1))
                            {
                                color = colorN;
                                seed = seedN;
                            }
                            else
                            {

                                    if(dist(UV, seedN.xy) < dist(UV,seed.xy))
                                    {
                                         color = colorN;
                                         seed = seedN;
                                    }

                            }
                        }
                }

            }
        }


        gSeeds = seed;
        gColors = color;
        //gSeeds2 = texture(gSeedsSampler2, UV).rgb;
        //gColors2 = texture(gColorsSampler2, UV).rgb;
        //gColors = vec3(1,0,0);
    }

}

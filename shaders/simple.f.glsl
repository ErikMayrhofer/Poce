#version 450

in vec2 Texcoord;
in vec2 Coords;
uniform sampler2D texture_sampler;

out vec4 outColor;

uniform game_data{
    vec2 playerL;
    vec2 playerR;
    vec2 ball;
};

void main()
{
    if(distance(Coords, playerR) < 10){
        outColor = vec4(0.0,1.0,0.0,0.0);
    }else if(distance(Coords, playerL) < 10){
        outColor = vec4(1.0,0.0,0.0,0.0);
    }else if(distance(Coords, ball) < 10){
        outColor = vec4(0.0,0.0,0.0,0.0);
    }else{
        outColor = texture2D(texture_sampler, Texcoord);
    }
}
#version 450

in vec2 Texcoord;
uniform sampler2D texture_sampler;

out vec4 outColor;

uniform game_data{
    vec2 playerL;
    vec2 playerR;
    vec2 ball;
};

void main()
{
    //outColor = texture(tex, Texcoord);
    if(distance(Texcoord, playerR) < 0.2){
        outColor = vec4(0.0,1.0,0.0,0.0);
    }else if(distance(Texcoord, playerL) < 0.2){
        outColor = vec4(1.0,0.0,0.0,0.0);
    }else if(distance(Texcoord, ball) < 0.2){
        outColor = vec4(0.0,0.0,0.0,0.0);
    }else{
        outColor = texture2D(texture_sampler, Texcoord);
    }
}
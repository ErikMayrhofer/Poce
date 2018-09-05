#version 450
#define M_PI 3.1415926535897932384626433832795

in vec2 Texcoord;
in vec2 Coords;
uniform sampler2D texture_sampler;

out vec4 outColor;

uniform game_data{
    vec2 playerL;
    vec2 playerR;
    vec2 ball;
    vec2 fieldSize;
    int timeMS;
};

uniform config_data{
    float faceSize;
};

float angle(vec2 a, vec2 b){
    float dy = b.y - a.y;
    float dx = a.x - b.x;
    if(dx == 0){
        if(dy > 0){
            return M_PI/2;
        }else if(dy < 0){
            return -M_PI/2;
        }
    }

    return atan(dy, dx)+M_PI;
}

vec4 debug(vec2 coords){
    return vec4(mod(10*coords, 1.0), 1.0, 1.0);
}

vec4 distort(vec2 center, float radius, float amount){
    radius += 20;
    float angle = angle(center, Coords);
    float dist = distance(Coords, center);
    float distPct = dist/radius;
    float scaledDistPct = pow(distPct, amount);
    float scaledDist = scaledDistPct * radius;
    float dx = cos(angle)*scaledDist;
    float dy = sin(angle)*scaledDist;
    vec2 newCoords = center + vec2(dx, -dy);
    return texture2D(texture_sampler, newCoords/fieldSize);
}

vec4 blackHole(vec2 center, float radius, float amount){
    float angle = angle(center, Coords);
    float dist = distance(Coords, center);
    float distPct = 1.0-dist/radius;
    float scaledDistPct = pow(distPct*6.0, amount);
    if(scaledDistPct > 1.0){
        return vec4(0.0);
    }
    float scaledDist = scaledDistPct * radius;
    float dx = cos(angle)*scaledDist;
    float dy = sin(angle)*scaledDist;
    vec2 newCoords = center + vec2(dx, -dy);
    return texture2D(texture_sampler, newCoords/fieldSize);
}

void main()
{
    //float amount = sin(float(timeMS)/2000.0)*2.0+1.0;
    float amount = 2.0;
    if(distance(Coords, playerR) < faceSize){
        outColor = blackHole(playerR, faceSize, amount);
    }else if(distance(Coords, playerL) < faceSize){
        outColor = blackHole(playerL, faceSize, amount);
    }else if(distance(Coords, ball) < faceSize){
        outColor = blackHole(ball, faceSize, amount);
    }else{
        outColor = texture2D(texture_sampler, Texcoord);
    }
}
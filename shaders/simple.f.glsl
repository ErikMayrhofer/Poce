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

vec2 rotate(vec2 center, vec2 position, float angle, float stretch, float skew){
	vec2 normCoords = position - center;

	float cos = cos(angle * stretch);
	float sin = sin(angle * skew);
	float nx = cos * normCoords.x + sin * normCoords.y;
	float ny = cos * normCoords.y - sin * normCoords.x;

	return vec2(nx, ny) + center;
}

float flattenFromInfinity(float zeropoint, float x, float flatness){
    if(x > zeropoint) return 0.0;
    float mx = x - zeropoint;
    return -(mx*mx*mx)/(flatness*x);
}

vec4 blackHole(vec2 center, float radius, float amount){
    vec2 st = Coords;
    vec2 mt = center;

    float dx = st.x - mt.x;
    float dy = st.y - mt.y;

    float dist = sqrt(dx * dx + dy * dy);
    float pull = flattenFromInfinity(radius, dist, 500000);

    vec2 r = rotate(mt,st,pull, 1.0, 0.6);
    vec4 imgcolor = texture2D(texture_sampler, r/fieldSize);
    float blackfac = 0.1;
    vec3 color = vec3(imgcolor.x,imgcolor.y,imgcolor.z) - vec3(pull*blackfac);
    return vec4(color,1.);
}

void main()
{

    float amount = 2.0;
    float rdist = distance(Coords, playerR);
    float ldist = distance(Coords, playerL);
    float bdist = distance(Coords, ball);
    float minDist = rdist;
    vec2 minCoord = playerR;
    if(ldist < minDist){
        minDist = ldist;
        minCoord = playerL;
    }
    if(bdist < minDist){
        minDist = bdist;
        minCoord = ball;
    }

    if(minDist < faceSize){
        outColor = blackHole(minCoord, faceSize, amount);
    }else{
        outColor = texture2D(texture_sampler, Texcoord);
    }
}
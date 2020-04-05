#version 410 core

uniform sampler2D colorTexture;
uniform sampler2D heightmap;

// from evaluation shader
in VertexFS
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	float tesLevel;
} vert;

layout(location = 0) out vec4 color;

const float ScaleUV = 0.05, MaxHeight = 6;
vec3 TerreinColor()
{
	vec3 c=vec3(1);

	float n = (clamp(vert.normal.y,0.5,1)-0.5)*2;
	n = 1 - n;
	n*=0.8;
	n*=n*n*n*n;
	//n = 1 - n;
	vec2 t= texture(heightmap, vert.texcoord*ScaleUV).xy;
	float h = vert.position.y/MaxHeight;
	vec3 grass = vec3(0.07, 0.53, 0);
	vec3 mount = vec3(0.27, 0.24, 0);
	vec3 snow = vec3(1);
	if(h<0.05)
		return vec3(0,0,1);
	if(h<0.45)
		return mix(mount,grass,t.x);
	else
		return mix(mount,snow,t.y);
	return c;
}
void main(void)
{
	color = texture(colorTexture, vert.texcoord)  * vec4(TerreinColor(),1);
}

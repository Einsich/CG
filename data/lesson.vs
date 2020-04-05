#version 410 core

// mesh data
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
uniform vec3 cameraPosition;

// to control shader
out VertexCS
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	float tesLevel;
} vertcs;
float GetTessLevel(float AvgDistance)
{
 
    if (AvgDistance <= 2.0) {
        return 10.0;
    }
    else if (AvgDistance <= 5.0) {
        return 7.0;
    }
    else {
        return 3.0;
    }
}
void main(void)
{
	vertcs.position = position;
	vertcs.texcoord = texcoord;
	vertcs.normal = normal;
	vertcs.tesLevel = 3;//GetTessLevel(distance(cameraPosition, position));
	gl_Position = vec4(position, 1.0);
}


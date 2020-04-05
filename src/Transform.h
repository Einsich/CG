#pragma once
#include "math3d.h"
class Transform
{

public:
	vec3 position;
	vec3 rotation;
	mat4 rotMatrix;
	Transform()
	{
		rotation = vec3(0, 0, 0);
		rotMatrix = mat4_identity;
	}
	void Rotate(vec3 euler);
	void LookAt(vec3 target);
	void Move(vec3 dir, bool camera = false);
	vec3 Right(bool camera = false) { return  camera ? rotMatrix.r1 : vec3(rotMatrix[0], rotMatrix[4], rotMatrix[8]); }
	vec3 Up(bool camera = false) { return  camera ? rotMatrix.r2 : vec3(rotMatrix[1], rotMatrix[5], rotMatrix[9]); }
	vec3 Forward(bool camera = false) { return  camera ? rotMatrix.r3 : vec3(rotMatrix[2], rotMatrix[6], rotMatrix[10]); }
	mat4 GetTransformMatrix();
};
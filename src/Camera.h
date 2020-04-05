#pragma once
#include "common.h"
#include "math3d.h"
#include "mathgl.h"
#include "Transform.h"
#include "OpenGL.h"

class Camera
{
public:
	Transform transform;
	mat4 projection;
	float zFar;
	Camera() {
	}
	Camera(const vec3& position,const vec3& target);
	void CameraPerspective(float fov, float aspect, float zNear, float zFar);
	void CameraOrtho(float right, float top, float zNear, float zFar);
	void CameraSetup(GLuint program, const mat4& model);
	void CameraSetupLightMatrix(GLuint program, Camera * mainCamera);
};



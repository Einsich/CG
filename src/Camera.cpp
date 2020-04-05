#include "Camera.h"

Camera::Camera(const vec3&pos,const vec3& target)
{
	transform.position   = pos;
	transform.rotation   = vec3_zero;
	projection = mat4_identity;
	transform.LookAt(target);
}


void Camera::CameraPerspective(float fov, float aspect, float zNear, float zFar)
{
	this->zFar = zFar;
	projection = GLPerspective(fov, aspect, zNear, zFar);
}

void Camera::CameraOrtho( float right, float top, float zNear, float zFar)
{
	this->zFar = zFar;
	projection = GLOrthographic(-right, right, -top, top, zNear, zFar);
}



void Camera::CameraSetup(GLuint program, const mat4 &model)
{
	// расчитаем необходимые матрицы
	mat4 view           = transform.rotMatrix * GLTranslation(-transform.position);
	mat4 viewProjection = projection * view;
	mat3 normal         = transpose(mat3(inverse(model)));

	mat4 modelViewProjection = viewProjection * model;

	// передаем матрицы в шейдерную программу
	//glUniformMatrix4fv(glGetUniformLocation(program, "transform.model"),          1, GL_TRUE, model.m);
	//glUniformMatrix4fv(glGetUniformLocation(program, "transform.viewProjection"), 1, GL_TRUE, viewProjection.m);
	//glUniformMatrix3fv(glGetUniformLocation(program, "transform.normal"),         1, GL_TRUE, normal.m);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"),
		1, GL_TRUE, modelViewProjection.m);

	glUniform3fv(glGetUniformLocation(program, "cameraPosition"), 1, transform.position.v);
}

void Camera::CameraSetupLightMatrix(GLuint program, Camera* mainCamera)
{
	// матрица сдвига текстурных координат
	static const mat4 bias(
		0.5f, 0.0f, 0.0f, 0.5f,
		0.0f, 0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// расчитаем необходимые матрицы
	mat4 view = transform.rotMatrix *GLTranslation(-transform.position);
	mat4 viewProjection = bias * projection * view;

	// передадим матрицу источника освещения в шейдерную программу
	glUniformMatrix4fv(glGetUniformLocation(program, "transform.light"), 1, GL_TRUE, viewProjection.m);
}

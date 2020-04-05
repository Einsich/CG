#include "Transform.h"
#include "mathgl.h"

void Transform::Rotate(vec3 euler)
{
	rotation += euler;
	if (rotation.x > 360)
		rotation.x -= 360;
	if (rotation.x < -360)
		rotation.x += 360;

	if (rotation.y > 360)
		rotation.y -= 360;
	if (rotation.y < -360)
		rotation.y += 360;

	if (rotation.z > 360)
		rotation.z -= 360;
	if (rotation.z < -360)
		rotation.z += 360;
	rotMatrix = GLFromEuler(rotation);
}

//Rotate forward of focus to target
void Transform::LookAt(vec3 target)
{
	rotation = GLToEuler(GLLookAt(position, target, vec3_y));
	rotMatrix = GLFromEuler(rotation);
}
void Transform::Move(vec3 dir, bool camera)
{
	position += Forward(camera) * dir.z +Right(camera) * dir.x + Up(camera) * dir.y;
}
mat4 Transform::GetTransformMatrix()
{
	return GLTranslation(position) * rotMatrix;
}
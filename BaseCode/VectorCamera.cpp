#include <cmath>
#include "VectorCamera.h"
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>


#define PI 3.14159f


VectorCamera::VectorCamera()
{
	anglePitch = 0.f;
}

VectorCamera::~VectorCamera()
{
}


// Initialize the camera at initPosition looking in the Z- direction
// with a near plane of 0.01 and a far one of 100

void VectorCamera::init(const glm::vec3 &initPosition)
{
	position = initPosition;
	angleDirection = 180.f;
 	rangeDistanceCamera[0] = 0.01f;
	rangeDistanceCamera[1] = 100.f;
	computeModelViewMatrix();
}

// Resizing the viewport may change the aspect ratio.
// The projection matrix may need updating.
	
void VectorCamera::resizeCameraViewport(int width, int height)
{
  projection = glm::perspective(60.f / 180.f * PI, float(width) / float(height), 0.01f, 100.0f);
}

// Rotate the camera and recompute the modelview matrix.
// This takes into account rotations around the Y axis.

void VectorCamera::rotateCamera(float rotation)
{
	angleDirection += rotation;
	if(angleDirection < 0.f)
		angleDirection += 360.f;
	if(angleDirection >= 360.f)
		angleDirection -= 360.f;
	computeModelViewMatrix();
}

// This one rotates the camera around the local X axis instead.

void VectorCamera::changePitch(float rotation)
{
	anglePitch += rotation;
	if(anglePitch < -45.f)
		anglePitch = -45.f;
	if(anglePitch > 45.f)
		anglePitch = 45.f;
	computeModelViewMatrix();
}

// Move the camera forward in the direction it is looking in.

void VectorCamera::moveForward(float distance)
{
	glm::vec3 direction(sin(PI * angleDirection / 180.f), 0.f, cos(PI * angleDirection / 180.f));
	
	direction *= distance;
	position += direction;
	computeModelViewMatrix();
}

// Move the camera but in the direction of the local X axis (sideways).
// In FPS games this is known as strafing.

void VectorCamera::strafe(float distance)
{
	glm::vec3 direction(-cos(PI * angleDirection / 180.f), 0.f, sin(PI * angleDirection / 180.f));
	
	direction *= distance;
	position += direction;
	computeModelViewMatrix();
}

// Recompute the modelview matrix with the transformations needed to capture
// the current position and orientation of the vector camera

void VectorCamera::computeModelViewMatrix()
{
	glm::vec3 direction(sin(PI * angleDirection / 180.f), 0.f, cos(PI * angleDirection / 180.f));
	
	modelview = glm::rotate(glm::mat4(1.f), PI * anglePitch / 180.f, glm::vec3(1.f, 0.f, 0.f));
	modelview = modelview * glm::lookAt(position, position + direction, glm::vec3(0.f, 1.f, 0.f));
}

glm::mat4 &VectorCamera::getProjectionMatrix()
{
  return projection;
}

glm::mat4 &VectorCamera::getModelViewMatrix()
{
  return modelview;
}

glm::vec3 VectorCamera::getPosition()
{
	return position;
}

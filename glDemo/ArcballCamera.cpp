
#include "ArcballCamera.h"

using namespace std;
using namespace glm;

//
// Private API
//

// update position, orientation and view matrices when camera rotation and radius is modified
void ArcballCamera::calculateDerivedValues() {

	const float theta_ = glm::radians<float>(theta);
	const float phi_ = glm::radians<float>(phi);

	// calculate position vector
	//cameraPos = glm::vec4(sinf(phi_) * cosf(-theta_) * radius, sinf(-theta_) * radius, cosf(phi_) * cosf(-theta_) * radius, 1.0f);

	// calculate orientation basis R
	//R = glm::eulerAngleY(phi_) * glm::eulerAngleX(theta_);
		
	// calculate view and projection transform matrices
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -radius)) * glm::eulerAngleX(-theta_) * glm::eulerAngleY(-phi_);
	projectionMatrix = glm::perspective(glm::radians<float>(fovY), aspect, nearPlane, farPlane);
}


//
// Public method implementation
//

// ArcballCamera constructors

// initialise camera parameters so it is placed at the origin looking down the -z axis (for a right-handed camera) or +z axis (for a left-handed camera)
ArcballCamera::ArcballCamera() {

	theta = 0.0f;
	phi = 0.0f;
	radius = 15.0f;

	fovY = 55.0f;
	aspect = 1.0f;
	nearPlane = 0.1f;
	farPlane = 500.0f;

	//F = ViewFrustum(55.0f, 1.0f, 0.1f, 500.0f);

	// calculate derived values
	calculateDerivedValues();
	//F.calculateWorldCoordPlanes(C, R);
}


// create a camera with orientation <theta, phi> representing Euler angles specified in degrees and Euclidean distance 'init_radius' from the origin.  The frustum / viewplane projection coefficients are defined in init_fovy, specified in degrees spanning the entire vertical field of view angle, init_aspect (w/h ratio), init_nearPlane and init_farPlane.  If init_farPlane = 0.0 (as determined by equalf) then the resulting frustum represents an infinite perspective projection.  This is the default
ArcballCamera::ArcballCamera(float theta, float phi, float radius, float fovY, float aspect, float nearPlane, float farPlane) {

	this->theta = theta;
	this->phi = phi;
	this->radius = std::max<float>(0.0f, radius);

	this->fovY = fovY;
	this->aspect = aspect;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;

	//F = ViewFrustum(init_fovy, init_aspect, init_nearPlane, init_farPlane);

	// calculate derived values
	calculateDerivedValues();
	//F.calculateWorldCoordPlanes(C, R);
}


#pragma region Accessor methods for stored values

// return the pivot rotation around the x axis (theta) in degrees
float ArcballCamera::getTheta() {

	return theta;
}

// return the pivot rotation around the y axis (phi) in degrees
float ArcballCamera::getPhi() {

	return phi;
}

void ArcballCamera::rotateCamera(float dTheta, float dPhi) {

	theta += dTheta;
	phi += dPhi;

	calculateDerivedValues();
}

float ArcballCamera::getRadius() {

	return radius;
}

void ArcballCamera::scaleRadius(float s) {

	radius *= s;
	calculateDerivedValues();
}

void ArcballCamera::incrementRadius(float i) {

	radius = std::max<float>(radius + i, 0.0f);
	calculateDerivedValues();
}

float ArcballCamera::getFovY() {

	return fovY;
}

void ArcballCamera::setFovY(float fovY) {

	this->fovY = fovY;
	calculateDerivedValues();
}

float ArcballCamera::getAspect() {

	return aspect;
}

void ArcballCamera::setAspect(float aspect) {

	this->aspect = aspect;
	calculateDerivedValues();
}

float ArcballCamera::getNearPlaneDistance() {

	return nearPlane;
}

void ArcballCamera::setNearPlaneDistance(float nearPlaneDistance) {

	this->nearPlane = nearPlaneDistance;
	calculateDerivedValues();
}

float ArcballCamera::getFarPlaneDistance() {

	return farPlane;
}

void ArcballCamera::setFarPlaneDistance(float farPlaneDistance) {

	this->farPlane = farPlaneDistance;
	calculateDerivedValues();
}

#pragma endregion


#pragma region Accessor methods for derived values

// return the camera location in world coordinate space
//glm::vec4 ArcballCamera::getPosition() {
//
//	return cameraPos;
//}

// return a const reference to the camera's orientation matrix in world coordinate space
//glm::mat4 ArcballCamera::getOrientationBasis() {
//
//	return R;
//}

// return a const reference to the view transform matrix for the camera
glm::mat4 ArcballCamera::viewTransform() {

	return viewMatrix;
}

// return a const reference the projection transform for the camera
glm::mat4 ArcballCamera::projectionTransform() {

	return projectionMatrix;
}

#pragma endregion
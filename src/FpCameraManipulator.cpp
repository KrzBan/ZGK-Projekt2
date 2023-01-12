#include "FpCameraManipulator.hpp"

osg::Matrixd FpCameraManipulator::getMatrix() const {
	return osg::Matrixd::inverse(getInverseMatrix());
}
osg::Matrixd FpCameraManipulator::getInverseMatrix() const {
	return osg::Matrixd::lookAt(Position, Position + Forward, Up);
}

void FpCameraManipulator::setByMatrix(const osg::Matrixd& matrix) {
	
}

void FpCameraManipulator::setByInverseMatrix(const osg::Matrixd& matrix) {
	
}

void FpCameraManipulator::home(double) {
	Position = {};
	Yaw = 0.0f;
	Pitch = 0.0f;

	UpdateVectors();
}

void FpCameraManipulator::home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
	home(ea.getTime());
}

bool FpCameraManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy) {
	constexpr double speed = 20.0;
	Yaw += speed * dx;
	Pitch += speed * dy;

	constexpr double viewRange = 70;
	Pitch = std::clamp(Pitch, -viewRange, viewRange);

	UpdateVectors();

	return false;
}

bool FpCameraManipulator::performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy) {
	constexpr double speed = 5.0;
	Position += {dx * speed, dy * speed, 0.0};
	return false;
}

void FpCameraManipulator::UpdateVectors() {
	osg::Vec3d front;
	front.x() = sin(osg::DegreesToRadians(Yaw))* cos(osg::DegreesToRadians(Pitch));
	front.y() = cos(osg::DegreesToRadians(Yaw))* cos(osg::DegreesToRadians(Pitch));
	front.z() = sin(osg::DegreesToRadians(Pitch));

	Forward = front;
	Forward.normalize();
	Right = Forward ^ WorldUp;
	Right.normalize();
	Up = Right ^ Forward;
	Up.normalize();
}
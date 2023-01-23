#include "FpCameraManipulator.hpp"

osg::Matrixd FpCameraManipulator::getMatrix() const {
	return osg::Matrixd::inverse(getInverseMatrix());
}
osg::Matrixd FpCameraManipulator::getInverseMatrix() const {
	return osg::Matrixd::lookAt(m_Position, m_Position + m_ForwardVec, m_UpVec);
}

void FpCameraManipulator::setByMatrix(const osg::Matrixd& matrix) {
	
}

void FpCameraManipulator::setByInverseMatrix(const osg::Matrixd& matrix) {
	
}

void FpCameraManipulator::home(double) {
	m_Position = { 0.0, -50.0, 0.0 };
	m_Yaw = 0.0f;
	m_Pitch = 0.0f;

	UpdateVectors();
}

void FpCameraManipulator::home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
	home(ea.getTime());
}

bool FpCameraManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy) {
	RotateView(dx, dy);
	return false;
}

bool FpCameraManipulator::performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy) {
	MoveCamera(dy, dx);
	return false;
}

void FpCameraManipulator::RotateView(double dx, double dy) {
	constexpr double speed = 20.0;
	m_Yaw += speed * dx;
	m_Pitch += speed * dy;

	constexpr double viewRange = 70;
	m_Pitch = std::clamp(m_Pitch, -viewRange, viewRange);

	UpdateVectors();
}
void FpCameraManipulator::MoveCamera(double forward, double right) {
	constexpr double speed = 5.0;

	auto forwardFlat = m_ForwardVec;
	forwardFlat.z() = 0.0;
	forwardFlat.normalize();

	m_Position += forwardFlat * forward;
	m_Position += m_RightVec * right;
}

void FpCameraManipulator::UpdateVectors() {
	osg::Vec3d front;
	front.x() = sin(osg::DegreesToRadians(m_Yaw))* cos(osg::DegreesToRadians(m_Pitch));
	front.y() = cos(osg::DegreesToRadians(m_Yaw))* cos(osg::DegreesToRadians(m_Pitch));
	front.z() = sin(osg::DegreesToRadians(m_Pitch));

	m_ForwardVec = front;
	m_ForwardVec.normalize();
	m_RightVec = m_ForwardVec ^ m_WorldUpVec;
	m_RightVec.normalize();
	m_UpVec = m_RightVec ^ m_ForwardVec;
	m_UpVec.normalize();
}
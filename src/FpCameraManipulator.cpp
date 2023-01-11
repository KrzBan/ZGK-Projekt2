#include "FpCameraManipulator.hpp"

osg::Matrixd FpCameraManipulator::getMatrix() const {
	osg::Matrixd matrix;
	matrix.makeTranslate(0.0f, 0.0f, _distance);
	matrix.postMultTranslate(_center);
	return matrix;
}
osg::Matrixd FpCameraManipulator::getInverseMatrix() const {
	osg::Matrixd matrix;
	matrix.makeTranslate(0.0f, 0.0f, -_distance);
	matrix.preMultTranslate(-_center);
	return matrix;
}

void FpCameraManipulator::setByMatrix(const osg::Matrixd& matrix) {
	setByInverseMatrix(osg::Matrixd::inverse(matrix));
}

void FpCameraManipulator::setByInverseMatrix(const osg::Matrixd& matrix) {
	osg::Vec3d eye, center, up;
	matrix.getLookAt(eye, center, up);

	_center = center; _center.z() = 0.0f;
	if (_node.valid())
		_distance = abs((_node->getBound().center() - eye).z());
	else
		_distance = abs((eye - center).length());
}

void FpCameraManipulator::home(double) {
	if (_node.valid())
	{
		_center = _node->getBound().center();
		_center.z() = 0.0f;
		_distance = 2.5 * _node->getBound().radius();
	}
	else
	{
		_center.set(osg::Vec3());
		_distance = 1.0;
	}
}

void FpCameraManipulator::home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
	home(ea.getTime());
}

bool FpCameraManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy) {
	_center.x() -= 100.0f * dx;
	_center.y() -= 100.0f * dy;
	return false;
}

bool FpCameraManipulator::performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy) {
	_distance *= (1.0 + dy);
	if (_distance < 1.0) _distance = 1.0;
	return false;
}
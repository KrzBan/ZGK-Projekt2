#pragma once

#include "Core.hpp"

class FpCameraManipulator : public osgGA::StandardManipulator {

public:
	FpCameraManipulator() {}
	virtual ~FpCameraManipulator() {}

	virtual osg::Matrixd getMatrix() const;
	virtual osg::Matrixd getInverseMatrix() const;
	virtual void setByMatrix(const osg::Matrixd& matrix);
	virtual void setByInverseMatrix(const osg::Matrixd& matrix);

	virtual void setTransformation(const osg::Vec3d&, const osg::Quat&) {}
	virtual void setTransformation(const osg::Vec3d&, const osg::Vec3d&, const osg::Vec3d&) {}
	virtual void getTransformation(osg::Vec3d&, osg::Quat&) const {}
	virtual void getTransformation(osg::Vec3d&, osg::Vec3d&, osg::Vec3d&) const {}

	virtual void home(double);
	virtual void home(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & us);

	virtual bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy);
	virtual bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy);

	void RotateView(double dx, double dy);
	void MoveCamera(double forward, double right);

private:
	void UpdateVectors();

public:
	// camera Attributes
	osg::Vec3d m_Position = { 0.0, -5.0, 0.0 };
	osg::Vec3d m_WorldUpVec = { 0.0, 0.0, 1.0 };
	osg::Vec3d m_UpVec{};
	osg::Vec3d m_RightVec{};
	osg::Vec3d m_ForwardVec{};

	// euler Angles
	double m_Yaw = 0.0;
	double m_Pitch = 0.0;
};
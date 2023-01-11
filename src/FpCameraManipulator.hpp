#pragma once

#include "Core.hpp"

class FpCameraManipulator : public osgGA::StandardManipulator {

public:
	FpCameraManipulator() : _distance(1.0) {}

	virtual osg::Matrixd getMatrix() const;
	virtual osg::Matrixd getInverseMatrix() const;
	virtual void setByMatrix(const osg::Matrixd& matrix);
	virtual void setByInverseMatrix(const osg::Matrixd& matrix);

	// Leave empty as we don't need these here. They are used by other functions and classes to set up the manipulator directly.
	virtual void setTransformation(const osg::Vec3d&, const osg::Quat&) {}

	virtual void setTransformation(const osg::Vec3d&, const osg::Vec3d&, const osg::Vec3d&) {}
	virtual void getTransformation(osg::Vec3d&, osg::Quat&) const {}
	virtual void getTransformation(osg::Vec3d&, osg::Vec3d&, osg::Vec3d&) const {}

	virtual void home(double);
	virtual void home(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & us);

protected:
	virtual ~FpCameraManipulator() {}

	virtual bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy);
	virtual bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy);

	osg::Vec3 _center;
	double _distance;
};
#include "Ray.hpp"

Ray::Ray(const osg::Vec3d& origin, const osg::Vec3d& direction)
	: m_Origin{ origin }, m_Direction{ direction } {}

osg::Vec3d Ray::Origin() const {
	return m_Origin;
}

osg::Vec3d Ray::Direction() const {
	return m_Direction;
}

osg::Vec3d Ray::At(float t) const {
	return m_Origin + m_Direction * t;
}

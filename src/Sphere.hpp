#pragma once

#include "Core.hpp"
#include "Ray.hpp"
#include "Utils.hpp"

struct Sphere {
	osg::Vec3d m_Position{};
	double m_Radius{};

	explicit Sphere(const osg::Vec3d& position, double radius);

	bool Hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const;
};


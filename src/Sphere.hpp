#pragma once

#include "Core.hpp"
#include "Ray.hpp"
#include "Utils.hpp"

class Sphere {
public:

	osg::Vec3d m_Position{};
	double m_Radius{};


public:
	explicit Sphere(const osg::Vec3d& position, double radius);

	bool Hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const;
};

struct SphereNode {
	Sphere sphere;
	Ref<osg::Node> node;
};
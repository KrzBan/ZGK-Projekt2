#pragma once

#include "Core.hpp"

class Ray {
private:
	osg::Vec3d m_Origin{};
	osg::Vec3d m_Direction{};

public:
	explicit Ray(const osg::Vec3d& origin, const osg::Vec3d& direction);

	osg::Vec3d Origin() const;
	osg::Vec3d Direction() const;

	osg::Vec3d At(float t) const;
};

struct HitInfo {
    osg::Vec3d position;
    osg::Vec3d normal;
    double t;
    bool isFrontFace;

    inline void SetFaceNormal(const Ray& r, const osg::Vec3d& outward_normal) {
        isFrontFace = (r.Direction() * outward_normal) < 0;
        normal = isFrontFace ? outward_normal : -outward_normal;
    }
};
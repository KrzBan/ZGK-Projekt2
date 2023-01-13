#include "Sphere.hpp"

#include "Utils.hpp"


Sphere::Sphere(const osg::Vec3d& position, double radius)
       : m_Position{ position }, m_Radius{ radius } {}

bool Sphere::Hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const {
       osg::Vec3d oc = ray.Origin() - m_Position;
       auto a = ray.Direction().length2();
       auto half_b = oc * ray.Direction();
       auto c = oc.length2() - m_Radius * m_Radius;

       auto discriminant = half_b * half_b - a * c;
       if (discriminant < 0) return false;
       auto sqrtd = std::sqrt(discriminant);
       
       // Find the nearest root that lies in the acceptable range.
       auto root = (-half_b - sqrtd) / a;
       if (root < tMin || tMax < root) {
           root = (-half_b + sqrtd) / a;
           if (root < tMin || tMax < root)
               return false;
       }

       hitInfo.t = root;
       hitInfo.position = ray.At(root);
       osg::Vec3d outwardNormal = (hitInfo.position - m_Position) / m_Radius;
       hitInfo.SetFaceNormal(ray, outwardNormal);

       return true;
}

#ifndef _SPHERE_
#define _SPHERE_

#include "myvector.h"
#include "ray.h"
class sphere
{
 public:
  myvector loc;
  double radius;
  ~sphere(){}
  sphere(){}
 sphere(myvector loc_p,double radius_p):loc(loc_p),radius(radius_p){}
  sphere(const sphere &sphere_p)
    {
      this->loc=sphere_p.loc;
      this->radius=sphere_p.radius;
    }
  double intersect(const ray& ray_here);
};

#endif 

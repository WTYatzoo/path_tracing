#include "sphere.h"

double sphere::intersect(const ray& ray_here)
{
  myvector op=ray_here.origin-this->loc;
  double t,eps=1e-6;
  double b =op.dot(ray_here.dir);
  double det=b*b-op.len_sq()+this->radius*this->radius;
  
  if(det<0) // no intersection
    {
      return -1; 
    }
  else
    {
      det=sqrt(det);
      if(-b-det>=eps)
	{
	  return -b-det;
	}
      else if(-b+det>=eps)
	{
	  return -b+det;
	}
      else
	{
	  return -1; // no intersection
	}
    }
}

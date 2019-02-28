#ifndef _QUAD_
#define _QUAD_
#include "head.h"
#include "myvector.h"
#include "ray.h"
class quad
{
 public:
  myvector vertex[4];
  myvector normal_f;
  myvector vertex_center;
  double d;
  double area;
  ~quad(){}
  quad()
    {
      normal_f=vertex_center=myvector(0,0,0);
    }
  void calNormalAndArea()
    {
      int i;
      this->normal_f.normalize();
      for(i=0;i<4;i++)
	{
	  vertex_center+=vertex[i];
	}
      vertex_center+=normal_f*500;
      
      d=-vertex[0].dot(normal_f);
      area=fabs((vertex[1]-vertex[0]).cross(vertex[3]-vertex[0]).len());
    }
  double intersect(const ray& ray_here);
  
};

#endif 

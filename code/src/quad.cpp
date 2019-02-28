#include "quad.h"

double quad::intersect(const ray& ray_here)
{
  double eps=1e-8;
  double help=normal_f.dot(ray_here.dir);
  if(fabs(help)<=eps) //ray_here 和面平行
    {
      return -1;
    }
  else
    {
      double t=-1*(d+normal_f.dot(ray_here.origin))/help;
      myvector intersect_point=ray_here.origin+t*ray_here.dir;
      bool con1=((intersect_point.x>=vertex[0].x)&&(vertex[2].x>=intersect_point.x))||((vertex[0].x>=intersect_point.x)&&(intersect_point.x>=vertex[2].x));
      bool con2=((intersect_point.y>=vertex[0].y)&&(vertex[2].y>=intersect_point.y))||((vertex[0].y>=intersect_point.y)&&(intersect_point.y>=vertex[2].y));
      bool con3=((intersect_point.z>=vertex[0].z)&&(vertex[2].z>=intersect_point.z))||((vertex[0].z>=intersect_point.z)&&(intersect_point.z>=vertex[2].z));
      if(con1&&con2&&con3)
	{
	  return t;
	}
      else
	{
	  return -1;
	}

    }
}

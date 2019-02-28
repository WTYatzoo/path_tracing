#include "object.h"

double object::intersect(const ray& ray_here,int &id2)
{
  if(this->is_sphere==1)
    {
      return this->mysphere.intersect(ray_here);
    }
  else
    {
      double t=1e20,t_now;
      int i;
      for(i=0;i<num_quad;i++)
	{
	  t_now=myquads[i].intersect(ray_here);
	  if(t_now>0&&t_now<t)
	    {
	      t=t_now;
	      id2=i;
	    }
	}
      if(fabs(t-1e20)<1e-4)
	{
	  return -1; //与该object的所有quad都不交
	}
      else
	{
	  return t; //找到quad集合中最近交点
	}
    }
}

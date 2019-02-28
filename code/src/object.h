#ifndef _OBJECT_
#define _OBJECT_

#include "head.h"
#include "material.h"
#include "sphere.h"
#include "quad.h"
#include "ray.h"
class object
{
 public:
  bool is_sphere; //this is a thick to judge whether the input is a sphere(para with quad ) or just a set of quads
  int num_quad;
  sphere mysphere; // may be used 
  quad myquads[8]; // may be used
  material mymate; // the obj is used with only one kind of mate. so for the input , if the mates are  dif for two quads , they belongs to two obj
  ~object(){}
  object(){
    num_quad=0;
  }
 object(bool is_sphere_p,sphere sphere_p,material mate_p):is_sphere(is_sphere_p),mysphere(sphere_p),mymate(mate_p){
    num_quad=0;
  }
  double intersect(const ray& ray_here,int &id2);
};

#endif

#ifndef _PATH_TRACING_
#define _PATH_TRACING_
#include "ray.h"
#include "object.h"
class path_tracing
{
 public:
  object myobj[12]; // describe the scene
  int num_obj;
  int width,height;
  int default_scene;
  ~path_tracing();
  path_tracing(boost::property_tree::ptree &para_tree);
  double rand_uniform();
  double clamp(double x) ;//截断
  double gamma_correct(double x);
  bool intersect(const ray& ray_here,double &t ,int &id1,int &id2);
  myvector radiance(const ray& ray_here,int depth,int E);
};

#endif 

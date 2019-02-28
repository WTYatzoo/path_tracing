#ifndef _RAY_
#define _RAY_
#include "myvector.h"
class ray
{
 public:
  myvector origin;
  myvector dir;
  ray(myvector origin_p, myvector dir_p):origin(origin_p),dir(dir_p){}
  ~ray(){}
};

#endif

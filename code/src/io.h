#ifndef _IO_
#define _IO_

#include "head.h"
#include "object.h"
#include "myvector.h"

class io
{
 public:
  io(){}
  ~io(){}
  void getObj(object (&myobj)[12],int &num_obj,std::string input_obj,std::string input_mtl);
  void saveImage(myvector **image, int  height,int width, std::string output_path);
};

#endif

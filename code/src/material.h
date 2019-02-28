#ifndef _MATERIAL_
#define _MATERIAL_

#include "myvector.h"
class material
{
 public:
  myvector ka;
  myvector kd;
  myvector ks;
  double ns;
  double tr,ni;
  ~material(){}
  material()
  {
    ka=kd=ks=myvector(0,0,0);
    ns=tr=ni=0;
  }
  material(const material &mate_p)
    {
      this->ka=mate_p.ka; this->kd=mate_p.kd; this->ks=mate_p.ks;
      this->ns=mate_p.ns; this->tr=mate_p.tr; this->ni=mate_p.ni;
    }
 material(myvector ka_p,myvector kd_p,myvector ks_p,double ns_p,double tr_p,double ni_p):ka(ka_p),kd(kd_p),ks(ks_p),ns(ns_p),tr(tr_p),ni(ni_p){}
};
#endif

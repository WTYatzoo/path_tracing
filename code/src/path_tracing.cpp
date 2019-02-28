#include "head.h"
#include "object.h"
#include "io.h"
#include "path_tracing.h"
using namespace std;

double pi=3.1415926535898;
double inverse_pi=1/pi;


object myobj_default[]={
  object(1,sphere(myvector(0,3,0),1.00),material(myvector(1,1,1),myvector(0.8,0.8,0.8),myvector(0,0,0),1,0,1)), // light source
  object(1,sphere(myvector(0,1,0),1.00),material(myvector(0,0,0),myvector(0,0,0),myvector(0.9,0,0),50,0,1)), // perfect specular
  object(1,sphere(myvector(-2,1,0),1.00),material(myvector(0,0,0),myvector(1,0,0),myvector(0,0,0),1,0,1)),   // diffuse
  object(1,sphere(myvector(2,1,0),1.00),material(myvector(0,0,0),myvector(0,0,0),myvector(1,1,1),50,0,1)),//inperfect specular
  object(1,sphere(myvector(1,0.6,3),0.60),material(myvector(0,0,0),myvector(0,0,0),myvector(0,0,0),1,0.9,2.5)),  //refraction
  object(1,sphere(myvector(-1,0.8,4),0.80),material(myvector(0,0,0),myvector(0,0,0),myvector(0,0,0),1,0.9,1.5)),  //refraction
  object(1,sphere(myvector(0,-5000,0),5000),material(myvector(0,0,0),myvector(1,1,1),myvector(0,0,0),0,0,1))  //diffuse
};

object myobj[12]; // describe the scene
int num_obj;
int default_scene;
int width,height;


double rand_uniform()
{
  return (double)rand()/(double)RAND_MAX;
}

double clamp(double x) //截断
{
  if(x>1)
    {
      return 1;
    }
  else if(x<0)
    {
      return 0;
    }
  else
    {
      return x;
    }
}

double gamma_correct(double x)
{
  return pow(clamp(x),1/2.2)*255+0.5; 
}
bool intersect(const ray& ray_here,double &t ,int &id1,int &id2)
{
  t=1e20; // 无穷大的交点　视为　不交
  int i=0;
  double t_now;
  for(i=0;i<num_obj;i++)
    {
      t_now=myobj[i].intersect(ray_here,id2);
      if(t_now>0&&t_now<t)
	{
	  t=t_now;
	  id1=i;
	}
    }
  if(fabs(t-1e20)<1e-4)
    {
      return 0; //与场景都不交
    }
  else
    {
      return 1; //找到最近交点
    }
}

myvector radiance(const ray& ray_here,int depth,int E) // depth: recursive depth  E: whether include 自发光
{
  int i;
  double t;
  int id1=0,id2=0; // id1 for which obj , id2 for which quad if the obj is not a sphere 
  depth++;
  if(intersect(ray_here,t,id1,id2)==0)// no intersection 
    {
      return myvector(0,0,0);
    }
  object intersect_obj=myobj[id1];
  if(depth>10) // too deep to  calculate
    {
      return myvector(0,0,0);
    }
  if(intersect_obj.mymate.ka.x>0||intersect_obj.mymate.ka.y>0||intersect_obj.mymate.ka.z>0)
    {
      return intersect_obj.mymate.ka*E;
    }
  myvector intersect_point=ray_here.origin+ray_here.dir*t;
  myvector normal;
  if(intersect_obj.is_sphere==1)
    {
      normal=(intersect_point-intersect_obj.mysphere.loc).normalize();
    }
  else 
    {
      normal=(intersect_point-intersect_obj.myquads[id2].vertex_center).normalize();
    }
  myvector normal_real=normal.dot(ray_here.dir)<0?normal:normal*-1;
  myvector color_from_refraction,color_from_specular,color_from_recursive_indirect_light,color_from_light_sources;

  color_from_refraction=myvector(0,0,0);
  if(intersect_obj.mymate.tr>0)// 透光能发生折射
    {
      ray reflect_ray=ray(intersect_point,(ray_here.dir-normal_real*2*normal_real.dot(ray_here.dir)).normalize()); 
      bool goin=(normal_real.dot(normal)>0);
      double n_para;
      if(goin)
	{
	  n_para=1/intersect_obj.mymate.ni;
	}
      else
	{
	  n_para=intersect_obj.mymate.ni;
	}
      double cos_dir_n=normal_real.dot(ray_here.dir);
      double cos_help=1-n_para*n_para*(1-cos_dir_n*cos_dir_n);
      if(cos_help<0) //入射角过大时全反射，无折射
	{
	  return  intersect_obj.mymate.ka*E+radiance(reflect_ray,depth,1);
	}
      else
	{
	  myvector in_dir=(n_para*ray_here.dir-normal_real*(cos_dir_n*n_para+sqrt(cos_help))).normalize();
	  double R0=(1-intersect_obj.mymate.tr);
	  double R_theta=R0+intersect_obj.mymate.tr*pow(1-fabs(cos_dir_n),5);
	  double Pro=0.25+0.5*R_theta;
	  double help1=R_theta/Pro; double help2=(1-R_theta)/(1-Pro);
	  color_from_refraction=help1*radiance(reflect_ray,depth,1)+help2*radiance(ray(intersect_point,in_dir),depth,1);
	  return intersect_obj.mymate.ka*E+color_from_refraction;
	}
    }

  color_from_specular=myvector(0,0,0);
   myvector ks_theta=myvector(0,0,0);
  if(intersect_obj.mymate.ks.x<=0&&intersect_obj.mymate.ks.y<=0&&intersect_obj.mymate.ks.z<=0)
    {
      ;
    }
  else
    {
      int sample_specular_brdf=1; int ss;
      for(ss=0;ss<sample_specular_brdf;ss++)
	{
	  myvector specular_reference=(ray_here.dir-normal_real*2*normal_real.dot(ray_here.dir)).normalize();
	  double r1=rand_uniform();
	  double r2=rand_uniform();
	  double cos_a=pow(r1,1/(intersect_obj.mymate.ns+1));
	  double sin_a=sqrt(1-cos_a*cos_a);
	  double phi=2*pi*r2;
	  myvector w=ray_here.dir;
	  myvector u;
	  if(fabs(w.x)>0.3)
	    {
	      u=myvector(-1*(w.y*w.y+w.z*w.z)/w.x,w.y,w.z).normalize();
	    }
	  else if(fabs(w.y)>0.3)
	    {
	      u=myvector(w.x,-1*(w.x*w.x+w.z*w.z)/w.y,w.z).normalize();
	    }
	  else 
	    {
	      u=myvector(w.x,w.y,-1*(w.x*w.x+w.y*w.y)/w.z).normalize();
	    }
	  myvector v=w.cross(u).normalize();
	  myvector specular_ray_dir=(u*cos(phi)*sin_a+v*sin(phi)*sin_a+w*cos_a).normalize();
	  myvector specular_reference_here=(specular_ray_dir-normal_real*2*normal_real.dot(specular_ray_dir)).normalize();
	  double help=pow(cos_a,intersect_obj.mymate.ns);

	  if(default_scene==1)
	    {
	      if(id1==3)// phong specular
		{
		  if(specular_reference_here.dot(normal_real)<0)
		    {
		      ;
		    }
		  else
		    {
		      ks_theta=help*intersect_obj.mymate.ks;
		      color_from_specular+=(1/sample_specular_brdf)*ks_theta.multi(radiance(ray(intersect_point,specular_reference_here),depth,1));
		    }	      
		}
	      else if(id1==1) // perfect specular
		{
		  color_from_specular+=(1/sample_specular_brdf)*intersect_obj.mymate.ks.multi(radiance(ray(intersect_point,specular_reference),depth,1));
		}
	      color_from_specular+=(1/sample_specular_brdf)*intersect_obj.mymate.ks.multi(radiance(ray(intersect_point,specular_reference),depth,1));
	    }
	  else
	    {
	      if(specular_reference_here.dot(normal_real)<0)
		{
		  ;
		}
	      else
		{
		  ks_theta=help*intersect_obj.mymate.ks;
		  color_from_specular+=(1/sample_specular_brdf)*ks_theta.multi(radiance(ray(intersect_point,specular_reference_here),depth,1));
		  }

	      //   color_from_specular+=(1/sample_specular_brdf)*intersect_obj.mymate.ks.multi(radiance(ray(intersect_point,specular_reference),depth,1));
	    }
        }
        return intersect_obj.mymate.ka*E+color_from_specular;
    }
  
  myvector kd=intersect_obj.mymate.kd; 
  // here just test diffuse

  color_from_recursive_indirect_light=myvector(0,0,0); //caused by diffuse
  color_from_light_sources=myvector(0,0,0);
  double p=(kd.x>kd.y&&kd.x>kd.z)?kd.x:(kd.y>kd.z)?kd.y:kd.z;
  if(depth>5||!p)
    {
      if(rand_uniform()<p)
	{
	  kd=kd*(1/p);
	}
      else
	{
	  return intersect_obj.mymate.ka*E;
	}
    }
  if(kd.x<=0&&kd.y<=0&&kd.z<=0)
    {
      ;
    }
  else
    {
      int sample_recursive=1,sr;  
      for(sr=0;sr<sample_recursive;sr++)
	{
	  double r1=2*pi*rand_uniform();
	  double r2=rand_uniform(); double r2s=sqrt(r2);
	  myvector w=normal_real;
	  myvector u;
	  if(fabs(w.x)>0.3)
	    {
	      u=myvector(-1*(w.y*w.y+w.z*w.z)/w.x,w.y,w.z).normalize();
	    }
	  else if(fabs(w.y)>0.3)
	    {
	      u=myvector(w.x,-1*(w.x*w.x+w.z*w.z)/w.y,w.z).normalize();
	    }
	  else 
	    {
	      u=myvector(w.x,w.y,-1*(w.x*w.x+w.y*w.y)/w.z).normalize();
	    }
	  myvector v=w.cross(u).normalize();
	  myvector shadow_ray_dir=(u*cos(r1)*r2s+v*sin(r1)*r2s+w*sqrt(1-r2)).normalize(); // build shadow ray for recursively searching
	  color_from_recursive_indirect_light+=(1/sample_recursive)*kd.multi(radiance(ray(intersect_point,shadow_ray_dir),depth,0));
	}

      // for diffuse reflection, only shadow ray to recursively search is not efficient , so directly sampling light sources is needed . here we do this step
      
      {	
	for(i=0;i<num_obj;i++)
	  {
	    object obj_here=myobj[i];
	    if(obj_here.mymate.ka.x<=0&&obj_here.mymate.ka.y<=0&&obj_here.mymate.ka.z<=0)
	      {
		continue; // this sphere is not light source
	      }

	    if(obj_here.is_sphere==1)
	      {
		myvector sw=obj_here.mysphere.loc-intersect_point;
		myvector su;
		if(fabs(sw.x)>0.3)
		  {
		    su=myvector(-1*(sw.y*sw.y+sw.z*sw.z)/sw.x,sw.y,sw.z).normalize();
		  }
		else if(fabs(sw.y)>0.3)
		  {
		    su=myvector(sw.x,-1*(sw.x*sw.x+sw.z*sw.z)/sw.y,sw.z).normalize();
		  }
		else
		  {
		    su=myvector(sw.x,sw.y,-1*(sw.x*sw.x+sw.y*sw.y)/sw.z).normalize();
		  }
		myvector sv=sw.cross(su).normalize();
		double cos_a_max=sqrt(1-obj_here.mysphere.radius*obj_here.mysphere.radius/sw.len_sq());

		int sample_light_source=1,sl;
		for(sl=0;sl<sample_light_source;sl++)
		  {
		    double r1=rand_uniform(); double r2=rand_uniform();
		    double cos_a=1-r1+r1*cos_a_max;
		    double sin_a=sqrt(1-cos_a*cos_a);
		    double phi=2*pi*r2;
		    myvector dir_here=(su*cos(phi)*sin_a+sv*sin(phi)*sin_a+sw*cos_a).normalize();
		    if(intersect(ray(intersect_point,dir_here),t,id1,id2)&&id1==i)
		      {
			double omega=2*pi*(1-cos_a_max);
			color_from_light_sources+=(1/sample_light_source)*kd.multi(obj_here.mymate.ka*dir_here.dot(normal_real)*omega)*inverse_pi;
		      }
		  }
	      }
	    else
	      {
		int count_quad=0;  int num_quad=obj_here.num_quad;
		for(count_quad=0;count_quad<num_quad;count_quad++)
		  {
		    int sample_light_source=1, sl;
		    for(sl=0;sl<sample_light_source;sl++)
		      {
			double r1=rand_uniform(); double r2=rand_uniform();
			myvector point_may=obj_here.myquads[count_quad].vertex[0]+r1*(obj_here.myquads[count_quad].vertex[1]-obj_here.myquads[count_quad].vertex[0])+r2*(obj_here.myquads[count_quad].vertex[3]-obj_here.myquads[count_quad].vertex[0]);
			myvector dir_here=point_may-intersect_point;
			if(intersect(ray(intersect_point,dir_here/dir_here.len()),t,id1,id2)&&id1==i&&id2==count_quad)
			  {
			    double omega=obj_here.myquads[count_quad].area*fabs(normal_real.dot(dir_here)*(point_may-obj_here.myquads[count_quad].vertex_center).normalize().dot(dir_here))/pow(dir_here.len_sq(),1);
			    color_from_light_sources+=(1/sample_light_source)*omega*kd.multi(obj_here.mymate.ka)*inverse_pi;
			  }
		      }
		  }
	      }
	  }
      }
      
        return intersect_obj.mymate.ka*E+color_from_light_sources+color_from_recursive_indirect_light;
    }
  //   return  intersect_obj.mymate.ka*E+color_from_refraction+color_from_specular+color_from_recursive_indirect_light+color_from_light_sources;
}

void readcmdline(int argc, char* argv[],boost::property_tree::ptree &para_tree)
{
  int i;
  for(i=1;i<argc;i++)
    {
      string para_here=argv[i];
      size_t pos=para_here.find("=");
      if(pos!= string::npos)
	{
	  string key=para_here.substr(0,pos);
	  string value=para_here.substr(pos+1);
	  para_tree.put(key+".value",value);
	  printf("--[cmdline para] %s %s \n",key.c_str(),value.c_str());
	}
    }
  return;
}

int main(int argc, char *argv[])
{
  boost::property_tree::ptree para_tree;
  readcmdline(argc,argv,para_tree);
  
  srand(pow(time(0),1/3)); // set random seed and filter it
  io myio=io();
  int i,j,a,b,s;
  default_scene=para_tree.get<int>("default_scene.value",1); //　是否使用默认场景
  if(default_scene==1)
    {
      num_obj=sizeof(myobj_default)/sizeof(object);
      for(i=0;i<num_obj;i++)
	{
	  myobj[i]=myobj_default[i];
	}
      printf("num_obj: %d\n",num_obj);
    }
  else
    {
      num_obj=0;
      string input_obj=para_tree.get<string>("input_obj.value");
      string input_mtl=para_tree.get<string>("input_mtl.value");
      myio.getObj(myobj,num_obj,input_obj,input_mtl);
      printf("num_obj: %d\n",num_obj);
    }
  width=para_tree.get<int>("width.value",1024);  height=para_tree.get<int>("height.value",1024);
  myvector eye=myvector(para_tree.get<double>("ex.value",0),para_tree.get<double>("ey.value",2),para_tree.get<double>("ez.value",20));
  myvector camera_dir=myvector(para_tree.get<double>("dir_x.value",0),para_tree.get<double>("dir_y.value",-0.4),para_tree.get<double>("dir_z.value",-1));
  ray camera=ray(eye,camera_dir.normalize());
  myvector cx=myvector(1,0,0)*para_tree.get<double>("cx.value",1);
  myvector cy=myvector(0,1,0)*para_tree.get<double>("cy.value",1);
  
  myvector **image=(myvector**) new myvector*[height]; 
  for(i=0;i<height;i++)
    {
      image[i]=(myvector*)new myvector[width];
      for(j=0;j<width;j++)
	{
	  image[i][j]=myvector(0,0,0);
	}
    }
  int sample=para_tree.get<int>("sample.value",40); // each subpixel`s sample num
  int depth_of_field=para_tree.get<int>("depth_of_field.value",0);  // whether there is depth of field 
  for(i=0;i<height;i++)
    {
      for(j=0;j<width;j++)
	{
	  for(a=0;a<2;a++)
	    {
	      for(b=0;b<2;b++)
		{
		  myvector color=myvector(0,0,0);
		  for(s=0;s<sample;s++)
		    {
		      double rand1=rand_uniform()*2; 
		      double rand2=rand_uniform()*2;
		      double da=rand1<1?sqrt(rand1)-1:1-sqrt(2-rand1);
		      double db=rand2<1?sqrt(rand2)-1:1-sqrt(2-rand2);
		      myvector dir_now=cx*(((a+0.5+da)/2+j)/width-0.5)+cy*(((b+0.5+db)/2+i)/height-0.5)+camera.dir;

		      double sample_r;
		      if(depth_of_field==1)
			{
			  sample_r=para_tree.get<double>("sample_r.value",0.2);
			}
		      else
			{
			  sample_r=0;
			}
		      rand1=sqrt(rand_uniform())*sample_r;
		      rand2=rand_uniform()*2*pi;
		      myvector dxdy=myvector(rand1*cos(rand2),0,rand1*sin(rand2)); //针孔镜头变成圆形镜头，需要采样
		      double help=para_tree.get<double>("focal_plane.value",2);//焦平面的z轴量
		      double t=(help-(camera.origin+dir_now).z)/dir_now.z;
		      myvector loc=camera.origin+dir_now+t*dir_now;
		      color+=radiance(ray(camera.origin+dxdy+dir_now,(loc-dxdy-camera.origin-dir_now).normalize()),0,1)*(1.0/sample); 
		    }
		  image[i][j]+=myvector(clamp(color.x),clamp(color.y),clamp(color.z))*0.25;
		}
	    }
	    printf("%d %d \n",i,j);
	}
    }

  for(i=0;i<height;i++) 
    {	  
      for(j=0;j<width;j++)
	{
	  image[i][j]=myvector(gamma_correct(image[i][j].x),gamma_correct(image[i][j].y),gamma_correct(image[i][j].z));
	}
    }
  string output_path=para_tree.get<string>("output_path.value","./image.ppm");
  myio.saveImage(image,width,height,output_path);
  
  for(i=0;i<height;i++)
    {      
      delete [] image[i];
    }
  delete [] image;
  
  return 0;
}


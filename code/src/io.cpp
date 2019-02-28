#include "io.h"
#include "material.h"

using namespace std;
void io::getObj(object (&myobj)[12],int &num_obj,std::string input_obj,std::string input_mtl)
{
  size_t i,j;
  vector<material > mymate;
  while(!mymate.empty())
    {
      mymate.pop_back();
    }
  map<string,int > mp_from_mate_to_index;
  mp_from_mate_to_index.clear();
  FILE* fp_mtl=fopen(input_mtl.c_str(),"r");
  char filter[30];
  string filter_string;
  fscanf(fp_mtl,"%s",filter);
  int index=0; int num_filter;
  double para[3];
  int index_para[3];
  map<string,int > mp_help;  mp_help["Ka"]=0; mp_help["Kd"]=1; mp_help["Ks"]=2; mp_help["Ns"]=3; mp_help["Tr"]=4; mp_help["Ni"]=5;
  while(filter[0]=='n'&&filter[1]=='e'&&filter[2]=='w')
    {
      material mate_now=material();
      fscanf(fp_mtl,"%s",filter);
      filter_string=filter;
      mp_from_mate_to_index[filter_string]=index;
      fscanf(fp_mtl,"%s %d",filter,&num_filter);
      while(!feof(fp_mtl)&&fscanf(fp_mtl,"%s",filter)&&!(filter[0]=='n'&&filter[1]=='e'&&filter[2]=='w'))
	{
	  filter_string=filter;
	  switch(mp_help[filter_string])
	  {
	  case 0:
	    fscanf(fp_mtl,"%lf %lf %lf ",&para[0],&para[1],&para[2]);
	    mate_now.ka=myvector(para[0],para[1],para[2]);
	    break;
	  case 1:
	    fscanf(fp_mtl,"%lf %lf %lf ",&para[0],&para[1],&para[2]);
	    mate_now.kd=myvector(para[0],para[1],para[2]);
	    break;
	  case 2:
	    fscanf(fp_mtl,"%lf %lf %lf ",&para[0],&para[1],&para[2]);
	    mate_now.ks=myvector(para[0],para[1],para[2]);
	    break;
	  case 3:
	    fscanf(fp_mtl,"%lf",&para[0]);
	    mate_now.ns=para[0];
	    break;
	  case 4:
	    fscanf(fp_mtl,"%lf",&para[0]);
	    mate_now.tr=para[0];
	    break;
	  case 5:
	    fscanf(fp_mtl,"%lf",&para[0]);
	    mate_now.ni=para[0];
	    break;
	  }
	}
      mymate.push_back(mate_now);
      index++;
      //回到　"newmtl"
    }
  fclose(fp_mtl);
  vector<myvector > myvertex,mynormal;
  while(!myvertex.empty())
    {
      myvertex.pop_back();
    }
  while(!mynormal.empty())
    {
      mynormal.pop_back();
    }
  FILE* fp_obj=fopen(input_obj.c_str(),"r");
  do
   {
     fscanf(fp_obj,"%s",filter);
   }while(filter[0]!='g');

  bool is_sphere;
  while(filter[0]=='g')
    {
      fscanf(fp_obj,"%s",filter);
      fscanf(fp_obj,"%s",filter);
      myvector now=myvector();
      while(filter[0]=='v'&&filter[1]=='\0')
	{
	  fscanf(fp_obj,"%lf %lf %lf",&now.x,&now.y,&now.z);
	  myvertex.push_back(now);
	  fscanf(fp_obj,"%s",filter);
	}
      while(filter[0]=='v'&&filter[1]=='t')
	{
	  fscanf(fp_obj,"%lf %lf %lf",&now.x,&now.y,&now.z);
	  fscanf(fp_obj,"%s",filter);
	}
      while(filter[0]=='v'&&filter[1]=='n')
	{
	  fscanf(fp_obj,"%lf %lf %lf",&now.x,&now.y,&now.z);
	  mynormal.push_back(now);
	  fscanf(fp_obj,"%s",filter);
	}
      if(filter[0]=='s')
	{
	  fscanf(fp_obj,"%s",filter);
	  if(filter[0]=='1')
	    {
	      is_sphere=1;
	    }
	  else
	    {
	      is_sphere=0;
	    }
	}
      fscanf(fp_obj,"%s",filter); fscanf(fp_obj,"%s",filter); 
      fscanf(fp_obj,"%s",filter);
      while(filter[0]=='u'&&filter[1]=='s'&&filter[2]=='e')
	{
	  fscanf(fp_obj,"%s",filter); filter_string=filter;
	  material mate_now=mymate[mp_from_mate_to_index[filter_string]];
	  object obj_now; obj_now.is_sphere=is_sphere; obj_now.mymate=mate_now;
	  fscanf(fp_obj,"%s",filter);
	  int max_index=0; int min_index=1e5;
	  int num_quad=0;
	  while(filter[0]=='f'&&!feof(fp_obj))
	    {
	      if(is_sphere==1)
		{
		  //统计最大顶点索引和最小顶点索引
		  for(i=0;i<4;i++)
		    {
		      fscanf(fp_obj,"%d/%d/%d",&index_para[0],&index_para[1],&index_para[2]);
		      if(index_para[0]>max_index)
			{
			  max_index=index_para[0];
			}
		      if(index_para[0]<min_index)
			{
			  min_index=index_para[0];
			}
		    }
		}
	      else if(is_sphere==0)
		{
		  quad quad_now;
		  for(i=0;i<4;i++)
		    {
		      fscanf(fp_obj,"%d/%d/%d",&index_para[0],&index_para[1],&index_para[2]);
		      quad_now.vertex[i]=myvertex[index_para[0]-1];
		      quad_now.normal_f+=mynormal[index_para[2]-1];
		    }
		  quad_now.calNormalAndArea();
		  obj_now.myquads[num_quad]=quad_now;
		  num_quad++;
		}
	      fscanf(fp_obj,"%s",filter);
	    }
	  obj_now.num_quad=num_quad;
	  if(is_sphere==1)
	    {
	      min_index--; max_index--;
	      myvector loc=myvector(0,0,0); double radius=0,temp_r;
	      for(i=min_index;i<=max_index;i++)
		{
		  loc+=myvertex[i];
		}
	      loc/=(max_index-min_index+1);
	      for(i=min_index;i<=max_index;i++)
		{
		  temp_r=(myvertex[i]-loc).len(); radius+=temp_r;
		}
	      radius/=(max_index-min_index+1);
	      obj_now.mysphere=sphere(loc,radius);
	    }	  
	  myobj[num_obj]=obj_now;
	  num_obj++;
	}
      // 又回到 "g"
    }
  fclose(fp_obj);
  return ;
}

void io::saveImage(myvector **image, int  width,int height, std::string output_path)
{
  FILE* fp=fopen(output_path.c_str(),"w");
  fprintf(fp,"P3\n%d %d\n%d\n",width,height,255);
  int i,j;
  // 从上到下 从左到右 右手坐标系
  for(i=height-1;i>=0;i--) 
    {
      for(j=0;j<width;j++)
	{
	  fprintf(fp,"%d %d %d ",int(image[i][j].x),int(image[i][j].y),int(image[i][j].z));
	}
    }
  fclose(fp);
  return ;
}

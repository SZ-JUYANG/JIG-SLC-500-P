
#include "CommonFunction.h"

u32 Float2Long(float tdata)
{
	union
  {
	  float fd;
		u32 ld;
	}tempdata;
	tempdata.fd=tdata;
	return tempdata.ld;
}


float Long2Float(u32 tdata)
{
	union
  {
	  float fd;
		u32 ld;
	}tempdata;
	tempdata.ld=tdata;
	return tempdata.fd;
}








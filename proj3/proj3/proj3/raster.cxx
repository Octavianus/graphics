/************************************************************************

  Raster image support.
  
  $Id: raster.cxx,v 1.6 2000/08/01 22:33:28 garland Exp $

 ************************************************************************/

#include "raster.h"

#include <stdio.h>
#include <memory.h>
#include <string>
#include <cctype>

FloatRaster::FloatRaster(const ByteRaster &img)
    : Raster<float>(img.width(), img.height(), img.channels())
{
    for(int i=0; i<length(); i++)
	(*this)[i] = (float)img[i] / 255.0f;
}

FloatRaster::FloatRaster(const FloatRaster &img)
    : Raster<float>(img.width(), img.height(), img.channels())
{
    memcpy(head(), img.head(), img.length()*sizeof(float));
}

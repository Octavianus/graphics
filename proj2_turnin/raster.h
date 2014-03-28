#ifndef GFXRASTER_INCLUDED // -*- C++ -*-
#define GFXRASTER_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Raster image support.
    
  $Id: raster.h,v 1.4 2000/08/01 17:42:34 garland Exp $

 ************************************************************************/

template<class T>
class Raster
{
public:
    T *data;
    int W, H, nchan;

    Raster(int w, int h, int c)
    {
	W=w; H=h; nchan=c;
	data = new T[length()];
    }
    ~Raster() { delete data; }

    int width() const { return W; }
    int height() const { return H; }
    int channels() const { return nchan; }
    int length() const { return W*H*nchan; }

    T& operator[](int i)       { return data[i]; }
    T  operator[](int i) const { return data[i]; }

    T       *pixel(int i, int j)       { return data + (j*W + i)*nchan; }
    const T *pixel(int i, int j) const { return data + (j*W + i)*nchan; }

    T       *head()       { return data; }
    const T *head() const { return data; }

    void reverse(int start=0, int end=-1);
    void hflip();
    void vflip();
};

typedef Raster<unsigned char> ByteRaster;

class FloatRaster : public Raster<float>
{
public:
    FloatRaster(int w, int h, int c) : Raster<float>(w,h,c) {}
    FloatRaster(const FloatRaster &img);
    FloatRaster(const ByteRaster &img);
};


////////////////////////////////////////////////////////////////////////
//
// Templated raster methods
//

template<class T>
inline void Raster<T>::reverse(int start, int end)
{
    if(end<0 || end>=length()) end = length() - channels();

    int i=start, j=end;
    
    while(i<j)
    {
	for(int k=0; k<channels(); k++)
	{
	    T tmp = (*this)[i+k];
	    (*this)[i+k] = (*this)[j+k];
	    (*this)[j+k] = tmp;
	}
	i += channels();
	j -= channels();
    }
}

template<class T>
inline void Raster<T>::hflip()
{
     int i = 0;
     int j = channels()*(width()-1);

     while( i<length() )
     {
	 reverse(i, i+j);
	 i += j + channels();
     }
}

template<class T>
inline void Raster<T>::vflip()
{
    reverse();
    hflip();
}


////////////////////////////////////////////////////////////////////////
//
// Supported external image file formats.
//

// JPEG support provided through libjpeg (if available)
extern int jpeg_output_quality;
extern bool write_jpeg_image(const char *filename, const ByteRaster&);
extern ByteRaster *read_jpeg_image(const char *filename);

// GFXRASTER_INCLUDED
#endif

#include "filter.h"


int SobelFilter(int Th, GzDisplay *sobel)
{
	GzDisplay *temp;
    GzNewDisplay(&temp, GZ_RGBAZ_DISPLAY, WIDTH, HEIGHT);
	memset(temp->fbuf,0,sizeof(GzPixel)*WIDTH*HEIGHT);
	for(int y = 1; y < HEIGHT-1; ++y)
	{
		for (int x = 1; x < WIDTH-1; ++x)
		{
			int Ixr,Ixg,Ixb, Iyr,Iyg,Iyb, Ixy;
			
			Ixr = (sobel->fbuf[ARRAY2(x+1,y+1)].red - sobel->fbuf[ARRAY2(x-1,y+1)].red) 
			+ 2 * (sobel->fbuf[ARRAY2(x+1,y)].red - sobel->fbuf[ARRAY2(x-1,y)].red)
			+ (sobel->fbuf[ARRAY2(x+1,y-1)].red - sobel->fbuf[ARRAY2(x-1,y-1)].red);

			Ixg = (sobel->fbuf[ARRAY2(x+1,y+1)].green - sobel->fbuf[ARRAY2(x-1,y+1)].green) 
			+ 2 * (sobel->fbuf[ARRAY2(x+1,y)].green - sobel->fbuf[ARRAY2(x-1,y)].green)
			+ (sobel->fbuf[ARRAY2(x+1,y-1)].green - sobel->fbuf[ARRAY2(x-1,y-1)].green);

			Ixb = (sobel->fbuf[ARRAY2(x+1,y+1)].blue - sobel->fbuf[ARRAY2(x-1,y+1)].blue) 
			+ 2 * (sobel->fbuf[ARRAY2(x+1,y)].blue - sobel->fbuf[ARRAY2(x-1,y)].blue)
			+ (sobel->fbuf[ARRAY2(x+1,y-1)].blue - sobel->fbuf[ARRAY2(x-1,y-1)].blue);


			Iyr = (sobel->fbuf[ARRAY2(x-1,y-1)].red - sobel->fbuf[ARRAY2(x-1,y+1)].red)
			+ 2 * (sobel->fbuf[ARRAY2(x,y-1)].red - sobel->fbuf[ARRAY2(x,y+1)].red)
			+ (sobel->fbuf[ARRAY2(x+1,y-1)].red - sobel->fbuf[ARRAY2(x+1,y+1)].red);

			Iyg = (sobel->fbuf[ARRAY2(x-1,y-1)].green - sobel->fbuf[ARRAY2(x-1,y+1)].green)
			+ 2 * (sobel->fbuf[ARRAY2(x,y-1)].green - sobel->fbuf[ARRAY2(x,y+1)].green)
			+ (sobel->fbuf[ARRAY2(x+1,y-1)].green - sobel->fbuf[ARRAY2(x+1,y+1)].green);

			Iyb = (sobel->fbuf[ARRAY2(x-1,y-1)].blue - sobel->fbuf[ARRAY2(x-1,y+1)].blue)
			+ 2 * (sobel->fbuf[ARRAY2(x,y-1)].blue - sobel->fbuf[ARRAY2(x,y+1)].blue)
			+ (sobel->fbuf[ARRAY2(x+1,y-1)].blue - sobel->fbuf[ARRAY2(x+1,y+1)].blue);
						
			
			Ixy = sqrt(float(Ixr)*float(Ixr) + float(Ixg)*float(Ixg) + float(Ixb)*float(Ixb) 
				+ float(Iyr)*float(Iyr) + float(Iyg)*float(Iyg) + float(Iyb)*float(Iyb));
			
			if (Ixy > Th)
			{
				temp->fbuf[ARRAY2(x,y)].red = 100;
			}
		}
	}
	for(int y = 1; y < HEIGHT-1; ++y)
	{
		for (int x = 1; x < WIDTH-1; ++x)
		{
			
			if (temp->fbuf[ARRAY2(x,y)].red == 100)
			{
				 sobel->fbuf[ARRAY2(x,y)].red = 0;
				 sobel->fbuf[ARRAY2(x,y)].green = 0;
				 sobel->fbuf[ARRAY2(x,y)].blue = 0;



			}
		}
	}

	return GZ_SUCCESS;
}
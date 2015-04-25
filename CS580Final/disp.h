/* 
*  disp.h -- include file for Display
*  USC csci 580 
*/

#pragma once

#include "gz.h"
#include <stdio.h>

/* define general display pixel-type */
#ifndef GZ_PIXEL
struct GzPixel{
  GzIntensity    red;
  GzIntensity    green;
  GzIntensity    blue;
  GzIntensity    alpha;
  GzDepth	 z;
};
#define GZ_PIXEL
#endif


/* define a display type */
#ifndef GZ_DISPLAY
struct GzDisplay{
  unsigned short	xres;
  unsigned short	yres;
  GzDisplayClass	dispClass;
  short			open;
  GzPixel		*fbuf;		/* frame buffer array */
};
#define GZ_DISPLAY
#endif

#define	MAXXRES	1024	/* put some bounds on size in case of error */
#define	MAXYRES	1024

#define RGBSCALE 4096   /*RGB scale*/

#define	ARRAY(x,y)	(x+(y*display->xres))	/* simplify fbuf indexing */

// Function declaration
int GzNewFrameBuffer(char** framebuffer, int width, int height);
int GzNewDisplay(GzDisplay **display, GzDisplayClass dispClass, int xRes, int yRes);
int GzFreeDisplay(GzDisplay *display);
int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass);
int GzInitDisplay(GzDisplay *display);
int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z);
int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z);
int GzFlushDisplay2File(FILE* outfile, GzDisplay *display);
int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay* display);
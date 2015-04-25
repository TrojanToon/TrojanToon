/*
 *  disp.cpp -- definition file for Display
 *  USC csci 580
 */

#include "gz.h"
#include "disp.h"
#define Z_MAX (2e9)


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
    /* create a framebuffer:
     -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
     -- pass back pointer
     */
    if (width > MAXXRES || height > MAXYRES) {
        return GZ_FAILURE;
    }
    
    (* framebuffer) =(char *)(new GzPixel[width * height]);
    return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay **display, GzDisplayClass dispClass, int xRes, int yRes)
{
    /* create a display:
     -- allocate memory for indicated class and resolution
     -- pass back pointer to GzDisplay object in display
     */
    if (xRes > MAXXRES || yRes > MAXYRES) {
        return GZ_FAILURE;
    }
    (* display) = new GzDisplay();
    (* display)->xres = xRes;
    (* display)->yres = yRes;
    (* display)->dispClass = dispClass;
    int status = GzNewFrameBuffer( (char**) & ((* display)->fbuf), xRes, yRes);
    if (status) {
        return GZ_FAILURE;
    }
    return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay *display)
{
    /* clean up, free memory */
    delete [] display->fbuf;
    delete display;
    return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
    /* pass back values for an open display */
    (* xRes) = display->xres;
    (* yRes) = display->yres;
    (* dispClass) = display->dispClass;
    return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay *display)
{
    /* set everything to some default values - start a new frame */
    for (int j = 0; j < display->yres; j++) {
        for (int i = 0; i < display->xres; i++) {
            GzPutDisplay(display,i,j,0.5*RGBSCALE,0.4375*RGBSCALE,0.375*RGBSCALE,1,Z_MAX);
        }
    }
    display->open = 1;
    return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
    /* write pixel values into the display */
    if(i >= display->xres || j >= display->yres || i < 0 || j < 0){
        return GZ_FAILURE;
    }
    int pixel = ARRAY(i,j);
    if (r > RGBSCALE-1) {
        r = RGBSCALE-1;
    }
    if (g > RGBSCALE-1) {
        g = RGBSCALE-1;
    }
    if (b > RGBSCALE-1) {
        b = RGBSCALE-1;
    }
    //printf("%d\n",z);
    if(z > display->fbuf[pixel].z && display->open) return GZ_SUCCESS;
    display->fbuf[pixel].red = r;
    display->fbuf[pixel].green = g;
    display->fbuf[pixel].blue = b;
    display->fbuf[pixel].alpha = a;
    display->fbuf[pixel].z = z;
    return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
    /* pass back pixel value in the display */
    /* check display class to see what vars are valid */
    if (display->dispClass == GZ_RGBAZ_DISPLAY) {
        if(i >= display->xres || j >= display->yres || i < 0 || j < 0){
            return GZ_FAILURE;
        }
        int pixel = ARRAY(i,j);
        (* r) = display->fbuf[pixel].red;
        (* g) = display->fbuf[pixel].green;
        (* b) = display->fbuf[pixel].blue;
        (* a) = display->fbuf[pixel].alpha;
        (* z) = display->fbuf[pixel].z;
    }
    return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{
    /* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
    fprintf(outfile,"P3\n");
    fprintf(outfile,"%d %d\n",display->xres,display->yres);
    fprintf(outfile,"%d\n",RGBSCALE-1);
    for (int j = 0; j < display->yres; j++) {
        for (int i = 0; i < display->xres; i++) {
            GzIntensity r,g,b,a;
            GzDepth z;
            GzGetDisplay(display,i,j,&r,&g,&b,&a,&z);
            fprintf(outfile,"%d %d %d ",r,g,b);
        }
        fprintf(outfile,"\n");
    }
    return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{
    /* write pixels to framebuffer:
     - Put the pixels into the frame buffer
     - Caution: store the pixel to the frame buffer as the order of blue, green, and red
     - Not red, green, and blue !!!
     */
    
    
    return GZ_SUCCESS;
}
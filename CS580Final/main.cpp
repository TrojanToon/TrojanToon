/**
 * Main entry point
 * USC csci 580 *
*/

#include "Application6.h"
#include "gz.h"
#include "disp.h"
#include "rend.h"
#include "filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTFILE "output.ppm"

#define	AAKERNEL_SIZE	6

float AAFilter[AAKERNEL_SIZE][3] =			/* X, Y, coef */
{
    {-0.52, 0.38, 0.128},
    {0.41, 0.56, 0.119},
    {0.27, 0.08, 0.294},
    {-0.17, -0.29, 0.249},
    {0.58, -0.55, 0.104},
    {-0.31, -0.71, 0.106}
};

int main(int argc, const char * argv[])
{
    GzDisplay *outputDepthMap, *outputNormalMap, *outputTexture,  *final;
    const int width = 256;
    const int height = 256;
    int edges[width*height];
    memset(edges,0,sizeof(int)*width*height);

    GzNewDisplay(&outputTexture, GZ_RGBAZ_DISPLAY, width, height);
    GzNewDisplay(&outputNormalMap, GZ_RGBAZ_DISPLAY, width, height);
    GzNewDisplay(&outputDepthMap, GZ_RGBAZ_DISPLAY, width, height);



    GzNewDisplay(&final, GZ_RGBAZ_DISPLAY, width, height);
    memset(final->fbuf,0,sizeof(GzPixel)*width*height);
    
    for (int i = 0; i < AAKERNEL_SIZE; ++i) {
        Application6 application;
        application.runRenderer(GZ_NORMALS,outputTexture,AAFilter[i][0],AAFilter[i][1]);
        application.runRenderer(GZ_NORMALMAP,outputNormalMap,AAFilter[i][0],AAFilter[i][1]);
        application.runRenderer(GZ_DEPTHMAP,outputDepthMap,AAFilter[i][0],AAFilter[i][1]);

        

        
        for (int j = 0; j < width*height; ++j) {
            final->fbuf[j].red += outputTexture->fbuf[j].red * AAFilter[i][2];
            final->fbuf[j].green += outputTexture->fbuf[j].green * AAFilter[i][2];
            final->fbuf[j].blue += outputTexture->fbuf[j].blue * AAFilter[i][2];
     
        }
    }
    // Edge detection convolution
 /*   if(SobelFilter(final)){
        return GZ_FAILURE;
    }
*/
    if(SobelFilter(5000,outputNormalMap))
    {
        return GZ_FAILURE;
    }
    if(SobelFilter(100,outputDepthMap))
    {
        return GZ_FAILURE;
    }
    for (int i = 0; i < width*height; ++i){
        if (outputDepthMap->fbuf[i].red == 0 && outputDepthMap->fbuf[i].blue == 0 && outputDepthMap->fbuf[i].green == 0)
        {
            edges[i] = 1;
        }
        if (outputNormalMap->fbuf[i].red == 0 && outputNormalMap->fbuf[i].blue == 0 && outputNormalMap->fbuf[i].green == 0)
        {
            edges[i] = 1;
        }
        if (edges[i] == 1){
            final->fbuf[i].red = 0;
            final->fbuf[i].blue = 0;
            final->fbuf[i].green = 0;
        }
    }
    FILE *outfile;
    if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
    {
        printf("Could not open output file for writing %s \n", OUTFILE);
        return GZ_FAILURE;
    }
    GzFlushDisplay2File(outfile, final);
    
    if( fclose( outfile ) )
        printf( "The output file was not closed\n" );
    
    GzFreeDisplay(outputTexture);
    GzFreeDisplay(outputNormalMap);
    GzFreeDisplay(outputDepthMap);
    GzFreeDisplay(final);
    return 0;
}


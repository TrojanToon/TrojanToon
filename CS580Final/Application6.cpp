// Application5.cpp: implementation of the Application5 class.
//
//////////////////////////////////////////////////////////////////////

/*
 * application test code for homework assignment #4
*/

#define INFILE  "ppot.asc"
#define OUTFILE "output.ppm"

#include "Application6.h"
#include "gz.h"
#include "disp.h"
#include "rend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */

int Application6::runRenderer(int mode, GzDisplay* output, float shiftX, float shiftY)
{
	GzCamera    camera;
	GzToken     nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer   valueListTriangle[3]; 	/* vertex attribute pointers */
	GzToken     nameListShader[9];      /* shader attribute names */
	GzPointer   valueListShader[9];     /* shader attribute pointers */
	GzToken     nameListLights[10];     /* light info */
	GzPointer   valueListLights[10];
    GzToken     nameListAA[2];
    GzPointer   valueListAA[2];
	GzCoord		vertexList[3];		/* vertex position coordinates */ 
	GzCoord		normalList[3];		/* vertex normals */ 
	GzTextureIndex	uvList[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int		status; 
	int		xRes, yRes, dispClass;	/* display parameters */ 
	int     interpStyle;
    float   specpower;
 
	/* Transforms */
	GzMatrix	translateAndScale = 
	{ 
		{3.25,	0.0,	0.0,	0.0}, 
		{0.0,	3.25,	0.0,	-3.25}, 
		{0.0,	0.0,	3.25,	3.5}, 
		{0.0,	0.0,	0.0,	1.0} 
	}; 
	 
	GzMatrix	rotateX = 
	{ 
		{1.0,	0.0,	0.0,	0.0}, 
		{0.0,	.7071,	.7071,	0.0}, 
		{0.0,	-.7071,	.7071,	0.0}, 
		{0.0,	0.0,	0.0,	1.0}
	}; 
	 
	GzMatrix	rotateY = 
	{ 
		{.866,	0.0,	-0.5,	0.0}, 
		{0.0,	1.0,	0.0,	0.0}, 
		{0.5,	0.0,	.866,	0.0}, 
		{0.0,	0.0,	0.0,	1.0}
	};

	/* Light */
	GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };
	GzLight	light2 = { {0, -0.7071, -0.7071}, {0.9, 0.2, 0.3} };
	GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.2, 0.7, 0.3} };
	GzLight	ambientlight = { {0, 0, 0}, {0.3, 0.3, 0.3} };

	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient = {0.7, 0.7, 0.7};

	/* 
	 * initialize the display and the renderer 
	 */ 

	int width = 256;    // frame buffer and display width
	int height = 256;   // frame buffer and display height

	status = 0;

	status |= GzNewDisplay(&display, GZ_RGBAZ_DISPLAY, width, height);

	status |= GzGetDisplayParams(display, &xRes, &yRes, &dispClass); 
	 
	status |= GzInitDisplay(display); 
 
	status |= GzNewRender(&render, GZ_Z_BUFFER_RENDER, display); 

//#if 1 	/* set up app-defined camera if desired, else use camera defaults */
//   camera.position[X] = -3;
//    camera.position[Y] = -25;
//    camera.position[Z] = -4;
//
//    camera.lookat[X] = 7.8;
//    camera.lookat[Y] = 0.7;
//    camera.lookat[Z] = 6.5;

//    camera.worldup[X] = -0.2;
//    camera.worldup[Y] = 1.0;
//    camera.worldup[Z] = 0.0;

//	camera.FOV = 63.7;              /* degrees */

//	status |= GzPutCamera(render, &camera); 
//#endif 
#if 1 	/* set up app-defined camera if desired, else use camera defaults */
	camera.position[X] = 13.2;      
  	camera.position[Y] = -8.7;
  	camera.position[Z] = -14.8;

  	camera.lookat[X] = 0.8;
  	camera.lookat[Y] = 0.7;
  	camera.lookat[Z] = 4.5;

  	camera.worldup[X] = -0.2;
  	camera.worldup[Y] = 1.0;
  	camera.worldup[Z] = 0.0;

	camera.FOV = 53.7;              /* degrees */

	status |= GzPutCamera(render, &camera); 
#endif 
	status |= GzBeginRender(render);

	/* 
	  renderer is ready for frame --- define lights and shader at start of frame 
	*/

	/*
	 * Tokens associated with light parameters
	 */
	nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
	valueListLights[0] = (GzPointer)&light1;
	nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
	valueListLights[1] = (GzPointer)&light2;
	nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
	valueListLights[2] = (GzPointer)&light3;
	status |= GzPutAttribute(render, 3, nameListLights, valueListLights);

	nameListLights[0] = GZ_AMBIENT_LIGHT;
	valueListLights[0] = (GzPointer)&ambientlight;
	status |= GzPutAttribute(render, 1, nameListLights, valueListLights);

	/*
	 * Tokens associated with shading 
	 */
	nameListShader[0]  = GZ_DIFFUSE_COEFFICIENT;
	valueListShader[0] = (GzPointer)diffuseCoefficient;

	/* 
	* Select either GZ_COLOR or GZ_NORMALS as interpolation mode  
	*/
	nameListShader[1]  = GZ_INTERPOLATE;
//#if 0
//	interpStyle = GZ_COLOR;         /* Gouraud shading */
//#else 
//	interpStyle = GZ_NORMALS;       /* Phong shading */
//#endif
//*/
	/*set interpStyle directly*/
	interpStyle = mode;

	valueListShader[1] = (GzPointer)&interpStyle;
	nameListShader[2]  = GZ_AMBIENT_COEFFICIENT;
	valueListShader[2] = (GzPointer)ambientCoefficient;
	nameListShader[3]  = GZ_SPECULAR_COEFFICIENT;
	valueListShader[3] = (GzPointer)specularCoefficient;
	nameListShader[4]  = GZ_DISTRIBUTION_COEFFICIENT;
	specpower = 32;
	valueListShader[4] = (GzPointer)&specpower;
	nameListShader[5] = GZ_TEXTURE_MAP;
#if 1
	valueListShader[5] = (GzPointer)tex_fun;  /* or use ptex_fun */
#else
	valueListShader[5] = (GzPointer)ptex_fun;
#endif

	status |= GzPutAttribute(render, 6, nameListShader, valueListShader);
    
    nameListAA[0] = GZ_AASHIFTX;
    nameListAA[1] = GZ_AASHIFTY;
    valueListAA[0] = (GzPointer)&shiftX;
    valueListAA[1] = (GzPointer)&shiftY;
    
    status |= GzPutAttribute(render, 2, nameListAA, valueListAA);

	status |= GzPushMatrix(render, translateAndScale);  
	status |= GzPushMatrix(render, rotateY); 
	status |= GzPushMatrix(render, rotateX);

	if (status) return(GZ_FAILURE); 

	/* 
	* Tokens associated with triangle vertex values 
	*/ 
	nameListTriangle[0] = GZ_POSITION;
	nameListTriangle[1] = GZ_NORMAL;
	nameListTriangle[2] = GZ_TEXTURE_INDEX;
	 
	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE , "r" )) == NULL )
	{
		printf("Could not open input from %s \n", INFILE);
		return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
	{
		printf("Could not open output file for writing %s \n", OUTFILE);
		return GZ_FAILURE;
	}

	/* 
	* Walk through the list of triangles, set color 
	* and render each triangle 
	*/ 
	while( fscanf(infile, "%s", dummy) == 1) { 	/* read in tri word */
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[0][0]), &(vertexList[0][1]),  
		&(vertexList[0][2]), 
		&(normalList[0][0]), &(normalList[0][1]), 	
		&(normalList[0][2]), 
		&(uvList[0][0]), &(uvList[0][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[1][0]), &(vertexList[1][1]), 	
		&(vertexList[1][2]), 
		&(normalList[1][0]), &(normalList[1][1]), 	
		&(normalList[1][2]), 
		&(uvList[1][0]), &(uvList[1][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[2][0]), &(vertexList[2][1]), 	
		&(vertexList[2][2]), 
		&(normalList[2][0]), &(normalList[2][1]), 	
		&(normalList[2][2]), 
		&(uvList[2][0]), &(uvList[2][1]) ); 
 
	    /* 
	    * Set the value pointers to the first vertex of the
	    * triangle, then feed it to the renderer 
		* NOTE: this sequence matches the nameList token sequence
	    */ 
	    valueListTriangle[0] = (GzPointer)vertexList;
	    valueListTriangle[1] = (GzPointer)normalList;
	    valueListTriangle[2] = (GzPointer)uvList;
	    GzPutTriangle(render, 3, nameListTriangle, valueListTriangle); 
	} 

	// GzFlushDisplay2File(outfile, display); 	/* write out or update display to file*/
    memcpy(output->fbuf,display->fbuf,sizeof(GzPixel)*xRes*yRes);

	/* 
	 * Clean up and exit
	 */ 

	if( fclose( infile ) )
		printf( "The input file was not closed\n" );
  
	if( fclose( outfile ) )
		printf( "The output file was not closed\n" );

	status |= GzFreeRender(render); 
	status |= GzFreeDisplay(display); 
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

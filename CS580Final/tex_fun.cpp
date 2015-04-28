/* Texture functions for cs580 GzLib	*/
#include	<stdlib.h>
#include	<stdio.h>
#include	<math.h>
#include	"gz.h"

GzColor	*image;
int xs, ys;
int reset = 1;
int reset_hatching = 1;
void VectScale(float s, GzCoord sVect, GzCoord inp);

void VectAdd(GzCoord sum, GzCoord a, GzCoord b);

int idx(int x, int y) {
	return x + y * xs;
}

// Texture Coordinate Mapping
int pos(int x, int y){
	if(x>xs-1){x = xs-1;}
	if(y>ys-1){y = ys-1;}
	return x + y * xs;
}


/* Image texture function */
int tex_hatching(float intensity, float u, float v, GzColor color)
{
	unsigned char	pixel[3];
	unsigned char	dummy;
	char		foo[8];
	int		i;
	FILE		*fd;

	if (reset_hatching) {          /* open and load texture file */
		fd = fopen ("tex_hatching.ppm", "rb");
		if (fd == NULL) {
			fprintf (stderr, "texture file not found\n");
			return GZ_FAILURE;
		}
		fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*xs*ys);
		if (image == NULL) {
			fprintf (stderr, "malloc for texture image failed\n");
			return GZ_FAILURE;
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		reset_hatching = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	/* determine texture cell corner values and perform bilinear interpolation */
	/* set color to interpolated GzColor value and return */
	if(intensity>=0.99){
		color[0] = 255;
		color[1] = 255;
		color[2] = 255;
		return GZ_SUCCESS;
	}
	if(intensity<0){intensity = 0;}
	  int pid = (int)(intensity * 8) ;
	  
	  if(u<0){u = 0;}
	  if(v<0){v = 0;}
	  if(u>1){u = 1;}
	  if(v>1){v = 1;}
	  float uX = u * (xs)/8 -1;
	  float uY = v * (ys)-1;
	  int pX = (int)uX;
	  int pY = (int)uY;
	  float s = uX - pX;
	  float t = uY - pY;
	  pX = pX + pid * (xs)/8;
	  //bilinear [interpolation
	  for(int i=0;i<3;i++){
		color[i] = s * t * image[pos(pX+1,pY+1)][i] + (1-s) * t * image[pos(pX,pY+1)][i] + s * (1-t) * image[pos(pX+1,pY)][i] + (1-s) * (1-t) * image[pos(pX,pY)][i];
	  }
	return GZ_SUCCESS;
}

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	unsigned char	pixel[3];
	unsigned char	dummy;
	char		foo[8];
	int		i;
	FILE		*fd;

	if (reset) {          /* open and load texture file */
		fd = fopen ("texture", "rb");
		if (fd == NULL) {
			fprintf (stderr, "texture file not found\n");
			return GZ_FAILURE;
		}
		fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*xs*ys);
		if (image == NULL) {
			fprintf (stderr, "malloc for texture image failed\n");
			return GZ_FAILURE;
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		reset = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	/* determine texture cell corner values and perform bilinear interpolation */
	/* set color to interpolated GzColor value and return */
	u = fmax(0,fmin(1,u)) * (xs-1);
	v = fmax(0,fmin(1,v)) * (ys-1);
	int u0 = floor(u);
	int v0 = floor(v);
	float weightU = 1 - (u-u0);
	float weightV = 1 - (v-v0);
	GzColor temp;
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	VectScale(weightU * weightV, temp, image[idx(u0,v0)]);
	VectAdd(color,color,temp);
	if (weightV != 1) {
		VectScale(weightU * (1-weightV), temp, image[idx(u0,v0+1)]);
		VectAdd(color,color,temp);
	}
	if (weightU != 1) {
		VectScale((1-weightU) * weightV, temp, image[idx(u0+1,v0)]);
		VectAdd(color,color,temp);
	}
	if (weightV != 1 && weightU != 1) {
		VectScale((1-weightU) * (1-weightV), temp, image[idx(u0+1,v0+1)]);
		VectAdd(color,color,temp);
	}
	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	u = (u * 2 - 1) * 1 - 0.5;
	v = (v * 2 - 1) * 1;
	float Zre = 0, Zim = 0;
	float length;
	for (int i = 0; i < 200; i++) {
		float re = Zre * Zre - Zim * Zim + u;
		float im = Zre * Zim * 2 + v;
		Zre = re;
		Zim = im;
		length = re * re + im * im;
		if (length > 4) {
			break;
		}
	}
	if (length > 4) {
		color[0] = 1;
		color[1] = 0;
		color[2] = 1;
	}
	else {
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
	}
	return GZ_SUCCESS;
}


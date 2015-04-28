/* CS580 Homework 3 */

#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	"gz.h"
#include	"rend.h"
#include	"disp.h"
#define PI 	3.1416
#define Z_MAX (2e9)

using namespace std;
extern int tex_hatching(float intensity, float u, float v, GzColor color);
typedef int (*tex_fun_pointer)(float u, float v, GzColor color);

float CLAMP (float value,float low,float high);
short	ctoi(float color);
int minBound(float x, float y, float z);
int maxBound(float x, float y, float z);
int DrawTriangle(GzRender *render, GzCoord *valueList, GzCoord *normalList, GzTextureIndex *texture);
int DrawPoint(GzRender *render, GzColor color, int x, int y, int z);
void setXsp(GzRender *render, GzDisplay *display);
void InitCamera(GzRender *render);
void VectAdd(GzCoord sum, GzCoord a, GzCoord b);
void VectSub(GzCoord sub, GzCoord a, GzCoord b);
void VectDot(float *dot, GzCoord a, GzCoord b);
void VectX(GzCoord crox, GzCoord a, GzCoord b);
void computeXiw(GzRender *render);
void computeXpi(GzRender *render);
int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	degree = degree * PI / 180;
	memset(mat,0,sizeof(GzMatrix));
	mat[0][0] = 1;
	mat[3][3] = 1;
	mat[1][1] = cos(degree);
	mat[1][2] = -sin(degree);
	mat[2][1] = sin(degree);
	mat[2][2] = cos(degree);

	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	degree = degree * PI / 180;
	memset(mat,0,sizeof(GzMatrix));
	mat[1][1] = 1;
	mat[3][3] = 1;
	mat[0][0] = cos(degree);
	mat[2][0] = -sin(degree);
	mat[0][2] = sin(degree);
	mat[2][2] = cos(degree);
	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	degree = degree * PI / 180;
	memset(mat,0,sizeof(GzMatrix));
	mat[2][2] = 1;
	mat[3][3] = 1;
	mat[0][0] = cos(degree);
	mat[0][1] = -sin(degree);
	mat[1][0] = sin(degree);
	mat[1][1] = cos(degree);
	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	memset(mat,0,sizeof(GzMatrix));
	mat[0][0] = 1;
	mat[1][1] = 1;
	mat[2][2] = 1;
	mat[3][3] = 1;
	mat[0][3] = translate[0];
	mat[1][3] = translate[1];
	mat[2][3] = translate[2];

	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	memset(mat,0,sizeof(GzMatrix));
	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- keep closed until all inits are done 
- setup Xsp and anything only done once 
- span interpolator needs pointer to display 
- check for legal class GZ_Z_BUFFER_RENDER 
- init default camera 
*/ 
	(* render) = new GzRender();
	(* render)->open = 0;
	(* render)->display = display;
	if (renderClass != GZ_Z_BUFFER_RENDER)
		return GZ_FAILURE;
	(* render)->renderClass = renderClass;
	setXsp(*render,display);
	InitCamera(*render);
	return GZ_SUCCESS;

}

void InitCamera(GzRender *render){
	render->camera.position[0] = DEFAULT_IM_X;
	render->camera.position[1] = DEFAULT_IM_Y;
	render->camera.position[2] = DEFAULT_IM_Z;
	render->camera.FOV = DEFAULT_FOV;
	render->camera.lookat[0] = 0.0;
	render->camera.lookat[1] = 0.0;
	render->camera.lookat[2] = 0.0;
	render->camera.worldup[0] = 0.0;
	render->camera.worldup[1] = 1.0;
	render->camera.worldup[2] = 0.0;

}
void setXsp(GzRender *render, GzDisplay *display){
	memset(render->Xsp,0,sizeof(GzMatrix));
	render->Xsp[0][0] = display->xres/2.0;
	render->Xsp[1][1] = -display->yres/2.0;
	render->Xsp[0][3] = display->xres/2.0;
	render->Xsp[1][3] = display->yres/2.0;
	render->Xsp[2][2] = Z_MAX;
	render->Xsp[3][3] = 1.0;
}

int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	delete render;
	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
/*  
- set up for start of each frame - clear frame buffer 
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms if it want to. 
*/ 
	render->Xsp[2][2] = Z_MAX * tan(render->camera.FOV*PI/(2*180));
	computeXiw(render);
	computeXpi(render);
	render->matlevel = 0;
    render->numlights = 0;
	GzPushMatrix(render,render->Xsp);
	GzPushMatrix(render,render->camera.Xpi);
	GzPushMatrix(render,render->camera.Xiw);
    memset(&(render->Xnorm[0]),0,sizeof(GzMatrix));
    render->Xnorm[0][0][0] = 1;
    render->Xnorm[0][1][1] = 1;
    render->Xnorm[0][2][2] = 1;
    render->Xnorm[0][3][3] = 1;
    memset(&(render->Xnorm[1]),0,sizeof(GzMatrix));
    render->Xnorm[1][0][0] = 1;
    render->Xnorm[1][1][1] = 1;
    render->Xnorm[1][2][2] = 1;
    render->Xnorm[1][3][3] = 1;
	return GZ_SUCCESS;
}

void VectAdd(GzCoord sum, GzCoord a, GzCoord b){
	sum[0] = a[0] + b[0];
	sum[1] = a[1] + b[1];
	sum[2] = a[2] + b[2];
}

void VectSub(GzCoord sub, GzCoord a, GzCoord b){
	sub[0] = a[0] - b[0];
	sub[1] = a[1] - b[1];
	sub[2] = a[2] - b[2];
}

void VectMul(GzCoord prod, GzCoord a, GzCoord b){
    prod[0] = a[0] * b[0];
    prod[1] = a[1] * b[1];
    prod[2] = a[2] * b[2];
}

void VectDot(float *dot, GzCoord a, GzCoord b){
	*dot = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void VectX(GzCoord crox, GzCoord a, GzCoord b){
	crox[0] = a[1]*b[2] - a[2]*b[1];
	crox[1] = a[2]*b[0] - a[0]*b[2];
	crox[2] = a[0]*b[1] - a[1]*b[0];
}

void VectNormalize(GzCoord a){
	float norm = a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
	norm = sqrt(norm);
	a[0] = a[0]/norm;
	a[1] = a[1]/norm;
	a[2] = a[2]/norm;
}

void VectScale(float s, GzCoord sVect, GzCoord inp){
	sVect[0] = s * inp[0];
	sVect[1] = s * inp[1];
	sVect[2] = s * inp[2];

}

// void computeColor(GzRender *render, GzCoord normal, GzColor color){
//     VectMul(color,render->Ka,render->ambientlight.color);
//     GzColor specular;
//     memset(specular,0,sizeof(GzColor));
//     GzColor diffuse;
//     memset(diffuse,0,sizeof(GzColor));
//     for (int i = 0; i < render->numlights; ++i) {
//         float NdotL;
//         VectDot(&NdotL,render->lights[i].direction,normal);
//         if (NdotL < 0) {
//             VectScale(-1,normal,normal);
//             NdotL *= -1;
//         }
//         GzCoord E = {0,0,-1};
//         float NdotE;
//         VectDot(&NdotE,E,normal);
//         if (NdotE <= 0) {
//             continue;
//         }
        
//         GzCoord IeNdotL;
//         VectScale(NdotL,IeNdotL,render->lights[i].color);
//         VectAdd(diffuse,diffuse,IeNdotL);
        
//         float RdotE;
//         GzCoord R;
//         GzCoord tmp;
//         VectScale(2*NdotL,tmp,normal);
//         VectSub(R,tmp,render->lights[i].direction);
//         VectDot(&RdotE,R,E);
//         RdotE = fmax(0.0f,RdotE);
//         GzCoord IeRdotEs;
//         VectScale(pow(RdotE,render->spec),IeRdotEs,render->lights[i].color);
//         VectAdd(specular,specular,IeRdotEs);
//     }
//     GzColor diffcolor;
//     VectMul(diffcolor,diffuse,render->Kd);
//     GzColor speccolor;
//     VectMul(speccolor,specular,render->Ks);
//     VectAdd(color,color,diffcolor);
//     VectAdd(color,color,speccolor);
    
//     color[0] = fmin(color[0],1);
//     color[1] = fmin(color[1],1);
//     color[2] = fmin(color[2],1);
// }



GzCoord one = {1,1,1};

void computeColor(GzRender *render, GzCoord normal, GzColor color){
    VectMul(color,one,render->ambientlight.color);
    GzColor specular;
    memset(specular,0,sizeof(GzColor));
    GzColor diffuse;
    memset(diffuse,0,sizeof(GzColor));
    for (int i = 0; i < render->numlights; ++i) {
        float NdotL;
        VectDot(&NdotL,render->lights[i].direction,normal);
        if (NdotL < 0) {
            VectScale(-1,normal,normal);
            NdotL *= -1;
        }
        GzCoord E = {0,0,-1};
        float NdotE;
        VectDot(&NdotE,E,normal);
        if (NdotE <= 0) {
            continue;
        }
        
        GzCoord IeNdotL;
        VectScale(NdotL,IeNdotL,render->lights[i].color);
        VectAdd(diffuse,diffuse,IeNdotL);
        

        float RdotE;
        GzCoord R;
        GzCoord tmp;
        VectScale(2*NdotL,tmp,normal);
        VectSub(R,tmp,render->lights[i].direction);
        VectDot(&RdotE,R,E);
        RdotE = fmax(0.0f,RdotE);
        GzCoord IeRdotEs;
        VectScale(pow(RdotE,render->spec),IeRdotEs,render->lights[i].color);
        VectAdd(specular,specular,IeRdotEs);
    }

    VectAdd(color,color,diffuse);
    VectAdd(color,color,specular);
    
    color[0] = fmin(color[0],1);
    color[1] = fmin(color[1],1);
    color[2] = fmin(color[2],1);
}
void computeHatchingColor(GzRender *render, GzCoord normal, GzColor tex, GzColor color, float x, float y){
    
	

    GzColor specular;
    memset(specular,0,sizeof(GzColor));
    GzColor diffuse;
    memset(diffuse,0,sizeof(GzColor));
	float NdotL;
    for (int i = 2; i < 3; ++i) {
        
        VectDot(&NdotL,render->lights[i].direction,normal);
        if (NdotL < 0) {
            VectScale(-1,normal,normal);
            NdotL *= -1;
        }
        GzCoord E = {0,0,-1};
        float NdotE;
        VectDot(&NdotE,E,normal);
        if (NdotE <= 0) {
            continue;
        }
		
		
		float a[3];
		a[0] =tex[0];
		a[1] = tex[1];
		a[2] = tex[2];
		GzCoord IeNdotL;
        VectScale(NdotL,IeNdotL,render->lights[i].color);
        VectAdd(diffuse,diffuse,IeNdotL);




        float RdotE;
        GzCoord R;
        GzCoord tmp;
        VectScale(2*NdotL,tmp,normal);
        VectSub(R,tmp,render->lights[i].direction);
        VectDot(&RdotE,R,E);
        RdotE = fmax(0.0f,RdotE);
        GzCoord IeRdotEs;
        VectScale(pow(RdotE,render->spec),IeRdotEs,render->lights[i].color);
        VectAdd(specular,specular,IeRdotEs);
    }

    GzColor diffcolor;
	
	float specularAverage = (specular[0] + specular[1] + specular[2])/3;
	if(specularAverage >0.05){
		int i = 0;
	}
	float ambientAverage = (render->ambientlight.color[0] + render->ambientlight.color[1] + render->ambientlight.color[2])/3;
	float intensity = specularAverage + ambientAverage +NdotL/3;// NdotL * Kd
	//float intensity = NdotL;
	if(intensity>1){
		intensity = 0.99;
	}
	tex_hatching(intensity,x,y,tex);  
    VectMul(color,tex,render->ambientlight.color);
	VectMul(diffcolor,diffuse,tex);
	



    GzColor speccolor;
    VectMul(speccolor,specular,render->Ks);
    VectAdd(color,color,diffcolor);
    VectAdd(color,color,speccolor);
    
    color[0] = fmin(color[0],1);
    color[1] = fmin(color[1],1);
    color[2] = fmin(color[2],1);

}
void computeGoochColor(GzRender *render, GzCoord normal, GzColor tex, GzColor color){

	//Gooch Shading
		GzCoord coolColorPara, warmColorPara;
		coolColorPara[0] = 159.0/255;
		coolColorPara[1] = 148.0/255;
		coolColorPara[2] = 255.0/255;
		warmColorPara[0] = 255.0/255;
		warmColorPara[1] = 75.0/255;
		warmColorPara[2] = 75.0/255;
		float alpha = 0.25;
		float beta = 0.5;

	///


    VectMul(color,tex,render->ambientlight.color);
    GzColor specular;
    memset(specular,0,sizeof(GzColor));
    GzColor diffuse;
    memset(diffuse,0,sizeof(GzColor));
    for (int i = 0; i < render->numlights; ++i) {
        float NdotL;
        VectDot(&NdotL,render->lights[i].direction,normal);
        if (NdotL < 0) {
            VectScale(-1,normal,normal);
            NdotL *= -1;
        }
        GzCoord E = {0,0,-1};
        float NdotE;
        VectDot(&NdotE,E,normal);
        if (NdotE <= 0) {
            continue;
        }
        
        GzCoord IeNdotL;
        VectScale(NdotL,IeNdotL,render->lights[i].color);


		//Gooch Shading     Shading + Color Shift
		// coolMul = (1+NdotL)/2; warmMul = 1 - coolMul;
		//Color(i) = coolMul * (alpha * originColor + coolColorPara) + warmMul *  (beta * originColor + warmColorPara);
		
		GzCoord coolColor,warmColor,goochColor;
		VectScale(alpha,coolColor,tex);
		VectScale(beta,warmColor,tex);
		VectAdd(coolColor,coolColor,coolColorPara);
		VectAdd(warmColor,warmColor,warmColorPara);
		float coolMul = (1 + NdotL)/2;
		float warmMul = 1 - coolMul;
		VectScale(coolMul,coolColor,coolColor);
		VectScale(warmMul,warmColor,warmColor);
		VectAdd(goochColor,coolColor,warmColor);
		VectMul(goochColor,goochColor,render->lights[i].color);
		//
		VectAdd(diffuse,diffuse,goochColor);
       // VectAdd(diffuse,diffuse,IeNdotL);
        


        float RdotE;
        GzCoord R;
        GzCoord tmp;
        VectScale(2*NdotL,tmp,normal);
        VectSub(R,tmp,render->lights[i].direction);
        VectDot(&RdotE,R,E);
        RdotE = fmax(0.0f,RdotE);
        GzCoord IeRdotEs;
        VectScale(pow(RdotE,render->spec),IeRdotEs,render->lights[i].color);
        VectAdd(specular,specular,IeRdotEs);
    }

    GzColor diffcolor;
    VectMul(diffcolor,diffuse,tex);


    GzColor speccolor;
    VectMul(speccolor,specular,render->Ks);
    VectAdd(color,color,diffcolor);
    VectAdd(color,color,speccolor);
    
    color[0] = fmin(color[0],1);
    color[1] = fmin(color[1],1);
    color[2] = fmin(color[2],1);

}
void computePhongColor(GzRender *render, GzCoord normal, GzColor tex, GzColor color){
    VectMul(color,tex,render->ambientlight.color);
    GzColor specular;
    memset(specular,0,sizeof(GzColor));
    GzColor diffuse;
    memset(diffuse,0,sizeof(GzColor));
    for (int i = 0; i < render->numlights; ++i) {
        float NdotL;
        VectDot(&NdotL,render->lights[i].direction,normal);
        if (NdotL < 0) {
            VectScale(-1,normal,normal);
            NdotL *= -1;
        }
        GzCoord E = {0,0,-1};
        float NdotE;
        VectDot(&NdotE,E,normal);
        if (NdotE <= 0) {
            continue;
        }
        
        GzCoord IeNdotL;
        VectScale(NdotL,IeNdotL,render->lights[i].color);
        VectAdd(diffuse,diffuse,IeNdotL);
        


        float RdotE;
        GzCoord R;
        GzCoord tmp;
        VectScale(2*NdotL,tmp,normal);
        VectSub(R,tmp,render->lights[i].direction);
        VectDot(&RdotE,R,E);
        RdotE = fmax(0.0f,RdotE);
        GzCoord IeRdotEs;
        VectScale(pow(RdotE,render->spec),IeRdotEs,render->lights[i].color);
        VectAdd(specular,specular,IeRdotEs);
    }

    GzColor diffcolor;
    VectMul(diffcolor,diffuse,tex);




    GzColor speccolor;
    VectMul(speccolor,specular,render->Ks);
    VectAdd(color,color,diffcolor);
    VectAdd(color,color,speccolor);
    
    color[0] = fmin(color[0],1);
    color[1] = fmin(color[1],1);
    color[2] = fmin(color[2],1);
}

void computeHachingColor(GzRender *render, GzCoord normal, GzColor tex, GzColor color, int x, int y){
	float ambient = 0.025f;
    GzCoord lightVector;
    memset(lightVector,0,sizeof(GzCoord));
    GzCoord E = {0,0,-1};
    for (int i = 0; i < render->numlights; ++i) {
    	VectAdd(lightVector,lightVector,render->lights[i].direction);
    }
    VectNormalize(lightVector);
    float NdotL;
    float diffuse;
    VectDot(&NdotL,lightVector,normal);
    diffuse = CLAMP(NdotL,0,1);
        
    
    // GzColor reflectedVector;
    // memset(reflectedVector,0,sizeof(GzCoord));    
    GzCoord R;
    GzCoord tmp;
    VectScale(2*NdotL,tmp,normal);
    VectSub(R,tmp,lightVector);
    float specular = 0;
    float RdotE;
    VectDot(&RdotE,R,E);
	specular = pow(RdotE, render->spec);
    float lightIntensity = CLAMP(diffuse + specular + ambient, 0, 1);
    color[0] = 1;
    color[1] = 1;
    color[2] = 1;
    if (lightIntensity < 0.85) 
	{
	   // hatch from left top corner to right bottom
	   if ( ( (x + y) % 10 ) == 0) 
	   {
	   		color[0] = 0;
    		color[1] = 0;
    		color[2] = 0;
	   }
	}

	if (lightIntensity < 0.75) 
	{
	   // hatch from right top corner to left boottom
	   if ( ( (x - y) % 10 ) == 0) 
	   {
	      color[0] = 0;
    		color[1] = 0;
    		color[2] = 0;
	   }
	}

	if (lightIntensity < 1) 
	{
	   // hatch from left top to right bottom
	   if ( ( (x + y - 5) % 10 ) == 0) 
	   {
	      color[0] = 0;
    		color[1] = 0;
    		color[2] = 0;
	   }
	}

	if (lightIntensity < 0.75) 
	{
	   // hatch from right top corner to left bottom
	   if ( ( (x - y - 5) % 10 ) == 0) 
	   {
	      color[0] = 0;
    		color[1] = 0;
    		color[2] = 0;
	   }
	}
}

void computeXiw(GzRender *render){
	GzCoord z;
	VectSub(z,render->camera.lookat,render->camera.position);
	VectNormalize(z);
	GzCoord up;//y axis
	float upZ;
	VectDot(&upZ,render->camera.worldup,z);
	GzCoord temp;
	VectScale(upZ,temp,z);
	VectSub(up,render->camera.worldup,temp);
	VectNormalize(up);
	GzCoord x;
	VectX(x,up,z);
	render->camera.Xiw[0][0] = x[0];
	render->camera.Xiw[0][1] = x[1];
	render->camera.Xiw[0][2] = x[2];
	float tempf;
	VectDot(&tempf,x,render->camera.position); 
	render->camera.Xiw[0][3] = -tempf;
	render->camera.Xiw[1][0] = up[0];
	render->camera.Xiw[1][1] = up[1];
	render->camera.Xiw[1][2] = up[2];
	VectDot(&tempf,up,render->camera.position); 
	render->camera.Xiw[1][3] = -tempf;
	render->camera.Xiw[2][0] = z[0];
	render->camera.Xiw[2][1] = z[1];
	render->camera.Xiw[2][2] = z[2];
	VectDot(&tempf,z,render->camera.position); 
	render->camera.Xiw[2][3] = -tempf;
 	render->camera.Xiw[3][3] = 1;
	render->camera.Xiw[3][0] = 0;
	render->camera.Xiw[3][1] = 0;
	render->camera.Xiw[3][2] = 0;
}

void computeXpi(GzRender *render){
	memset(render->camera.Xpi,0,sizeof(GzMatrix));
	render->camera.Xpi[0][0] = 1;
	render->camera.Xpi[1][1] = 1;
	render->camera.Xpi[2][2] = 1;
	render->camera.Xpi[3][3] = 1;
	render->camera.Xpi[3][2] = tan(render->camera.FOV*PI/(180*2.0));

}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	render->camera.position[0] = camera->position[0];
	render->camera.position[1] = camera->position[1];
	render->camera.position[2] = camera->position[2];
	render->camera.FOV = camera->FOV;
	render->camera.lookat[0] = camera->lookat[0];
	render->camera.lookat[1] = camera->lookat[1];
	render->camera.lookat[2] = camera->lookat[2];
	render->camera.worldup[0] = camera->worldup[0];
	render->camera.worldup[1] = camera->worldup[1];
	render->camera.worldup[2] = camera->worldup[2];
	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if(render->matlevel >= MATLEVELS) return GZ_FAILURE;
	memcpy(render->Ximage[render->matlevel],matrix,sizeof(GzMatrix));
    
    GzMatrix tmp;
    memcpy(tmp,matrix,sizeof(GzMatrix));
    tmp[0][3] = 0;
    tmp[1][3] = 0;
    tmp[2][3] = 0;
    float norm = 1.0 / sqrt(tmp[0][0] * tmp[0][0] +
                            tmp[0][1] * tmp[0][1] +
                            tmp[0][2] * tmp[0][2]);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            tmp[i][j] *= norm;
        }
    }
    memcpy(render->Xnorm[render->matlevel],tmp,sizeof(GzMatrix));
    
	render->matlevel++;
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if(render->matlevel <= 0) return GZ_FAILURE;
	render->matlevel--;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender *render, int numAttributes, GzToken *nameList, 
	GzPointer *valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	for (int i = 0; i < numAttributes; ++i){
		if(nameList[i] == GZ_RGB_COLOR)
			memcpy(render->flatcolor,valueList[i],3 * sizeof(float));
        else if (nameList[i] == GZ_AMBIENT_COEFFICIENT)
            memcpy(render->Ka,valueList[i],sizeof(GzColor));
        else if (nameList[i] == GZ_DIFFUSE_COEFFICIENT)
            memcpy(render->Kd,valueList[i],sizeof(GzColor));
        else if (nameList[i] == GZ_SPECULAR_COEFFICIENT)
            memcpy(render->Ks,valueList[i],sizeof(GzColor));
        else if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT)
            memcpy(&(render->spec),valueList[i],sizeof(float));
        else if (nameList[i] == GZ_INTERPOLATE)
            memcpy(&(render->interp_mode),valueList[i],sizeof(int));
        else if (nameList[i] == GZ_AMBIENT_LIGHT)
            memcpy(&(render->ambientlight),valueList[i],sizeof(GzLight));
        else if (nameList[i] == GZ_DIRECTIONAL_LIGHT) {
            if (render->numlights == MAX_LIGHTS)
                return GZ_FAILURE;
            memcpy(&(render->lights[render->numlights]),valueList[i],sizeof(GzLight));
            render->numlights += 1;
        }
        else if (nameList[i] == GZ_TEXTURE_MAP)
        	render->tex_fun = valueList[i];
        else if (nameList[i] == GZ_AASHIFTX)
            memcpy(&(render->shiftX),valueList[i],sizeof(float));
        else if (nameList[i] == GZ_AASHIFTY)
            memcpy(&(render->shiftY),valueList[i],sizeof(float));
	}
	
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender *render, int numParts, GzToken *nameList, 
				  GzPointer *valueList)
/* numParts : how many names and values */
{
    GzCoord *vectexList, *normalList;
    GzTextureIndex *textureList;
    for (int i = 0; i < numParts; ++i){
		if(nameList[i] == GZ_NULL_TOKEN) continue;
		if(nameList[i] == GZ_POSITION)
            vectexList = (GzCoord *)valueList[i];
        if(nameList[i] == GZ_NORMAL)
            normalList = (GzCoord *)valueList[i];
        if (nameList[i] == GZ_TEXTURE_INDEX)
        	textureList = (GzTextureIndex *)valueList[i];
    }
    DrawTriangle(render, vectexList, normalList, textureList);
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts  
- Clip - just discard any triangle with verts behind view plane 
       - test for triangles with all three verts off-screen 
- invoke triangle rasterizer  
*/ 
	return GZ_SUCCESS;
}

void matVectMul(GzMatrix A, GzCoord *valueList){
	for(int i = 0; i < 3; ++i){
		float d = A[3][0]*valueList[i][0] + A[3][1]*valueList[i][1] + A[3][2]*valueList[i][2] +A[3][3];
		float x = (A[0][0]*valueList[i][0] + A[0][1]*valueList[i][1] + A[0][2]*valueList[i][2] +A[0][3])/d;
		float y = (A[1][0]*valueList[i][0] + A[1][1]*valueList[i][1] + A[1][2]*valueList[i][2] +A[1][3])/d;
		float z = (A[2][0]*valueList[i][0] + A[2][1]*valueList[i][1] + A[2][2]*valueList[i][2] +A[2][3])/d;
		valueList[i][0] = x;
		valueList[i][1] = y;
		valueList[i][2] = z;
	}
}

void interpolate(GzCoord vec0, GzCoord vec1, GzCoord vec2, float w0, float w1, float w2, GzCoord ret) {
    
    ret[0] = (vec0[0] * w0 + vec1[0] * w1 + vec2[0] * w2) / (w0+w1+w2);
    ret[1] = (vec0[1] * w0 + vec1[1] * w1 + vec2[1] * w2) / (w0+w1+w2);
    ret[2] = (vec0[2] * w0 + vec1[2] * w1 + vec2[2] * w2) / (w0+w1+w2);
}

void interpolate2d(GzTextureIndex vec0, GzTextureIndex vec1, GzTextureIndex vec2, float w0, float w1, float w2, GzTextureIndex ret) {
    
    ret[0] = (vec0[0] * w0 + vec1[0] * w1 + vec2[0] * w2) / (w0+w1+w2);
    ret[1] = (vec0[1] * w0 + vec1[1] * w1 + vec2[1] * w2) / (w0+w1+w2);
}

/* NOT part of API - just for general assistance */
int DrawTriangle(GzRender *render, GzCoord *valueList, GzCoord *normalList, GzTextureIndex *textureList){
	tex_fun_pointer tex_fun = (tex_fun_pointer)render->tex_fun;

	for(int i = render->matlevel-1; i >= 0; --i){
		matVectMul(render->Ximage[i],valueList);
        matVectMul(render->Xnorm[i],normalList);
	}
    
    for (int i = 0; i < 3; ++i) {
        valueList[i][0] += render->shiftX;
        valueList[i][1] += render->shiftY;
    }

	float x0 = valueList[0][0],y0 = valueList[0][1],z0 = valueList[0][2],
		x1 = valueList[1][0],y1 = valueList[1][1],z1 = valueList[1][2],
		x2 = valueList[2][0],y2 = valueList[2][1],z2 = valueList[2][2];

	float A2 = y1-y0, B2 = x0-x1, C2 = -B2*y0-A2*x0,
		A0 = y2-y1, B0 = x1-x2, C0 = -B0*y1-A0*x1,
		A1 = y0-y2, B1 = x2-x0, C1 = -B1*y2-A1*x2;

	float pA = y0*z1 -y0*z2-y1*z0+y1*z2+y2*z0-y2*z1, 
		  pB = -x0*z1+x0*z2+x1*z0-x1*z2-x2*z0+x2*z1, 
		  pC = x0*y1 -x0*y2-x1*y0+x1*y2+x2*y0-x2*y1, 
		  pD = 0-pA*x0-pB*y0-pC*z0;
    
    GzColor c0, c1, c2;
    float Vz[3], Uv[3][2];
    for (int i = 0; i<3; ++i){
    	Vz[i] = valueList[i][2]/(Z_MAX-valueList[i][2]);
    	Uv[i][0] = textureList[i][0]/(Vz[i] + 1);
    	Uv[i][1] = textureList[i][1]/(Vz[i] + 1);
    }
    if (render->interp_mode == GZ_COLOR) {
        computeColor(render,normalList[0],c0);
        computeColor(render,normalList[1],c1);
        computeColor(render,normalList[2],c2);
    }
	for (int x = minBound(x0,x1,x2); x <= maxBound(x0,x1,x2); ++x){
		int y = minBound(y0,y1,y2);
		float v0 = A0*x + B0*y + C0,
		 v1 = A1*x + B1*y + C1,
		 v2 = A2*x + B2*y + C2;
		for (; y <= maxBound(y0,y1,y2); ++y){
			if((v0>0 && v1>0 && v2>0)||(v0<0 && v1<0 && v2<0)){

                GzTextureIndex uv;
                interpolate2d(Uv[0],Uv[1],Uv[2],v0,v1,v2,uv);

                float z = (-pD-pA*x-pB*y)/pC;
                float vz = z/(Z_MAX-z);
                uv[0] = uv[0] * (vz+1);
                uv[1] = uv[1] * (vz+1);

                GzColor texColor;
                tex_fun(uv[0],uv[1],texColor);

                if (render->interp_mode == GZ_COLOR) {
                    GzColor color;
                    interpolate(c0,c1,c2,v0,v1,v2,color);
                    VectMul(color, color, texColor);
                    DrawPoint(render,color,x,y,round(z));
                }
                else if (render->interp_mode == GZ_NORMALS) {
                    GzCoord normal;
                    GzColor color;
                    interpolate(normalList[0],normalList[1],normalList[2],v0,v1,v2,normal);
                    VectNormalize(normal);
                    //computeGoochColor(render,normal,texColor,color);
					//computePhongColor(render,normal,texColor,color);
                    //computeHachingColor(render,normal,texColor,color,x,y);
					computeHatchingColor(render,normal,texColor,color,uv[0],uv[1]);
                    DrawPoint(render,color,x,y,round((-pD-pA*x-pB*y)/pC));
                }
                /*----------------- ADD FOR PROJECT ----------------------*/
                else if (render->interp_mode == GZ_NORMALMAP) {
                	GzCoord normal;
                	interpolate(normalList[0],normalList[1],normalList[2],v0,v1,v2,normal);
                    VectNormalize(normal);
                    for (int i = 0; i < 3; ++i){
						normal[i] = (normal[i] + 1.0) / 2.0;
					}
                    DrawPoint(render,normal,x,y,round((-pD-pA*x-pB*y)/pC));
                }
                else if (render->interp_mode == GZ_DEPTHMAP) {
                	GzColor color;
                	float depth = round((-pD-pA*x-pB*y)/pC);
					color[0] = depth/Z_MAX;
					color[1] = depth/Z_MAX;
					color[2] = depth/Z_MAX;
					DrawPoint(render,color,x,y,depth);
                }               
                else
                    DrawPoint(render,render->flatcolor,x,y,round((-pD-pA*x-pB*y)/pC));
			}
			v0 += B0;
			v1 += B1;
			v2 += B2;
		}
	}

	return GZ_SUCCESS;
}

int maxBound(float x, float y, float z){
	x = fmax(x,y);
	return ceil(fmax(x,z));
}

int minBound(float x, float y, float z){
	x = fmin(x,y);
	return floor(fmin(x,z));
}



int DrawPoint(GzRender *render, GzColor color, int x, int y, int z){
	GzPutDisplay(render->display, x, y, ctoi(color[0]),
		ctoi(color[1]),ctoi(color[2]), 1, z);
	return GZ_SUCCESS;
}

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

float CLAMP (float value,float low,float high) {
	return (value < low ? low : (value > high ? high : value));	
} 


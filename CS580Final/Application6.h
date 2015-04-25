// Application5.h: interface for the Application4 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct GzDisplay;
struct GzRender;

class Application6
{
public:	
	int runRenderer(int mode, GzDisplay* output, float shiftX = 0, float shiftY = 0);
protected:
	GzDisplay* display; // the display
	GzRender* render;   // the renderer
};

#include "gz.h"
#include "rend.h"
#include "disp.h"
#include "math.h"
#include <string.h>

#define WIDTH 256
#define HEIGHT 256
#define	ARRAY2(x,y)	((x)+((y)*WIDTH))	/* simplify fbuf indexing */


int SobelFilter(int Th, GzDisplay *sobel);
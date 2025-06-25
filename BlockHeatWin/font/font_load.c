#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <math.h>
#include "aux.h"

static GLint xsize  = 500;
static GLint ysize  = 500;
static GLint display_num;

void load_bitmapdata(GLubyte bitmaps[][12])
{
	int i,j;
	FILE *fp;
	unsigned int data[95][12];
	if((fp = fopen("bitmap_font.dat", "r")) == 0){
		printf("Can't open file");
		exit(1);
	}
	for(i=0; i<95; i++){
		for(j=0; j<12; j++){
			fscanf(fp, "%x",&data[i][j]);
			bitmaps[i][j] = (GLubyte)data[i][j];
			printf("%x ",bitmaps[i][j]);
		}
		printf("\n");
	}
	fclose(fp);
}


void MakeBitMapFont(void)
{
	int i;
	GLubyte bitmaps[95][12];

	load_bitmapdata(bitmaps);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	display_num = glGenLists (128);
	for(i=32; i<127; i++){
		glNewList(display_num+i, GL_COMPILE);
		   glBitmap(8, 12, 0.0, 1.0, 10.0, 0.0, bitmaps[i-32]);
		glEndList();
	}
}

void DrawString(char *str)
{
	int len;

	len = strlen(str);
	glListBase(display_num);
	glCallLists(len, GL_BYTE, str);
}

void DrawChar(char c)
{
	glListBase(display_num);
	glCallLists(1,GL_BYTE,&c);
}

void ModelDraw(void)
{
	char a[] = {0,1,2,3,4,5,0};

	glClear (GL_COLOR_BUFFER_BIT);

	glRasterPos2i(0, ysize/2 - (12/2));
	DrawString("MonoMonoMonoguchiMonoMonoMonoguchi!!!!!");
	glFlush();
	auxSwapBuffers();
}

void ReshapeWindow(int x, int y)
{
	glViewport(0, 0, x, y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, x-1, 0, y-1, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	xsize = x;
	ysize = y;
}

int main(int argc, char** argv)
{
	auxInitDisplayMode (AUX_DOUBLE | AUX_RGBA);
	auxInitPosition(0,0,500,500);
	auxInitWindow(argv[0]);
	glClearColor(0.0,0.0,0.0,1.0);

	glColor4f(1.0,1.0,1.0,1.0);

	MakeBitMapFont();

	auxReshapeFunc(ReshapeWindow);
	auxMainLoop(ModelDraw);
}

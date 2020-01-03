#include <stdio.h>
#include <stdlib.h>
#include <gl\glut.h>
#include <math.h>

#include "frame_buffer.h"
#include "primitives.h"
#include "color.h"
#include "scene.h"
#include <vector>
#include "utilMethods.h"

#include <iostream>
#include <fstream>

#define ON 1
#define OFF 0

using namespace std;


// Global variables
int window_width, window_height;    // Window dimensions

const int INITIAL_RES = 400;

FrameBuffer* fb;
Scene* scene = new Scene;

class point
{
public:
	double x,y,z,w;

	point(){ x = 0; y = 0; z = 0; w = 1;}
	point(double xa, double ya, double za)
	{
		x = xa; y = ya; z = za; w = 1.0;
	}
	point(double xa, double ya, double za, double wa)
	{
		x = xa; y = ya; z = za; w = wa;
	}
};

void drawRect(double x, double y, double w, double h)
{
	glVertex2f(x,y);
	glVertex2f(x+w,y);
	glVertex2f(x+w,y+h);
	glVertex2f(x, y+h);
}


// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void	display(void)
{
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	double w = 10/double(fb->GetWidth());
	double h = 10/double(fb->GetHeight());

	Color cl;
	glColor3f(0,0,1);

	glBegin(GL_QUADS);

	printf("width %d, height %d\n", fb->GetWidth(), fb->GetHeight());

	for(int y = 0; y < fb->GetHeight(); y++)
	{
		for(int x = 0; x < fb->GetHeight(); x++)
		{
			cl = fb->buffer[x][y].color;
			glColor3f(cl.r, cl.g, cl.b);

			drawRect(w*x, h*y, w, h);
		}
	}

	glEnd();
    glutSwapBuffers();
}


// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void	resize(int x,int y)
{
    glViewport(0,0,x,y);
    window_width = x;
    window_height = y;
    
    printf("Resized to %d %d\n",x,y);
}


// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseButton(int button,int state,int x,int y)
{
   ;
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y)
{
	;
}


// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case 'q':                           /* Quit */
		exit(1);
		break;
	case '-':
		//Decrease the buffer (Increase # virtual pixels)
		fb->Resize(fb->GetHeight()/2, fb->GetWidth()/2);
		break;
	case '=':
		//Increase the buffer (Increase # virtual pixels)
		fb->Resize(fb->GetHeight()*2, fb->GetWidth()*2);
		break;
	case '[':
		//Move image plane closer
		scene->imagePlaneZ += 1;
		break;
	case ']':
		//Move image plane farther
		scene->imagePlaneZ -= 1;
		break;
	case '.':
		//Increase image plane size
		scene->imagePlaneWidth += 1;
		scene->imagePlaneHeight += 1;
		break;
	case ',':
		//Decrease image plane size
		scene->imagePlaneWidth -= 1;
		scene->imagePlaneHeight -= 1;
		break;
	case 'r':
		//redraw
		scene->RayTrace(fb);
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}


int main(int argc, char* argv[])
{    
	scene->Load("transparent_sphere_and_teapot.rtl");
	scene->imagePlaneZ = -8.0;
	scene->imagePlaneHeight = 10.0;
	scene->imagePlaneWidth = 10.0;
	scene->maxDepth = 5;
	scene->attenuationFactor = 0.5;
	scene->Ia = Color(0.3, 0.3, 0.3);
	fb = new FrameBuffer(64, 64);
	scene->RayTrace(fb);


    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Raytracer");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,10,0,10,-10000,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

    // Switch to main loop
    glutMainLoop();
    return 0;        
}

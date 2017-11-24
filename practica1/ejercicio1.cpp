

#include <stdlib.h>
#include <stdio.h>
#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif

#define NUMVTX (8)

static int winwidth,winheight;
static int mx,my;
static int flag=0;
static float rotx=0.0f, roty=0.0f;

GLfloat c[3][3]={{1,0,0},
		 {0,1,0},
		 {0,0,1}};

GLfloat j[10][3]={{-0.5,-0.5,0.5},{-0.5,-0.5,-0.5},
		  {0,-0.5,0.5},{0,-0.5,-0.5},
		  {0,0.499,0.5},{0,0.499,-0.5},
		  {-0.5,0.5,0.5},{-0.5,0.5,-0.5},
		  {0.5,0.5,0.5},{0.5,0.5,-0.5}};

GLfloat h[12][3]={{-0.5,0.5,0.5},{-0.5,0.5,-0.5},
		  {-0.5,-0.5,0.5},{-0.5,-0.5,-0.5},
		  {-0.499,0,0.5},{-0.499,0,-0.5},
		  {-0.01,0,0.5},{-0.01,0,-0.5},
		  {0,0.5,0.5},{0,0.5,-0.5},
		  {0,-0.5,0.5},{0,-0.5,-0.5}};

GLfloat a[12][3]={{0.0,-0.5,0.5},{0.0,-0.5,-0.5},
		  {0.0,0.5,0.5},{0.0,0.5,-0.5},
		  {0.5,0.5,0.5},{0.5,0.5,-0.5},
		  {0.5,-0.5,0.5},{0.5,-0.5,-0.5},
		  {0.499,0,0.5},{0.499,0,-0.5},
		  {0.0,0,0.5},{0.0,0,-0.5}};
void renderJ(){
  glBegin(GL_QUAD_STRIP);
  glColor3fv(c[0]);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3fv(j[0]);
  glVertex3fv(j[1]);
  glVertex3fv(j[2]);
  glVertex3fv(j[3]);

  glColor3fv(c[1]);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3fv(j[4]);
  glVertex3fv(j[5]);
  
  glColor3fv(c[2]);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3fv(j[6]);
  glVertex3fv(j[7]);
  
  glColor3fv(c[0]);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3fv(j[8]);
  glVertex3fv(j[9]);
  glEnd();
  //cerrando las caras visibles
  glBegin(GL_QUAD_STRIP);
  glColor3fv(c[0]);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(j[0][0],j[0][1]-0.0001,j[0][2]);
  glVertex3f(j[1][0],j[1][1]-0.0001,j[1][2]);
  glVertex3f(j[2][0]+0.001,j[2][1]-0.0001,j[2][2]);
  glVertex3f(j[3][0]+0.001,j[3][1]-0.0001,j[3][2]);
  
  glColor3fv(c[1]);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(j[4][0]+0.001,j[4][1]-0.001,j[4][2]);
  glVertex3f(j[5][0]+0.001,j[5][1]-0.001,j[5][2]);
  
  glColor3fv(c[0]);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(j[8][0],j[8][1]-0.0001,j[8][2]);
  glVertex3f(j[9][0],j[9][1]-0.0001,j[9][2]);
  glEnd();
}

void drawgraphix()
{
	int i;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	
	glPushMatrix();

	glTranslatef(0.0f,0.0f,-3.0f);

	GLfloat mat_ambient[] = {0.1,0.1,0.1,1.0};
	GLfloat mat_diffuse[] = {1.0,1.0,1.0,1.0};
	//GLfloat mat_shininess[] = { 40.0 };
	GLfloat light_direction[] = { 0.0, 0.0, 1.0, 0.0};
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
       

	glRotatef(rotx,0.0f,1.0f,0.0f);
	glRotatef(roty,1.0f,0.0f,0.0f);
	/*
	glBegin(GL_QUAD_STRIP);
	for (i=0; i<12; i++) {
	  glColor3fv(c[i%6]);
	  glVertex3fv(a[i]);
	  }*/

	//render 1th J
	glTranslatef(-1.5,0.0,0.0);
	renderJ();
	
	//render H
	glTranslatef(1.2,0.0,0.0);
	glBegin(GL_QUAD_STRIP);
	glColor3fv(c[0]);
	glNormal3f(-1.0f,0.0f,0.0f);
	glVertex3fv(h[0]);
	glVertex3fv(h[1]);
	glVertex3fv(h[2]);
	glVertex3fv(h[3]);

	glColor3fv(c[0]);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3fv(h[4]);
	glVertex3fv(h[5]);

	glColor3fv(c[1]);
	glNormal3f(0.0f,-1.0f,0.0f);
	glVertex3fv(h[6]);
	glVertex3fv(h[7]);

	glColor3fv(c[1]);
	glNormal3f(-1.0f,0.0f,0.0f);
	glVertex3fv(h[8]);
	glVertex3fv(h[9]);

	glColor3fv(c[1]);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3fv(h[10]);
	glVertex3fv(h[11]);
	glEnd();
	//cerrando las caras de H
	
	glBegin(GL_QUAD_STRIP);
	glColor3fv(c[0]);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(h[0][0]+0.0001,h[0][1],h[0][2]);
	glVertex3f(h[1][0]+0.0001,h[1][1],h[1][2]);
	glVertex3f(h[4][0]+0.0001,h[4][1]+0.0001,h[4][2]);
	glVertex3f(h[5][0]+0.0001,h[5][1]+0.0001,h[5][2]);
	
	glColor3fv(c[1]);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(h[6][0]-0.0001,h[6][1]+0.0001,h[6][2]);
	glVertex3f(h[7][0]-0.0001,h[7][1]+0.0001,h[7][2]);

	glColor3fv(c[1]);
	glNormal3f(-1.0f,0.0f,0.0f);
	glVertex3f(h[10][0]-0.0001,h[10][1],h[10][2]);
	glVertex3f(h[11][0]-0.0001,h[11][1],h[11][2]);
	glEnd();
	
	glBegin(GL_QUAD_STRIP);
	glColor3fv(c[0]);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(h[2][0]+0.0001,h[2][1],h[2][2]);
	glVertex3f(h[3][0]+0.0001,h[3][1],h[3][2]);
	glVertex3f(h[4][0]+0.0001,h[4][1],h[4][2]);
	glVertex3f(h[5][0]+0.0001,h[5][1],h[5][2]);
	glEnd();

	//render 2th J
	glTranslatef(0.8,0.0,0.0);
        renderJ();
	
	//render A
	glTranslatef(0.8,0.0,0.0);
	glBegin(GL_QUAD_STRIP);
	glColor3fv(c[0]);
	glNormal3f(-1.0f,0.0f,0.0f);
	glVertex3fv(a[0]);
	glVertex3fv(a[1]);
	glVertex3fv(a[2]);
	glVertex3fv(a[3]);

	glColor3fv(c[1]);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3fv(a[4]);
	glVertex3fv(a[5]);

	glColor3fv(c[1]);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3fv(a[6]);
	glVertex3fv(a[7]);

	glColor3fv(c[1]);
	glNormal3f(-1.0f,1.0f,0.0f);
	glVertex3fv(a[8]);
	glVertex3fv(a[9]);

	glColor3fv(c[2]);
	glNormal3f(0.0f,-1.0f,0.0f);
	glVertex3fv(a[10]);
	glVertex3fv(a[11]);
	glEnd();
	
	//cerrando las caras de A
	glBegin(GL_QUAD_STRIP);
	glColor3fv(c[0]);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(a[0][0]+0.001,a[0][1],a[0][2]);
	glVertex3f(a[1][0]+0.001,a[1][1],a[1][2]);
	glVertex3f(a[2][0]+0.001,a[2][1]-0.0001,a[2][2]);
	glVertex3f(a[3][0]+0.001,a[3][1]-0.0001,a[3][2]);
	
	glColor3fv(c[1]);
	glNormal3f(0.0f,-1.0f,0.0f);
	glVertex3f(a[4][0]-0.0001,a[4][1]-0.0001,a[4][2]);
	glVertex3f(a[5][0]-0.0001,a[5][1]-0.0001,a[5][2]);

	glColor3fv(c[1]);
	glNormal3f(-1.0f,0.0f,0.0f);
	glVertex3f(a[8][0]-0.0001,a[8][1]+0.0001,a[8][2]);
	glVertex3f(a[9][0]-0.0001,a[9][1]+0.0001,a[9][2]);

	glColor3fv(c[2]);
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(a[10][0]-0.0001,a[10][1]+0.0001,a[10][2]);
	glVertex3f(a[11][0]-0.0001,a[11][1]+0.0001,a[11][2]);
	
	glEnd();
	
	glPopMatrix();
	
	glutSwapBuffers();
}

void reshapefunc(int width,int height)
{
	winwidth=width;
	winheight=height;

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0,(float)width/height,1.0,100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f,0.0f,3.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
}

void mousefunc(int button,int state,int x,int y)
{
	if (button==GLUT_LEFT_BUTTON) {
		if (state==GLUT_DOWN) {
			flag=1;
		} else {
			flag=0;
		}
	}
}

void motionfunc(int x,int y)
{
	if (flag>0) {
		if (flag>1) {
			rotx+=360.0f*(x-mx)/winwidth;
			roty+=360.0f*(y-my)/winheight;
		}

		mx=x;
		my=y;

		drawgraphix();

		flag=2;
	}
}

void keyboardfunc(unsigned char key,int x,int y)
{
	if (key=='q' || key==27) exit(0);
}


void idlefunc()
{
	glutPostRedisplay();
}


void init(void) 
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
}
int main(int argc,char **argv)
{
   winwidth=winheight=512;

   glutInit(&argc,argv);
   glutInitWindowSize(winwidth,winheight);
   glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
   glutCreateWindow("Practica 1 - Ejercicio 1.1");

   init();
   
   glutDisplayFunc(drawgraphix);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(mousefunc);
   glutMotionFunc(motionfunc);
   glutKeyboardFunc(keyboardfunc);
   glutIdleFunc(idlefunc);

   glutMainLoop();

   return(0);
}


#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "draw.h"
#include "matrix.h"
#include "list.h"
#include "polygon.h"

#define PI 3.141592653589793
#define REFRESHMS 1
#define STARCOUNT 50
#define STARSIZE 8

struct star
{
	float r, g, b;
	float x, y;
	float scale;
	float rotspeed;
	bool valid;
	struct vector velocity;
	struct polygon *poly, *prevTranslated;
} stars[STARCOUNT];

struct vector STARPOINTS[STARSIZE] = {
	{-0.15f, -0.15f, 0.0f, 1.0f},
	{ 0.0f,  -0.5f,  0.0f, 1.0f},
	{ 0.15f, -0.15f, 0.0f, 1.0f},
	{ 0.5f,   0.0f,  0.0f, 1.0f},
	{ 0.15f,  0.15f, 0.0f, 1.0f},
	{ 0.0f,   0.5f,  0.0f, 1.0f},
	{-0.15f,  0.15f, 0.0f, 1.0f},
	{-0.5f,   0.0f,  0.0f, 1.0f}
};

bool fill;

void drawStar(struct star *star);
void updateStar(unsigned index);
void display(void);
void mpress(int btn, int state, int x, int y);
void resize(GLsizei width, GLsizei height);
void keypress(unsigned char key, int x, int y);
void timer(int val);
struct list *clipStar();

void display(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	for(int i = 0; i < STARCOUNT; i++) {
		updateStar(i);
		drawStar(&stars[i]);
	}

	glFlush();
 	glutSwapBuffers();
}

void timer(int val)
{
	glutTimerFunc(REFRESHMS, timer, val + 1);
	glutPostRedisplay();
}

void drawStar(struct star *star)
{
	if(star->valid) {
		glColor3f(star->r, star->g, star->b);
		polyDraw(star->prevTranslated, false);
	}
}

void updateStar(unsigned i)
{
	if(stars[i].valid) {
		stars[i].x += stars[i].velocity.x;
		stars[i].y += stars[i].velocity.y;
		struct matrix *trans = mtxTranslate(stars[i].x,
																				stars[i].y,
																				0),
			*rotate = mtxRotation(stars[i].rotspeed, 0.0f, 0.0f, 1.0f);
		polyFree(stars[i].prevTranslated);
		polyTransformIP(stars[i].poly, rotate);
		stars[i].prevTranslated = polyTransform(stars[i].poly, trans);
		mtxFree(trans);
		mtxFree(rotate);
		if(polyInRect(stars[i].prevTranslated, 0.0f, 1.0f, 0.0f, 1.0f)) {
			for(unsigned j = 0; j < i; j++) {
				if(stars[j].valid) {
					if(polyIntersect(stars[i].prevTranslated,
													 stars[j].prevTranslated)) {
						stars[i].valid = false;
						stars[j].valid = false;
						break;
					}
				}
			}
		}
		else {
			stars[i].valid = false;
		}
	}
}

void resize(GLsizei width, GLsizei height)
{
	glViewport(0, 0, height, width);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0,
					1.0, 0.0, 1.0);
}

void keypress(unsigned char key, int x, int y)
{
	key = tolower(key);
	switch(key) {
	case 'q':
		exit(0);
	case 'f':
		fill = !fill;
		break;
	case 'r':
		for(int i = 0; i < STARCOUNT; i++) {
			polyFree(stars[i].poly);
			polyFree(stars[i].prevTranslated);
		}
		initStars();
	}
}

void initStars(void)
{
	srand(time(NULL));
	struct polygon *base = polyCreatePoints(STARPOINTS, STARSIZE);
	polyDraw(base, false);
	for(int i = 0; i < STARCOUNT; i++) {
		stars[i].valid = true;
		double tmp = rand();
		stars[i].rotspeed = tmp / RAND_MAX / 10.0f;

		tmp = rand();
		stars[i].velocity.x = tmp / RAND_MAX / 100.0f - 1 / 200.0f;
		tmp = rand();
		stars[i].velocity.y = tmp / RAND_MAX / 100.0f - 1 / 200.0f;

		tmp = rand();
		stars[i].r = tmp / RAND_MAX * (rand() % 255);
		tmp = rand();
		stars[i].g = tmp / RAND_MAX * (rand() % 25);
		tmp = rand();
		stars[i].b = tmp / RAND_MAX * (rand() % 255);

		tmp = rand();
		stars[i].x = (float)(tmp / RAND_MAX);
		tmp = rand();
		stars[i].y = (float)(tmp / RAND_MAX);

		stars[i].poly = NULL;
		bool intersect = true;
		do {	
			tmp = rand();
			stars[i].scale = tmp / RAND_MAX / 35.0f + 0.005f;
			struct matrix  *shift = mtxTranslate(stars[i].x,
																					 stars[i].y,
																					 0),
				*size = mtxScale(stars[i].scale, stars[i].scale, 1.0f);
			if(stars[i].poly) {
				polyFree(stars[i].poly);
				polyFree(stars[i].prevTranslated);
			}
			stars[i].poly = polyTransform(base, size);
			stars[i].prevTranslated = polyTransform(stars[i].poly, shift);
			mtxFree(shift);
			mtxFree(size);
			if(polyInRect(stars[i].prevTranslated, 0.0f, 1.0f, 0.0f, 1.0f)) {
				int j;
				intersect = false;
				for(j = 0; j < i && !intersect; j++) {
					intersect = polyIntersect(stars[i].prevTranslated,
																		stars[j].prevTranslated);
				}
				if(intersect) {
					stars[i].x += stars[j].scale * 4;
				}
			}
			else {
				tmp = rand();
				stars[i].x = (float)(tmp / RAND_MAX);
				tmp = rand();
				stars[i].y = (float)(tmp / RAND_MAX);
			}
		} while(intersect);
	}
}

int main(int argc, char **argv)
{
	fill = false;
	initStars();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 10);
	glutInitWindowSize(VIEWWIDTH + 2 * OFFWIDTH,
										 VIEWHEIGHT + 2 * OFFHEIGHT);
	glutCreateWindow("Astaroids");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keypress);
	glutTimerFunc(REFRESHMS, timer, 0);
	glPointSize(5);
	glLineWidth(2);
	glutMainLoop();
  return 0;
}

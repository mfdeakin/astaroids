
#ifndef __POLYGON_H
#define __POLYGON_H

/* TODO: Make polygon another type of matrix for
 * efficiency reasons such as:
 *   Pointer arithmetic
 *   System calls for memory allocation
 *   Slightly smaller memory footprint */

#include "matrix.h"
#include "list.h"

struct polygon;

struct vector {
	float x, y, z, w;
};

struct polygon *polyCreate(unsigned verts);
struct polygon *polyCreatePoints(struct vector *points,
																 unsigned verts);
struct polygon *polyCreateList(struct list *lst);
struct polygon *polyCopy(struct polygon *src);
void polyFree(struct polygon *poly);

void polySetPoint(struct polygon *poly,
									unsigned index,
									struct vector *pos);
struct vector polyPoint(struct polygon *poly,
										unsigned index);

struct polygon *polyTransform(struct polygon *poly,
															struct matrix *mtx);
void polyTransformIP(struct polygon *poly,
										 struct matrix *mtx);

struct matrix *polyToMatrix(struct polygon *poly);
struct list *polyToPtList(struct polygon *poly);

bool polyInRect(struct polygon *poly,
								float left, float right,
								float top, float bottom);
bool polyIntersect(struct polygon *poly1,
									 struct polygon *poly2);

void polyTranslate(struct polygon *poly,
									 float x, float y, float z);
void polyRotate(struct polygon *poly,
								float angle, float x, float y, float z);
void polyScale(struct polygon *poly,
							 float x, float y, float z);
/* Draws the polygon, filling it if requested */
void polyDraw(struct polygon *poly, bool fill);

#endif

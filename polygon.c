
#include "polygon.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

struct polygon {
	struct matrix *mtx;
	unsigned verts;
};

struct polygon *polyCreate(unsigned verts)
{
	struct polygon *poly = malloc(sizeof(struct polygon));
	poly->mtx = mtxCreate(verts, 4);
	poly->verts = verts;
	return poly;
}

struct polygon *polyCreateList(struct list *lst)
{
	struct polygon *poly = polyCreate(list_size(lst));
	int i;
	list_gotofront(lst);
	for(i = 0; i < poly->verts; i++) {
		struct pt *pt = (struct pt*)list_next(lst);
		mtxSet(poly->mtx, i, 0, pt->x);
		mtxSet(poly->mtx, i, 1, pt->y);
		mtxSet(poly->mtx, i, 2, 0);
		mtxSet(poly->mtx, i, 3, 1);
	}
	return poly;
}

struct polygon *polyCreatePoints(struct vector *points, unsigned verts)
{
	struct polygon *poly = polyCreate(verts);
	for(int i = 0; i < verts; i++) {
		mtxSet(poly->mtx, i, 0, points[i].x);
		mtxSet(poly->mtx, i, 1, points[i].y);
		mtxSet(poly->mtx, i, 2, points[i].z);
		mtxSet(poly->mtx, i, 3, points[i].w);
	}
	return poly;
}

struct polygon *polyCopy(struct polygon *src)
{
	struct polygon *p = malloc(sizeof(struct polygon));
	p->verts = src->verts;
	p->mtx = mtxCopy(src->mtx);
	return p;
}

void polyFree(struct polygon *p)
{
	mtxFree(p->mtx);
	free(p);
}

void polySetPoint(struct polygon *p,
									unsigned i,
									struct vector *pos)
{
	mtxSet(p->mtx, i, 0, pos->x);
	mtxSet(p->mtx, i, 1, pos->y);
	mtxSet(p->mtx, i, 2, pos->z);
	mtxSet(p->mtx, i, 3, pos->w);
}

struct vector polyPoint(struct polygon *p,
										unsigned i)
{
	struct vector point;
	point.x = mtxGet(p->mtx, i, 0);
	point.y = mtxGet(p->mtx, i, 1);
	point.z = mtxGet(p->mtx, i, 2);
	point.w = mtxGet(p->mtx, i, 3);
	return point;
}

struct polygon *polyTransform(struct polygon *poly,
															struct matrix *mtx)
{
	struct polygon *polygon = malloc(sizeof(struct polygon));
	polygon->verts = poly->verts;
	polygon->mtx = mtxMul(mtx, poly->mtx);
	return polygon;
}

void polyTransformIP(struct polygon *poly,
										 struct matrix *mtx)
{
	struct matrix *tmp = mtxMul(mtx, poly->mtx);
	mtxFree(poly->mtx);
	poly->mtx = tmp;
}

bool polyInRect(struct polygon *p,
								float left, float right,
								float top, float bottom)
{
	for(int i = 0; i < p->verts; i++) {
		float x = mtxGet(p->mtx, i, 0),
			y = mtxGet(p->mtx, i, 1);
		if(x < left || x > right)
			return false;
		if(y < top || y > bottom)
			return false;
	}
	return true;
}

bool lineIntersect(struct vector a1, struct vector a2,
									 struct vector b1, struct vector b2);

bool polyIntersect(struct polygon *poly1,
									 struct polygon *poly2)
{
	for(int i = 0; i < poly1->verts; i++) {
		struct vector a1 = polyPoint(poly1, i),
			a2 = polyPoint(poly1, (i + 1) % poly1->verts);
		for(int j = 0; j < poly2->verts; j++) {
			struct vector b1 = polyPoint(poly2, j),
				b2 = polyPoint(poly2, (j + 1) % poly2->verts);
			if(lineIntersect(a1, a2, b1, b2)) {
				return true;
			}
		}
	}
	return false;
}

bool lineIntersect(struct vector a1, struct vector a2,
									 struct vector b1, struct vector b2)
{
	struct vector A = {a2.x - a1.x, a2.y - a1.y, 0.0f, 0.0f},
		u1 = {b1.x - a1.x, b1.y - a1.y, 0.0f, 0.0f},
			u2 = {b2.x - a1.x, b2.y - a1.y, 0.0f, 0.0f};
	float aS1 = A.x * u1.y - A.y * u1.x,
		aS2 = A.x * u2.y - A.y * u2.x;
	if(aS1 == 0 || aS2 == 0)
		/* We don't consider it interesection if b1 or b2 is on A */
		return false;
	if((aS1 < 0 && aS2 < 0) ||
		 (aS1 > 0 && aS2 > 0))
		/* Same sign, so b1 and b2 are on the same side of A */
		return false;
	struct vector B = {b2.x - b1.x, b2.y - b1.y, 0.0f, 0.0f},
		u3 = {a2.x - b1.x, a2.y - b1.y, 0.0f, 0.0f};
	float bS1 = u1.x * B.y - u1.y * B.x,
		bS2 = B.x * u3.y - B.y * u3.x;
	if((bS1 < 0 && bS2 < 0) ||
		 (bS1 > 0 && bS2 > 0))
		return false;
	return true;
}

struct matrix *polyToMatrix(struct polygon *poly)
{
	return mtxCopy(poly->mtx);
}

struct list *polyToPtList(struct polygon *poly)
{
	struct list *lst = list_create(0);
	int i;
	for(i = 0; i < poly->verts; i++) {
		struct vector buffer = polyPoint(poly, i),
			*point = malloc(sizeof(struct pt));
		*point = buffer;
		list_insert(lst, point);
	}
	return lst;
}

void polyTranslate(struct polygon *poly,
									 float x, float y, float z)
{
	struct matrix *trans = mtxTranslate(x, y, z);
	struct matrix *newpoints = mtxMul(trans, poly->mtx);
	mtxFree(trans);
	mtxFree(poly->mtx);
	poly->mtx = newpoints;
}

void polyScale(struct polygon *poly,
							 float x, float y, float z)
{
	struct matrix *trans = mtxScale(x, y, z);
	struct matrix *newpoints = mtxMul(trans, poly->mtx);
	mtxFree(trans);
	mtxFree(poly->mtx);
	poly->mtx = newpoints;
}

void polyRotate(struct polygon *poly,
								float theta, float x, float y, float z)
{
	struct matrix *rot = mtxRotation(theta, x, y, z);
	struct matrix *newpoints = mtxMul(rot, poly->mtx);
	mtxFree(rot);
	mtxFree(poly->mtx);
	poly->mtx = newpoints;
}

void polyDraw(struct polygon *poly, bool fill)
{
	if(fill)
		glBegin(GL_POLYGON);
	else
		glBegin(GL_LINE_LOOP);
	for(int i = 0; i < poly->verts; i++) {
		struct vector buf = polyPoint(poly, i);
		/* printf("x: %11.8f  y: %11.8f  z: %11.8f\n", */
		/* 			 buf.x, buf.y, buf.z); */
		glVertex3f(buf.x, buf.y, buf.z);
	}
	glEnd();
	/* printf("\n"); */
}

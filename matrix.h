
#ifndef __MATRIX_H
#define __MATRIX_H

#include "draw.h"

struct matrix;

struct matrix *mtxCreate(unsigned width, unsigned height);
struct matrix *mtxCreateI(unsigned size);
struct matrix *mtxCopy(struct matrix *mtx);
struct matrix *mtxFromArray(float array[], unsigned w, unsigned h);
void mtxFree(struct matrix *mtx);
struct matrix *mtxAdd(struct matrix *lhs, struct matrix *rhs);
struct matrix *mtxNeg(struct matrix *mtx);
struct matrix *mtxMul(struct matrix *lhs, struct matrix *rhs);
float mtxDeterminate(struct matrix *mtx);
float mtxGet(struct matrix *mtx, unsigned x, unsigned y);
int mtxSet(struct matrix *mtx, unsigned x, unsigned y, float val);

struct matrix *mtxTranslate(float x, float y, float z);
struct matrix *mtxScale(float x, float y, float z);
struct matrix *mtxRotation(float theta, float x, float y, float z);

struct pt mtxToPoint(struct matrix *mtx);

#endif

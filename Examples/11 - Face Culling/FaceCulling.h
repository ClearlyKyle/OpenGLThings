#ifndef __FACECULLING_H__
#define __FACECULLING_H__

#include "Engine.h"

struct FaceCulling
{
    struct Camera cam;
    struct Mesh model;
};

void FaceCulling_Init();
void FaceCulling_Update();
void FaceCulling_OnExit();

#endif // __FACECULLING_H__
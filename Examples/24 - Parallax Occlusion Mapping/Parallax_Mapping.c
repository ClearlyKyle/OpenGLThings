#include "Parallax_Mapping.h"

static struct ParallaxMapping
{
    Camera_t cam;
} pm;

void ParallaxMapping_Init()
{
    Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    pm.cam       = cam;
}

void ParallaxMapping_Update()
{
    Camera_Inputs(&pm.cam);
}

void ParallaxMapping_OnExit()
{
    Camera_Print_Values(pm.cam);
}

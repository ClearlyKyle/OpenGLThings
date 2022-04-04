#include <stdio.h>
#include <stdlib.h>

#include "BasicTriangle.h"
//#include "TextureExample.h"
//#include "CameraExample.h"
//#include "LightingExample.h"
//#include "10_SpecularMaps.h"
//#include "challanges.h"
//#include "ModelLoadingExample.h"

int main(int argc, char *argv[])
{
    printf("STARTING MAIN\n");
    
    // Example 1 - Basic Triangle
    Window_Init(800, 800, BasicTriangle_Init, BasicTriangle_Update, BasicTriangle_OnExit);

    //  Window_Init(800, 800, Basic3D_Init, Basic3D_Update, Basic3D_OnExit);
    // Window_Init(800, 800, LightingExample_Init, LightingExample_Update, LightingExample_OnExit);
    // Window_Init(800, 800, challanges_Init, challanges_Update, challanges_OnExit);
    // Window_Init(800, 800, ModelLoading_Init, ModelLoading_Update, ModelLoading_OnExit);

    Window_Loop();

    printf("EXITING MAIN\n");
    return 0;
}

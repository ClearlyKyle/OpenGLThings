#include <stdio.h>
#include <stdlib.h>

#include "BasicTriangle.h"       // Example 1 - Basic Triangle
#include "ShaderColour.h"        // Example 2 - Basic Shader Colour
#include "TextureExample.h"      // Example 3 - Texture Drawing
#include "Basic3D.h"             // Example 4 - Basic 3D
#include "CameraExample.h"       // Example 5 - 3D Camera (FPS Style)
#include "LightingExample.h"     // Example 6 - Basic Lighting
#include "SpecularMaps.h"        // Example 7 - Specular Maps with Various Lighting Types
#include "ModelLoadingExample.h" // Example 8 - Loading A Model from OBJ File
//#include "challanges.h"

int main(int argc, char *argv[])
{
    printf("STARTING MAIN\n");

    // Example 1 - Basic Triangle
    // Window_Init(800, 800, BasicTriangle_Init, BasicTriangle_Update, BasicTriangle_OnExit);

    // Example 2 - Basic Shader Colour
    // Window_Init(800, 800, ShaderColour_Init, ShaderColour_Update, ShaderColour_OnExit);

    // Example 3 - Texture Drawing
    // Window_Init(800, 800, TextureExample_Init, TextureExample_Update, TextureExample_OnExit);

    // Example 4 - Basic 3D
    // Window_Init(800, 800, Basic3D_Init, Basic3D_Update, Basic3D_OnExit);

    // Example 5 - 3D Camera (FPS Style)
    // Window_Init(800, 800, CameraExample_Init, CameraExample_Update, CameraExample_OnExit);

    // Example 6 - Basic Lighting
    // Window_Init(800, 800, LightingExample_Init, LightingExample_Update, LightingExample_OnExit);

    // Example 7 - Specular Maps with Various Lighting Types
    // Window_Init(800, 800, SpecularMaps_Init, SpecularMaps_Update, SpecularMaps_OnExit);

    // Example 8 - Loading A Model from OBJ File
    Window_Init(800, 800, ModelLoading_Init, ModelLoading_Update, ModelLoading_OnExit);

    // Testing things
    // Window_Init(800, 800, challanges_Init, challanges_Update, challanges_OnExit);

    Window_Loop();

    printf("EXITING MAIN\n");
    return 0;
}

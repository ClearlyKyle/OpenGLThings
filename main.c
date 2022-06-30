#include <stdio.h>
#include <stdlib.h>

//#include "BasicTriangle.h"       // Example 1 - Basic Triangle
//#include "ShaderColour.h"        // Example 2 - Basic Shader Colour
//#include "TextureExample.h"      // Example 3 - Texture Drawing
//#include "Basic3D.h"             // Example 4 - Basic 3D
//#include "CameraExample.h"       // Example 5 - 3D Camera (FPS Style)
//#include "LightingExample.h"     // Example 6 - Basic Lighting
//#include "SpecularMaps.h"        // Example 7 - Specular Maps with Various Lighting Types
//#include "ModelLoadingExample.h" // Example 8 - Loading A Model from OBJ File
//#include "DepthBuffer.h"         // Example 9 - Visualise Depth Buffer
//#include "StencilBuffer.h"       // Example 10 - Stencil Buffer
//#include "FaceCulling.h"         // Example 11 - Face Culling
//#include "Transparancy.h"        // Example 12 - Transparancy
//#include "FrameProcess.h"        // Example 13 - Framebuffer and Post Process
//#include "CubeBoxes.h"           // Example 14 - Cubemaps & Skyboxes
//#include "Geometry.h"            // Example 15 - Geometry Shader
//#include "Instancing.h"          // Example 16 - Instancing
//#include "AntiAliasing.h"        // Example 17 - Anti-Aliasing (MSAA)
//#include "BlinnPhong.h"          // Example 18 - Blin-Phong Lighting
//#include "Gamma.h"               // Example 19 - Gamma Correction
//#include "UBOExample.h"          // Example 20 - Drawing Triangles with use of UBO
//#include "SM_With_Cubes.h"       // Example 21 - Shadow Mapping with Directional Lighting Using Cubes
//#include "SM_With_Model.h"       // Example 21 - Shadow Mapping with Directional Lighting Using a Model
//#include "SM_With_SP_PL.h"       // Example 22 - Shadow Mapping with Spotlights and Point Lights
//#include "Normal_Maps.h"         // Example 23 - Normal Mapping
//#include "Parallax_Mapping.h"    // Example 24 - Parallax Occluision Mapping
//#include "HDR.h"                 // Example 25 - HDR
//#include "Bloom.h"               // Example 26 - Bloom
//#include "Procedural.h"          // Example 27 - Procedural Generation
//#include "Compute.h"             // Example 28 - Compute Shaders
//#include "Ex1/Deferred.h"  // Example 29 - Deferred Shading
#include "Ex2/Deferred2.h" // Example 29 - Deferred Shading Another example
//#include "Breakout.h"            // Example 32 - Breakout
//#include "Mesh_Test.h"
//#include "Ortho.h"
//#include "challanges.h"
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
    // Window_Init(800, 800, ModelLoading_Init, ModelLoading_Update, ModelLoading_OnExit);

    // Example 9 - Visualise Depth Buffer
    // Window_Init(800, 800, DepthBuffer_Init, DepthBuffer_Update, DepthBuffer_OnExit);

    // Example 10 - Stencil Buffer
    // Window_Init(800, 800, StencilBuffer_Init, StencilBuffer_Update, StencilBuffer_OnExit);

    // Example 11 - Face Culling
    // Window_Init(800, 800, FaceCulling_Init, FaceCulling_Update, FaceCulling_OnExit);

    // Example 12 - Transparancy
    // Window_Init(800, 800, Transparancy_Init, Transparancy_Update, Transparancy_OnExit);

    // Example 13 - Framebuffer and Post Process
    // Window_Init(800, 800, FrameProcess_Init, FrameProcess_Update, FrameProcess_OnExit);

    // Example 14 - Cubemaps & Skyboxes
    // Window_Init(800, 800, CubeBoxes_Init, CubeBoxes_Update, CubeBoxes_OnExit);

    // Example 15 - Geometry Shader
    // Window_Init(800, 800, Geometry_Init, Geometry_Update, Geometry_OnExit);

    // Example 16 - Instancing
    // Window_Init(800, 800, Instancing_Init, Instancing_Update, Instancing_OnExit);

    // Example 17 - Anti-Aliasing (MSAA)
    // Window_Init(800, 800, AntiAliasing_Init, AntiAliasing_Update, AntiAliasing_OnExit);

    // Example 18 - Blinn-Phong Lighting
    // Window_Init(800, 800, BlinnPhong_Init, BlinnPhong_Update, BlinnPhong_OnExit);

    // Example 19 - Gamma Correction
    // Window_Init(800, 800, Gamma_Init, Gamma_Update, Gamma_OnExit);

    // Example 20 - Drawing Triangles with use of UBO
    // Window_Init(800, 800, UBOExample_Init, UBOExample_Update, UBOExample_OnExit);

    // Example 21 - Shadow Mapping with Directional Lighting Using Cubes
    // Window_Init(1024, 1024, ShadowMapCubes_Init, ShadowMapCubes_Update, ShadowMapCubes_OnExit);

    // Example 21 - Shadow Mapping with Directional Lighting Using a Model
    // Window_Init(1024, 1024, ShadowMapModel_Init, ShadowMapModel_Update, ShadowMapModel_OnExit);

    // Example 22 - Shadow Mapping with Spotlights and Point Lights
    // Window_Init(1024, 1024, ShadowMapLights_Init, ShadowMapLights_Update, ShadowMapLights_OnExit);

    // Example 23 - Normal Mapping
    // Window_Init(1024, 1024, NormalMaps_Init, NormalMaps_Update, NormalMaps_OnExit);

    // Example 24 - Parallax Occluision Mapping
    // Window_Init(1024, 1024, ParallaxMapping_Init, ParallaxMapping_Update, ParallaxMapping_OnExit);

    // Example 25 - HDR
    // Window_Init(1024, 1024, HDR_Init, HDR_Update, HDR_OnExit);

    // Example 26 - Bloom
    // Window_Init(1024, 1024, Bloom_Init, Bloom_Update, Bloom_OnExit);

    // Example 27 - Procedural Generation
    // Window_Init(1024, 1024, Procedural_Init, Procedural_Update, Procedural_OnExit);

    // Example 28 - Compute Shaders
    // Window_Init(1024, 1024, Compute_Init, Compute_Update, Compute_OnExit);

    // Example 29 - Deferred Shading
    // Window_Init(800, 800, Deferred_Init, Deferred_Update, Deferred_OnExit);
    Window_Init(800, 800, Deferred2_Init, Deferred2_Update, Deferred2_OnExit);

    // Example 32 - Breakout
    // Window_Init(1024, 1024, Breakout_Init, Breakout_Update, Breakout_OnExit);

    // Window_Init(1024, 1024, MeshTest_Init, MeshTest_Update, MeshTest_OnExit);
    //  Window_Init(800, 800, Ortho_Init, Ortho_Update, Ortho_OnExit);

    // Testing things
    // Window_Init(800, 800, challanges_Init, challanges_Update, challanges_OnExit);

    Window_Loop();

    printf("EXITING MAIN\n");
    return 0;
}

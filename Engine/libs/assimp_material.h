#ifndef __ASSIMP_MATERIAL_H__
#define __ASSIMP_MATERIAL_H__

#include <assimp/ai_assert.h>
#include <assimp/material.h>

// -------------------------------------------------------------------------------
const char *TextureTypeToString(C_ENUM aiTextureType in)
{
    switch (in)
    {
    case aiTextureType_NONE:
        return "n/a";
    case aiTextureType_DIFFUSE:
        return "Diffuse";
    case aiTextureType_SPECULAR:
        return "Specular";
    case aiTextureType_AMBIENT:
        return "Ambient";
    case aiTextureType_EMISSIVE:
        return "Emissive";
    case aiTextureType_OPACITY:
        return "Opacity";
    case aiTextureType_NORMALS:
        return "Normals";
    case aiTextureType_HEIGHT:
        return "Height";
    case aiTextureType_SHININESS:
        return "Shininess";
    case aiTextureType_DISPLACEMENT:
        return "Displacement";
    case aiTextureType_LIGHTMAP:
        return "Lightmap";
    case aiTextureType_REFLECTION:
        return "Reflection";
    case aiTextureType_BASE_COLOR:
        return "BaseColor";
    case aiTextureType_NORMAL_CAMERA:
        return "NormalCamera";
    case aiTextureType_EMISSION_COLOR:
        return "EmissionColor";
    case aiTextureType_METALNESS:
        return "Metalness";
    case aiTextureType_DIFFUSE_ROUGHNESS:
        return "DiffuseRoughness";
    case aiTextureType_AMBIENT_OCCLUSION:
        return "AmbientOcclusion";
    // case aiTextureType_SHEEN:
    //     return "Sheen";
    // case aiTextureType_CLEARCOAT:
    //     return "Clearcoat";
    // case aiTextureType_TRANSMISSION:
    //     return "Transmission";
    case aiTextureType_UNKNOWN:
        return "Unknown";
    default:
        break;
    }
    ai_assert(false);
    return "BUG";
}
#endif // __ASSIMP_MATERIAL_H__
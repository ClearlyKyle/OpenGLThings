#ifndef __GBUFFER_H__
#define __GBUFFER_H__

#include "Engine.h"

enum GBUFFER_TEXTURE_TYPE
{
    GBUFFER_TEXTURE_TYPE_POSITION,
    GBUFFER_TEXTURE_TYPE_DIFFUSE,
    GBUFFER_TEXTURE_TYPE_NORMAL,
    GBUFFER_TEXTURE_TYPE_TEXCOORD,
    GBUFFER_NUM_TEXTURES
};

struct GBuffer
{
    GLuint fbo;
    GLuint textures[GBUFFER_NUM_TEXTURES];
    GLuint depthTexture;
};

typedef struct GBuffer GBuffer_t;

GBuffer_t GBuffer_Init(unsigned int WindowWidth, unsigned int WindowHeight);
void      GBuffer_BindForWriting(const GBuffer_t *const buf);
void      GBuffer_BindForReading(const GBuffer_t *const buf);
void      GBuffer_SetReadBuffer(const enum GBUFFER_TEXTURE_TYPE TextureType);
void      GBuffer_Destroy(GBuffer_t *buf);

#endif // __GBUFFER_H__
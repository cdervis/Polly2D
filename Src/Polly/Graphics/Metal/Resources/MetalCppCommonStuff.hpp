#pragma once

// Keep this in sync with AllShaders.metal!

// [[buffer(n)]] slots
enum MetalBufferSlots
{
    MTLBufferSlot_GlobalCBuffer  = 0,
    MTLBufferSlot_SpriteVertices = 1,
    MTLBufferSlot_PolyVertices   = 2,
    MTLBufferSlot_MeshVertices   = 3,
};

// [[texture(n)]] slots
enum MetalTextureSlots
{
    MTLTextureSlot_SpriteImage        = 0,
    MTLTextureSlot_SpriteImageSampler = 0,
    MTLTextureSlot_MeshImage          = 2,
    MTLTextureSlot_MeshImageSampler   = 0,
};

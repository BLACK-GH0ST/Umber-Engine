//*********************************************************
//
// Copyright (c) Umbra Engine. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

/**
 * @Color Axis.hlsl contains only the shader for New level and Map for the Engine 
 * @brief makes a grid lines for the level.
 * @brief makes a distance fade (fog).
 *
 * Features:
 * - Fog for Distance of far grid 
 * - grid scaling + nicer map-view controls
 * - basic 3D world primitives for the new map view
 */

cbuffer VSConstants : register(b0) 
{
    float4x4 gViewProj;
  
    float3 cameraPos;
    float nearFog;
    
    float farFog;
    float3 colorFog;
};

struct VSIn
{
    float3 pos : POSITION;
    float4 col : COLOR;
};

struct VSOut
{
    float4 pos : SV_Position;
    float4 col : COLOR;
    float3 worldPos : TEXCOORD0;
};

VSOut VSMain(VSIn i)
{
    VSOut o;
    o.pos = mul(float4(i.pos, 1.0f), gViewProj);
    o.col = i.col;
    o.worldPos = i.pos;
    return o;
}

float4 PSMain(VSOut i) : SV_Target
{
  float dist = length(i.worldPos - cameraPos);
  float fogFactor = saturate((dist - nearFog) / (farFog - nearFog));
  float3 finalColor = lerp(i.col.rgb, colorFog, fogFactor);
  return float4(finalColor, 1.0f);
  
}

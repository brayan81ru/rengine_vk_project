#include "Structures.fxh"

struct VSInput
{
    float3 Pos     : ATTRIB0;
    float3 Normal  : ATTRIB1;
    float2 UV      : ATTRIB2;
    float3 Tangent : ATTRIB3;
};

struct PSInput
{
    float4 Pos     : SV_POSITION;
    float3 WorldPos: WORLDPOS;
    float3 Normal  : NORMAL;
    float2 UV      : TEXCOORD0;
    float3 Tangent : TANGENT;
};

void main(in VSInput VSIn, out PSInput PSIn)
{
    PSIn.WorldPos = mul(float4(VSIn.Pos, 1.0), g_World).xyz;
    PSIn.Pos = mul(float4(VSIn.Pos, 1.0), g_WorldViewProj);
    PSIn.Normal = normalize(mul(VSIn.Normal, (float3x3)g_WorldInvTranspose));
    PSIn.Tangent = normalize(mul(VSIn.Tangent, (float3x3)g_World));
    PSIn.UV = VSIn.UV;
}
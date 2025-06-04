// Common constant buffers
cbuffer CameraConstants : register(b0)
{
    float4x4 g_World;
    float4x4 g_WorldViewProj;
    float4x4 g_WorldInvTranspose;
    float3   g_CameraPos;
    float    g_Padding0;
};

cbuffer LightConstants : register(b1)
{
    float3 g_LightDirection;
    float  g_LightIntensity;
    float3 g_LightColor;
    float  g_Padding1;
};

// Vertex structures
struct VertexAttribs
{
    float3 Pos     : ATTRIB0;
    float3 Normal  : ATTRIB1;
    float2 UV      : ATTRIB2;
    float3 Tangent : ATTRIB3;
};

// Common texture samplers
SamplerState g_LinearSampler : register(s0);
SamplerState g_PointSampler  : register(s1);
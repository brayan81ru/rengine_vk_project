#include "Structures.fxh"

TextureCube g_Skybox : register(t10);

// PBR material properties
cbuffer MaterialConstants : register(b2)
{
    float3 g_Albedo;
    float  g_Metallic;
    float  g_Roughness;
    float  g_AO;
    float2 g_Padding2;
};

// Normal distribution function (Trowbridge-Reitz GGX)
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

// Geometry function (Schlick GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// Fresnel equation (Schlick approximation)
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Normal mapping calculation
float3 CalculateNormalMap(float3 normal, float3 tangent, Texture2D normalMap, float2 uv)
{
    // Unpack normal from texture
    float3 tangentNormal = normalMap.Sample(g_LinearSampler, uv).xyz * 2.0 - 1.0;

    // Create TBN matrix
    float3 N = normalize(normal);
    float3 T = normalize(tangent);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);

    // Transform normal to world space
    return normalize(mul(tangentNormal, TBN));
}

// Main PBR lighting calculation
float3 CalculatePBRLighting(float3 albedo, float3 normal, float metallic, float roughness, float3 worldPos)
{
    float3 N = normalize(normal);
    float3 V = normalize(g_CameraPos - worldPos);

    // Calculate reflectance at normal incidence
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);

    // Direct lighting
    float3 L = normalize(-g_LightDirection);
    float3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySchlickGGX(max(dot(N, V), 0.0), roughness) *
              GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    float3 kS = F;
    float3 kD = (float3(1.0, 1.0, 1.0) - kS) * (1.0 - metallic);

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);

    // Combine terms
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * g_LightColor * g_LightIntensity * NdotL;
}
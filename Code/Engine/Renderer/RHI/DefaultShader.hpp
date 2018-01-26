#pragma once

static const char* DEFAULT_SHADER = R"(
struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoords : TEXCOORDS;
	float4 colorValues : COLORS;
};

struct VertexShaderOutputToFragment
{
	float4 position : SV_Position;
	float3 normal : NORMAL;
	float2 texCoords : TEXCOORDS;
	float4 colorValues : COLORS;
	float3 worldPosition : WORLD;
};

cbuffer matrix_cb : register(b0)
{
	float4x4 MODEL;
	float4x4 VIEW;
	float4x4 PROJECTION;
}

cbuffer time_cb : register(b1)
{
	float1 TIME;
	float3 PADDING;
}

cbuffer light_cb : register(b2)
{
	float4 AMBIENT;
	float4 LIGHT_COLOR;
	float4 LIGHT_POSITION;
	float4 ATTENUATION;
}

Texture2D<float4> tTexture : register(t0);
SamplerState sSampler : register(s0);
SamplerState sPoint : register(s1);

//
// Vertex Shader
//
VertexShaderOutputToFragment VertexFunction(VertexShaderInput vertex)
{
	VertexShaderOutputToFragment outData = (VertexShaderOutputToFragment)0;

	float4 modelSpace = float4(vertex.position, 1.0f);
	float4 worldSpace = mul(modelSpace, MODEL);
	float4 viewSpace = mul(worldSpace, VIEW);
	float4 clipSpace = mul(viewSpace, PROJECTION);

	outData.position = clipSpace;
	outData.normal = mul(float4(vertex.normal, 0.0f), MODEL).xyz;
	outData.texCoords = vertex.texCoords;
	outData.colorValues = vertex.colorValues;

	return outData;
}

//
// Fragment Shader
//
float4 FragmentFunction(VertexShaderOutputToFragment data) : SV_Target0
{
	float4 color = tTexture.Sample(sPoint, data.texCoords);

	float4 t = color * data.colorValues;

	if(data.texCoords.x == 0 && data.texCoords.y == 0)
	{
		t = data.colorValues;
	}

	return t;
}
)";
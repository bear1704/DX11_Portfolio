static const int kSkyTextureNumber = 6;

Texture2D g_txDiffuse : register(t0);
Texture2D g_txDiffuseArray[kSkyTextureNumber]: register(t1);
SamplerState s0 : register(s0);

struct VS_INPUT
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXCOORD;
};
struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};
struct PS_INPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	uint primitiveID : SV_PrimitiveID;
};
cbuffer cb0 : register(b0)
{
	matrix g_matWorld: packoffset(c0);// 상수레지스터
	matrix g_matView : packoffset(c4);
	matrix g_matProj : packoffset(c8);
	float4 g_Color : packoffset(c12);
	float  g_fTime : packoffset(c13.x);
};
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT pOut = (VS_OUTPUT)0;
	// world
	pOut.p = mul(float4(input.p, 1.0f), g_matWorld);// local * g_matWorld;
	pOut.p = mul(pOut.p, g_matView);// world * g_matView;
	pOut.p = mul(pOut.p, g_matProj);// view * g_matWorld;	
	pOut.p = pOut.p.xyww;

	pOut.t = input.t;
	pOut.n = input.n;
	pOut.c = input.c;
	return pOut;
}

float4 PS2(PS_INPUT input) : SV_TARGET
{
	float4 color = g_txDiffuse.Sample(s0 , input.t);// *input.c;
	return color;
}

float4 GetTextureColor(uint index, float2 uv)
{
	float4 color = float4(1, 1, 1, 1);
	switch (index)
	{
	case 0:
		color = g_txDiffuseArray[0].Sample(s0, uv); break;
	case 1:
		color = g_txDiffuseArray[1].Sample(s0, uv); break;
	case 2:
		color = g_txDiffuseArray[2].Sample(s0, uv); break;
	case 3:
		color = g_txDiffuseArray[3].Sample(s0, uv); break;
	case 4:
		color = g_txDiffuseArray[4].Sample(s0, uv); break;
	case 5:
		color = g_txDiffuseArray[5].Sample(s0, uv); break;
	}
	return color;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
	uint index = input.primitiveID / 2;
	float4 color = GetTextureColor(index, input.t);
	return color;
}
float4 PS_Color(VS_OUTPUT input) : SV_TARGET
{
	float4 color = input.c;
	return color;
}

#define WIDEN(x) g_##x
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
TextureCube g_txDiffuse: register (t0);
SamplerState g_samLinear: register (s0);
cbuffer cb0: register(b0)
{
	matrix	g_matWorld		: packoffset(c0);
	matrix	g_matView		: packoffset(c4);
	matrix	g_matProj		: packoffset(c8);
	float4  g_MeshColor     : packoffset(c12);
};
//--------------------------------------------------------------------------------------
//Lighting Variables
//--------------------------------------------------------------------------------------
cbuffer cb1: register(b1)
{
	matrix				g_matWorldInverse: packoffset(c0);
	float4				g_AmbientMaterial: packoffset(c4);
	float4				g_DiffuseMaterial: packoffset(c5);
	float4				g_cSpecularMaterial: packoffset(c6);
	float4				g_cEmissionMaterial: packoffset(c7);

	float4				g_AmbientLightColor : packoffset(c8);
	float4				g_DiffuseLightColor: packoffset(c9);
	float4				g_cSpecularLightColor: packoffset(c10);
	float3				g_vLightDir : packoffset(c11);
	float			    g_fDamping : packoffset(c11.w);
	float3				g_vLightPos : packoffset(c12);
	float			    g_fRadius : packoffset(c12.w);
	float3				g_vEyeDir : packoffset(c13);
	float			    g_fIntensity : packoffset(c13.w);
	float3				g_vEyePos : packoffset(c14);
	float			    g_fEyeRadius : packoffset(c14.w);
};
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXCOORD;
};
//struct VS_OUTPUT
//{
//	float4 p : SV_POSITION;
//	float3 n : NORMAL;
//	float4 c : COLOR0;
//	float2 t : TEXCOORD0;
//	float4 d : TEXCOORD1;
//};
struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	float3 r : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 Diffuse(float3 vNormal)
{
	float fIntensity = max(0, dot(vNormal, normalize(-g_vLightDir)));
	float4 diffuse = g_AmbientMaterial * g_AmbientLightColor +
		(g_DiffuseMaterial * g_DiffuseLightColor * fIntensity);
	return diffuse;
}
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT vOut = (VS_OUTPUT)0;
	vOut.p = mul(vIn.p, WIDEN(matWorld));
	float4 worldpos = vOut.p;
	vOut.p = mul(vOut.p, WIDEN(matView));
	vOut.p = mul(vOut.p, WIDEN(matProj));
	vOut.n = normalize(mul(vIn.n, (float3x3)g_matWorldInverse));

	
	float3 incident = normalize(worldpos - g_vEyePos);
	vOut.r = normalize(reflect(incident, vOut.n));

	vOut.t = vIn.t * 10;
	vOut.c.x = dot(vOut.n, normalize(-g_vLightDir));
	vOut.c.y = dot(vOut.n, normalize(-g_vLightDir));// vIn.c * g_MeshColor;
	vOut.c.z = dot(vOut.n, normalize(-g_vLightDir));
	vOut.c.w = 1.0f;


	return vOut;
}


float4 PS(VS_OUTPUT vIn) : SV_Target
{
	//float3 R = normalize(reflect(g_vEyeDir, vIn.n));
	
	float4 vTexColor = g_txDiffuse.Sample(g_samLinear, vIn.r);
	//vTexColor.a = 1.0f;
	//float4 vFinalColor = vTexColor * Diffuse(vIn.n) * vIn.c;
	return vTexColor;
}

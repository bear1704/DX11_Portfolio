#define WIDEN(x) g_##x
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D g_txDiffuse: register (t0);
SamplerState g_samLinear: register (s0);

cbuffer cb0: register(b0)
{
	matrix	g_matWorld		: packoffset(c0);
	matrix	g_matView		: packoffset(c4);
	matrix	g_matProj		: packoffset(c8);
	float4  g_MeshColor     : packoffset(c12);
};

cbuffer cbChangesEveryFrame : register(b1)
{
	matrix				cb_mat_normal		:packoffset(c0);
	float3				cb_vec_light		:packoffset(c4);
	float				cb_padding1			:packoffset(c4.w);
	float3				cb_vec_camera_pos	:packoffset(c5);
	float				cb_padding2			:packoffset(c5.w);
	float3				cb_vec_eye_dir		:packoffset(c6);
	float				cb_padding3			:packoffset(c6.w);
};

cbuffer cbNearlyNotChange: register(b2)
{
	float4				cb_AmbientMaterial	: packoffset(c0);
	float4				cb_DiffuseMaterial	: packoffset(c1);
	float4				cb_SpecularMaterial	: packoffset(c2);

	float4				cb_AmbientLightColor	: packoffset(c3);
	float4				cb_DiffuseLightColor	: packoffset(c4);
	float4				cb_SpecularLightColor	: packoffset(c5);

	float				cb_is_dirty						: packoffset(c6.x);
	float				cb_SpecularIntensity			: packoffset(c6.y);
	float2				cb_notchange_padding			: packoffset(c6.z);
	//float				cb_notchange_padding2			: packoffset(c6.w);
}

//--------------------------------------------------------------------------------------
//Lighting Variables
//--------------------------------------------------------------------------------------
//cbuffer cb1: register(b1)
//{
//	matrix				g_matWorldInverse: packoffset(c0);
//	float4				g_AmbientMaterial: packoffset(c4);
//	float4				g_DiffuseMaterial: packoffset(c5);
//	float4				g_cSpecularMaterial: packoffset(c6);
//	float4				g_cEmissionMaterial: packoffset(c7);
//
//	float4				g_AmbientLightColor : packoffset(c8);
//	float4				g_DiffuseLightColor: packoffset(c9);
//	float4				g_cSpecularLightColor: packoffset(c10);
//	float3				g_vLightDir : packoffset(c11);
//	float			    g_fDamping : packoffset(c11.w);
//	float3				g_vLightPos : packoffset(c12);
//	float			    g_fRadius : packoffset(c12.w);
//	float3				g_vEyeDir : packoffset(c13);
//	float			    g_fIntensity : packoffset(c13.w);
//	float3				g_vEyePos : packoffset(c14);
//	float			    g_fEyeRadius : packoffset(c14.w);
//};
//--------------------------------------------------------------------------------------
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
	float4 d : TEXCOORD1;
};
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 Diffuse(float3 vNormal)
{
	float diffuse_intensity = max(0, dot(vNormal, normalize(-cb_vec_light)));
	float4 diffuse = cb_AmbientMaterial * cb_AmbientLightColor +
		(cb_DiffuseMaterial * cb_DiffuseLightColor * diffuse_intensity);
	return diffuse;
}

float4 Specular(float3 vNormal)
{
	// Specular Lighting
	float  fPower = 0.0f;
#ifndef HALF_VECTOR
	float3 R = reflect(-cb_vec_light, vNormal);
	fPower = pow(saturate(dot(R, -cb_vec_eye_dir)), cb_SpecularIntensity);
#else
	float3 vHalf = normalize(-cb_vec_light + -cb_vec_eye_dir);
	fPower = pow(saturate(dot(vNormal, vHalf)), 30.0f);
#endif
	float4 specular = cb_SpecularMaterial * cb_SpecularLightColor * fPower;
	return specular;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT vOut = (VS_OUTPUT)0;
	
	vOut.p = mul(float4(vIn.p, 1.0f), WIDEN(matWorld));
	vOut.p = mul(vOut.p, WIDEN(matView));
	vOut.p = mul(vOut.p, WIDEN(matProj));
	vOut.n = vIn.n;
	vOut.t = vIn.t;
	vOut.c.x = dot(vOut.n, normalize(-cb_vec_light));
	vOut.c.y = dot(vOut.n, normalize(-cb_vec_light));// vIn.c * g_MeshColor;
	vOut.c.z = dot(vOut.n, normalize(-cb_vec_light));
	vOut.c.w = 1.0f;
	return vOut;
}


float4 PS(VS_OUTPUT vIn) : SV_Target
{
	float4 vTexColor = g_txDiffuse.Sample(g_samLinear, vIn.t);
	float4 vFinalColor = vTexColor * (Diffuse(vIn.n) + Specular(vIn.n)) * vIn.c;
	vFinalColor.a = 1.0f;
	return vFinalColor;
}


////--------------------------------------------------------------------------------------
//// Pixel Shader
////--------------------------------------------------------------------------------------
//float4 PS_Texture(VS_OUTPUT vIn) : SV_Target
//{
//	return g_txDiffuse.Sample(g_samLinear, vIn.n);
//}
////--------------------------------------------------------------------------------------
//// Pixel Shader
////--------------------------------------------------------------------------------------
//float4 PS_Color(VS_OUTPUT vIn) : SV_Target
//{
//	return vIn.c;
//}

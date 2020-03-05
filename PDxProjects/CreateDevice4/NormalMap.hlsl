//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D	g_txDiffuse : register (t0);
Texture2D	g_txNormalMap	: register (t1);
SamplerState g_samLinear: register (s0);

cbuffer cb0 : register(b0)
{
	matrix	g_matWorld		: packoffset(c0);
	matrix	g_matView		: packoffset(c4);
	matrix	g_matProj		: packoffset(c8);
	float4  g_MeshColor     : packoffset(c12);
};

//-----------------------------------------------------------------------------
//상수 버퍼
//-----------------------------------------------------------------------------

cbuffer cbChangesEveryFrame : register(b1)
{
	matrix				cb_mat_normal		:packoffset(c0);
	float3				cb_vec_light		:packoffset(c4);
	float				cb_padding1 : packoffset(c4.w);
	float3				cb_vec_camera_pos	:packoffset(c5);
	float				cb_padding2 : packoffset(c5.w);
	float3				cb_vec_eye_dir		:packoffset(c6);
	float				cb_padding3 : packoffset(c6.w);
};


cbuffer cbNearlyNotChange: register(b2)
{
	float4				cb_AmbientMaterial	: packoffset(c0);
	float4				cb_DiffuseMaterial	: packoffset(c1);
	float4				cb_SpecularMaterial	: packoffset(c2);

	float4				cb_AmbientLightColor	: packoffset(c3);
	float4				cb_DiffuseLightColor	: packoffset(c4);
	float4				cb_SpecularLightColor	: packoffset(c5);

	float				cb_is_dirty : packoffset(c6.x);
	float				cb_SpecularIntensity : packoffset(c6.y);
	float2				cb_notchange_padding			: packoffset(c6.z);
}


struct PNCT2_VS_INPUT
{
	float4 p		: POSITION;
	float3 n		: NORMAL;
	float4 c		: COLOR0;
	float2 t		: TEXCOORD0;
	float3 Tan		: TANGENT;
};

struct PCT4_PS_INPUT
{
	float4 p			: SV_POSITION;
	float4 c			: COLOR0;
	float2 t			: TEXCOORD0;
	float3 vEye			: TEXCOORD1;
	float3 vHalf		: TEXCOORD2;
	float3 vLightDir	: TEXCOORD3;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PCT4_PS_INPUT VS(PNCT2_VS_INPUT input)
{
	PCT4_PS_INPUT output = (PCT4_PS_INPUT)0;
	float4 vWorldPos = mul(input.p, g_matWorld);
	float4 vViewPos = mul(vWorldPos, g_matView);
	output.p = mul(vViewPos, g_matProj);

	output.t = input.t * 50.0f;


	float3 vNormal = normalize(mul(input.n, (float3x3)cb_mat_normal));
	output.vEye = normalize(cb_vec_eye_dir);
	output.c = saturate(dot(input.n, -cb_vec_light));

	float3 T = normalize(mul(input.Tan, (float3x3)cb_mat_normal));
	float3 B = normalize(cross(vNormal, T));

	float3x3 tanMat = { T.x, B.x, vNormal.x,
		T.y, B.y, vNormal.y,
		T.z, B.z, vNormal.z };
	float3 vLightDir = cb_vec_light.xyz;// normalize(cb_vec_light.xyz - vWorldPos);
	output.vHalf = normalize(mul(normalize(vLightDir + output.vEye), tanMat));
	output.vLightDir = normalize(mul(-vLightDir, tanMat));
	output.vEye = normalize(mul(output.vEye, tanMat));
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PCT4_PS_INPUT input) : SV_Target
{

	float4 DiffuseColor = g_txDiffuse.Sample(g_samLinear, input.t);

	// 디퓨즈 조명 
	float4 normal = g_txNormalMap.Sample(g_samLinear, input.t);
	normal = normalize((normal - 0.5f) * 2.0f);
	float  fDot = saturate(dot(normal.xyz, input.vLightDir));
	float3 LightColor = cb_DiffuseMaterial.rgb * cb_DiffuseLightColor.rgb * fDot;

	// 스펙큘러 조명 		
	float3 R = reflect(-input.vLightDir, normal.xyz);
	float3 SpecColor = cb_SpecularMaterial.rgb * cb_SpecularLightColor.rgb * pow(saturate(dot(R, input.vEye)), cb_SpecularIntensity);
	//float3 SpecColor	=	cb_SpecularLightColor.rgb * pow( saturate(dot( input.vHalf, normal.xyz )), cb_SpecularPower );

	// 전체 컬러 조합  	
	float4 vFinalColor = DiffuseColor * float4(LightColor + SpecColor + cb_AmbientLightColor * cb_AmbientMaterial, 1.0f);
	//float4 vFinalColor = DiffuseColor;
	return vFinalColor;// *input.c;
}
float4 DEFAULT_PS(PCT4_PS_INPUT input) : SV_Target
{
	return g_txDiffuse.Sample(g_samLinear, input.t);
}
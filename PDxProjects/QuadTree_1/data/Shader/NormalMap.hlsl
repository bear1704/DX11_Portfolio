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
cbuffer cbObjectChangesEveryFrame: register(b1)
{
	// Global
	matrix				g_matNormal;
	float3				cb_vLightVector;
	float3				cb_vEyePos;
	float3				cb_vEyeDir;
};

cbuffer cbObjectNeverChanges: register(b2)
{
	// Global
	float4				cb_AmbientLightColor: packoffset(c0);
	float4				cb_DiffuseLightColor: packoffset(c1);
	float3				cb_SpecularLightColor: packoffset(c2);
	float				cb_SpecularPower : packoffset(c2.w);
};

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


	float3 vNormal = normalize(mul(input.n, (float3x3)g_matNormal));
	output.vEye = normalize(cb_vEyeDir);
	output.c = saturate(dot(input.n, -cb_vLightVector));

	float3 T = normalize(mul(input.Tan, (float3x3)g_matNormal));
	float3 B = normalize(cross(vNormal, T));

	float3x3 tanMat = { T.x, B.x, vNormal.x,
		T.y, B.y, vNormal.y,
		T.z, B.z, vNormal.z };
	float3 vLightDir = cb_vLightVector.xyz;// normalize(cb_vLightVector.xyz - vWorldPos);
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
	float3 LightColor = cb_DiffuseLightColor.rgb * fDot;

	// 스펙큘러 조명 		
	float3 R = reflect(-input.vLightDir, normal.xyz);
	float3 SpecColor = cb_SpecularLightColor.rgb * pow(saturate(dot(R, input.vEye)), cb_SpecularPower);
	//float3 SpecColor	=	cb_SpecularLightColor.rgb * pow( saturate(dot( input.vHalf, normal.xyz )), cb_SpecularPower );

	// 전체 컬러 조합  	
	float4 vFinalColor = DiffuseColor * float4(LightColor + SpecColor + cb_AmbientLightColor, 1.0f);
	//float4 vFinalColor = DiffuseColor;
	return vFinalColor;// *input.c;
}
float4 DEFAULT_PS(PCT4_PS_INPUT input) : SV_Target
{
	return g_txDiffuse.Sample(g_samLinear, input.t);
}
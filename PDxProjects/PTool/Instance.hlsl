Texture2D g_tex_diffuse : register(t0);
SamplerState s0 : register(s0);

cbuffer cb0 : register(b0)
{
	matrix matWorld : packoffset(c0);
	matrix matView : packoffset(c4);
	matrix matProj : packoffset(c8);
	float4 color : packoffset(c12);
	float etc : packoffset(c13.x);
};


struct VS_INPUT
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXCOORD;
	matrix m : InstanceWorld;
	float4 pc : POSCOLOR;
};

struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	float4 pc : POSCOLOR;
};


VS_OUTPUT VS_ALPHA(VS_INPUT input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	float4 vWorld = mul(float4(input.p, 1.0f), input.m);
	Output.p = mul(vWorld, matWorld);
	Output.p = mul(Output.p, matView);
	Output.p = mul(Output.p, matProj);
	Output.t = input.t;
	Output.n = input.n;
	Output.c = input.c;
	Output.c.w = color.w;
	Output.pc = input.pc;

	return Output;
}

float4 PS_ALPHA(VS_OUTPUT input) : SV_TARGET
{
	float4 color = g_tex_diffuse.Sample(s0, input.t) * input.pc.w;
	//float4 color = g_tex_diffuse.Sample(s0, input.t) * 0.4f;
	return color;
}


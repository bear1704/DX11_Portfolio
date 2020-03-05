Texture2D g_tex_diffuse : register(t0);
SamplerState s0 : register(s0);

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 Diffuse  : COLOR0;
};

float4 PS(in float4 p : POSITION,
	in float2 t : TEXCOORD0) : SV_TARGET
{
	return g_tex_diffuse.Sample(s0, t);
}
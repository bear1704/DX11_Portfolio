Texture2D g_tex_diffuse : register(t0);
SamplerState s0 : register(s0);

struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	//return g_tex_diffuse.Sample(s0, input.t);
	float4 color = g_tex_diffuse.Sample(s0, input.t);
	return color;

}


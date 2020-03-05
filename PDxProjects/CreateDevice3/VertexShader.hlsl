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

};


struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	Output.p = mul(float4(input.p, 1.0f), matWorld);
	Output.p = mul(Output.p, matView);
	Output.p = mul(Output.p, matProj);
	//Output.p = float4(input.p, 1.0f);
	Output.t = input.t;
	Output.n = input.n;
	Output.c = input.c;

	return Output;
}


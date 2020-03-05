struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 t  : TEXCOORD0;
};

struct VS_INPUT
{
	float3 p : POSITION1;
	float2 t : TEXCOORD2;

};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT Output;
	//float3 rot_pos = { input.p.x * cos(theta) - input.p.y * sin(theta) , input.p.x * sin(theta) + input.p.y * cos(theta) , input.p.z};
	//float3 vertex_pos = rot_pos* time;
	
	//Output.Position = float4(vertex_pos, 1.0f);
	//Output.Position = rot_pos;

	Output.Position = float4(input.p, 1.0f);
	Output.t = input.t;


	return Output;
}
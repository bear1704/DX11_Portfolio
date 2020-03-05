#define MAX_BONE_MATRIX 255
Texture2D g_tex_diffuse : register(t0);
SamplerState s0 : register(s0);


struct VS_INPUT
{
	float3 pos :		 POSITION;
	float3 normal : 	 NORMAL;
	float4 color :		 COLOR0;
	float2 tex :		 TEXCOORD0;
	float4 idx1 :		 TEXCOORD1;
	float4 weight1 :	 TEXCOORD2;
	float4 idx2 :		 TEXCOORD3;
	float4 weight2 :	 TEXCOORD4;
};


struct VS_OUTPUT
{
	float4 pos :	SV_POSITION;
	float3 normal : NORMAL;
	float4 color :	 COLOR0;
	float2 tex	:	 TEXCOORD0;
	float4 r	:	 TEXCOORD1;

};

cbuffer cb0 : register(b0)
{
	matrix g_mat_world : packoffset(c0);
	matrix g_mat_view : packoffset(c4);
	matrix g_mat_proj : packoffset(c8);
	float4 g_color :	packoffset(c12);
	float g_time : packoffset(c13.x);
};

cbuffer cb1 : register(b1)
{
	matrix g_mat_anim[MAX_BONE_MATRIX];
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT pOut = (VS_OUTPUT)0;
	float4x4 mat_anim;

	float3 vec_normal = input.normal;
	float4 vtx_local = float4(input.pos, 1.0f);
	float4 vtx_anim;

	for (int biped = 0; biped < 8; biped++)
	{
		if (biped < 4) //index 4, weight 4±îÁö
		{
			uint idx_bone = input.idx1[biped];
			float weight = input.weight1[biped];
			mat_anim = g_mat_anim[idx_bone];
			vtx_anim += weight * mul(vtx_local, mat_anim);
			//pOut.normal += weight * mul(input.normal, (float3x3)mat_anim); 
			pOut.normal += weight * mul(input.normal, mat_anim); 
		}
		else
		{
			uint idx_bone = input.idx2[biped - 4];
			float weight = input.weight2[biped - 4];
			mat_anim = g_mat_anim[idx_bone];
			vtx_anim += weight * mul(vtx_local, mat_anim);
			//pOut.normal += weight * mul(input.normal, (float3x3)mat_anim);
			pOut.normal += weight * mul(input.normal, mat_anim);
		}
	}

	pOut.pos = mul(vtx_anim, g_mat_world);
	pOut.pos = mul(pOut.pos, g_mat_view);
	pOut.pos = mul(pOut.pos, g_mat_proj);
	pOut.tex = input.tex;
	pOut.normal = input.normal;

	float3 vec_light = float3(0.0f, -1.0f, 0.0f) * -1.0f;
	float fdot = max(0.0f, dot(vec_light, pOut.normal));
	float4 vec_color = float4(fdot, fdot, fdot, 1.0f);
	pOut.r = vec_color;
	pOut.color = input.color;


	return pOut;

}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float3 vec_light = float3(0.0f, 0.0f, 1.0f) * -1.0f;
	float fdot = max(0.33f, dot(vec_light, input.normal));
	float4 vec_color = float4(fdot, fdot, fdot, 1.0f);
	float4 color = vec_color * g_tex_diffuse.Sample(s0, input.tex);

	return color * float4(input.color.xyz, 1.0f);
}

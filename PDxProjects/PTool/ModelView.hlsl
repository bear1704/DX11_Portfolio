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

cbuffer cbChangesEveryFrame : register(b2)
{
	float3				cb_vec_light		:packoffset(c4);
	float				cb_padding1 : packoffset(c4.w);
	float3				cb_vec_camera_pos	:packoffset(c5);
	float				cb_padding2 : packoffset(c5.w);
	float3				cb_vec_eye_dir		:packoffset(c6);
	float				cb_padding3 : packoffset(c6.w);
};

cbuffer cbNearlyNotChange: register(b3)
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
	//float3 vec_light = float3(0.0f, 0.0f, 1.0f) * -1.0f;
	//float fdot = max(0.33f, dot(vec_light, input.normal));
	//float4 vec_color = float4(fdot, fdot, fdot, 1.0f);
	//float4 color = vec_color * g_tex_diffuse.Sample(s0, input.tex);

	//return color * float4(input.color.xyz, 1.0f);

	float4 tex_color = g_tex_diffuse.Sample(s0, input.tex);
	float4 final_color = tex_color * (Diffuse(input.normal) + Specular(input.normal)) * input.color;
	final_color.a = 1.0f;
	return final_color;

}


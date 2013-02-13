/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////

struct vertexInput
{ 
	float4				iPosition			: POSITION;
	float2				iTexCoord			: TEXCOORD0;	
};

struct vertexOutput 
{ 
	float4				oPosition			: POSITION;
	float2				oTexCoord			: TEXCOORD0;
};

vertexOutput main_vp(	vertexInput			IN,
						uniform float4x4	worldViewProj
					 )
{
	vertexOutput OUT;

	OUT.oPosition = mul(worldViewProj, IN.iPosition);
	OUT.oTexCoord = IN.iTexCoord;
	return OUT;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////

float4 main_fp(	vertexOutput		IN,
				//uniform float4		screenLightPos,
				uniform float3		screenLightPos,
				uniform float		weight,
				uniform float		decay,
				uniform float		exposure,
				uniform float		density,
				uniform sampler2D	frameLightTex				: register(s0)
			  )	:	COLOR
{
	//float NUM_SAMPLES = 64;
	//float2 texCoord = IN.oTexCoord;
	//half2 deltaTexCoord = (texCoord - screenLightPos.xy);
	//deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;
	//half3 color = tex2D(frameLightTex, IN.oTexCoord);
	//half illuminationDecay = 1.0f;
	//for( int i = 0; i < NUM_SAMPLES; i++ )
	//{
	//	texCoord -= deltaTexCoord;
	//	half3 sample = tex2D(frameLightTex, texCoord);
	//	sample *= illuminationDecay * weight;
	//	color += sample;
	//	illuminationDecay *= decay;
	//}
	//return float4(color * exposure, 1);

	float NUM_SAMPLES = 64;
	float2 texCoord = IN.oTexCoord;
	float2 deltaTexCoord = (texCoord - screenLightPos.xy) / NUM_SAMPLES;	//screen direction from pixel to light 
	deltaTexCoord *= density;

//	if( screenLightPos.z < 0.0 )
//		deltaTexCoord = -deltaTexCoord;										//to handle light in the back
	
	float3 color = tex2D(frameLightTex, IN.oTexCoord.xy).rgb;				//first sample
	float illuminationDecay = 1.0;
	for( float i = 0.0; i < NUM_SAMPLES; i++ )								//the main loop
	{
		texCoord -= deltaTexCoord;
		float3 sample = tex2D(frameLightTex, texCoord.xy).rgb;
		sample *= illuminationDecay * weight;
		color += sample;
		illuminationDecay *= decay;
	}
	return float4(color * exposure, 1.0);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////
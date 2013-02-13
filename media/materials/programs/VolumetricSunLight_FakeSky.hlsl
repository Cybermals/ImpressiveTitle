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
	float3				oCamToPixel			: TEXCOORD1;
};

vertexOutput main_vp(	vertexInput			IN,
						uniform float4x4	worldViewProj,
						uniform float3		camPos
					 )
{
	vertexOutput OUT;
	OUT.oPosition = mul(worldViewProj, IN.iPosition);
	OUT.oTexCoord = IN.iTexCoord;
	OUT.oCamToPixel = IN.iPosition.xyz - camPos.xyz;
	return OUT;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////
//simple linear interpolation
float3 lerp(float3 v1, float4 v2, float i)
{
	return v1 + (v2-v1) * clamp(i, 0.0, 1.0);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////

float4 main_fp(	vertexOutput		IN,
				uniform float3		lightPos,
				uniform float3		camPos,
				uniform float3		sunCenter,
				uniform float3		sunPeriph,
				uniform float3		sunEarthBottom,
				uniform float3		sunEarthTop
			  )	:	COLOR
{
	float3 ctpn = normalize(IN.oCamToPixel);

	//direct sun color
	float directSun = dot(ctpn, normalize(lightPos - camPos));
	directSun = pow( clamp(directSun, 0.0, 1.0), 5.0);

	// atmosphere
	float sunEarthScatering = 1.0 - dot(ctpn, float3(0.0, 1.0, 0.0));
	sunEarthScatering = pow( clamp(sunEarthScatering, 0.0, 1.0), 5.0);

	//lerp color and add contribution of sun and atmosphere
	float3 directSunContrib = lerp(sunPeriph, sunCenter, directSun) * directSun;
	float3 sunEarthContrib = lerp(sunEarthTop, sunEarthBottom, sunEarthScatering);
	
	float4 OutPutColor = 0.1f * float4(directSunContrib + sunEarthContrib, 1.0);
	return OutPutColor;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////
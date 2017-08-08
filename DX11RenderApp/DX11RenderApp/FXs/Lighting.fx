
/* Defination of Lighting source and computation*/



struct Material
{
	float4 ambient; // reflection rate
	float4 diffuse;
	float4 specular;
};

struct DirectionalLight
{
	float4 ambient; // light color
	float4 diffuse;
	float4 specular;
	float3 direction;
	float  pad;
};

void ComputeDirectionalLight(DirectionalLight Ld, Material mat, float3 nor, float3 toEye, 
	out float4 ambient, out float4 diffuse, out float4 specular)
{
	// Ma x La
	ambient = Ld.ambient * mat.ambient;

	diffuse  = float4 (0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4 (0.0f, 0.0f, 0.0f, 0.0f);

	// kd * (Md x ld)
	float kd = max(dot(-Ld.direction, nor), 0);

	[flatten]
	if (kd != 0)
	{
		diffuse = kd * Ld.diffuse * mat.diffuse;

		float3 r = reflect(Ld.direction, nor);
		float ks = pow (max(dot(r, toEye), 0), mat.specular.w);

		specular = ks * Ld.specular * mat.specular;
	}

}

struct PointLight
{
	float4 ambient; // light color
	float4 diffuse;
	float4 specular;

	float3 position;
	float  range;

	float3 attenuation;
	float  pad;
};

void ComputePointLight(PointLight Lp, Material mat,
	float3 nor, float3 toEye, float3 pos,
	out float4 ambient, out float4 diffuse, out float4 specular)
{
	// calculate ambient
	ambient = Lp.ambient * mat.ambient;

	diffuse = float4(0, 0, 0, 0);
	specular = float4(0, 0, 0, 0);

	// calculate light direction
	float3 toLight = Lp.position - pos;
	float distance = length(toLight);

	// distance check
	[flatten]
	if (distance < Lp.range)
	{
		toLight /= distance;

		// calculate diffuse factor
		float kd = max(dot(toLight, nor), 0);

		// no diffuse, no specular
		[flatten]
		if (kd != 0)
		{
			float att = 1.0f / dot(Lp.attenuation, float3(1, distance, distance*distance));
			diffuse = att * kd * Lp.diffuse * mat.diffuse;

			float3 r = reflect(-toLight, nor);
			float ks = pow(max(dot(r, toEye), 0), mat.specular.w);
			specular = att * ks * Lp.specular * mat.specular;
		}
	}
}

struct SpotLight 
{
	float4 ambient;
	float4 diffuse;
	float4 specular;

	float3 position;
	float  range;

	float3 direction;
	float  spot;

	float3 attenuation;
	float pad;
};

void ComputeSpotLight(SpotLight Ls, Material mat,
	float3 nor, float3 toEye, float3 pos,
	out float4 ambient, out float4 diffuse, out float4 specular)
{
	// calculate spot factor
	ambient = float4(0, 0, 0, 0);
	diffuse = float4(0, 0, 0, 0);
	specular = float4(0, 0, 0, 0);

	float3 toLight = Ls.position - pos;
	float distance = length(toLight);
	toLight /= distance;

	float k_spot = max(dot(-toLight, Ls.direction), 0);

	[flatten]
	if (k_spot != 0)
	{
		k_spot = 1.0f;//pow(k_spot, Ls.spot);

		// calucalte ambient
		ambient = k_spot * Ls.ambient * mat.ambient;

		[flatten]
		if (distance < Ls.range)
		{
			float kd = max(dot(toLight, nor), 0);

			[flatten]
			if (kd != 0)
			{
				//float att = 1.0f / dot(Ls.attenuation, float3(1, distance, distance * distance));
				//diffuse = att * kd * Ls.diffuse * mat.diffuse;
				diffuse =  kd * Ls.diffuse * mat.diffuse;
				float3 r = reflect(-toLight, nor);
				float ks = pow(max(dot(r, toEye), 0), Ls.specular.w);
				//specular = att * ks * Ls.specular * mat.specular;
				specular = ks * Ls.specular * mat.specular;

			}

		}
	}
}



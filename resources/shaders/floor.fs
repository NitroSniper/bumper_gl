#version 460

in vec3 mdl_pos;
in vec2 texCoords;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D specularTex;
uniform vec4 lightColor;
uniform vec3 camPos;

uniform float scale;



out vec4 color;

void main()
{
	vec2 texCoord = texCoords * scale;
	vec3 lightPos = vec3(0.0f, 5.0f, 0.0f);
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - mdl_pos;

	// get normal from texture map
	vec3 normal = texture(normalTex, texCoord).rgb;
	

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	normal = normalize(normal * 2.0 - 1.0);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - mdl_pos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	color = (texture(diffuseTex, texCoord) * (diffuse * inten + ambient) + texture(specularTex, texCoord).r * specular * inten) * lightColor;
}
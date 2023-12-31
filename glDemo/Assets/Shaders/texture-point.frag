#version 410

// Diffuse texture - point light

// Texture sampler (for diffuse surface colour)
uniform sampler2D texture;

// Point light model
uniform vec3 lightPosition;
uniform vec3 lightColour;
uniform vec3 lightAttenuation; // x=constant, y=linear, z=quadratic


in SimplePacket {
	
	vec3 surfaceWorldPos;
	vec3 surfaceNormal;
	vec2 texCoord;

} inputFragment;


layout (location=0) out vec4 fragColour;

void main(void) {

	vec3 surfaceToLightVec = lightPosition - inputFragment.surfaceWorldPos;

	// calculate lambertian
	vec3 surfaceToLightNormalised = normalize(surfaceToLightVec);
	vec3 N = normalize(inputFragment.surfaceNormal);
	float l = dot(N, surfaceToLightNormalised);

	// calculate attenuation
	float d = length(surfaceToLightVec);

	float kc = lightAttenuation.x;
	float kl = lightAttenuation.y;
	float kq = lightAttenuation.z;

	float a = 1.0 / ( kc + (kl * d) + (kq * d * d) );

	// Calculate diffuse brightness / colour for fragment
	vec4 surfaceColour = texture2D(texture, inputFragment.texCoord);

	vec3 diffuseColour = surfaceColour.rgb * lightColour * l * a;

	fragColour = vec4(diffuseColour, 1.0);
	//fragColour = vec4(vec3(l, l, l), 1.0);
}

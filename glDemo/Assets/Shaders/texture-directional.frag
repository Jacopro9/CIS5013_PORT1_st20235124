#version 410

// Diffuse texture - directional light

// Texture sampler (for diffuse surface colour)
uniform sampler2D texture;

// Directional light model
uniform vec3 lightDirection;
uniform vec3 lightColour;


in SimplePacket {
	
	vec3 surfaceWorldPos;
	vec3 surfaceNormal;
	vec2 texCoord;

} inputFragment;


layout (location=0) out vec4 fragColour;

void main(void) {

	// calculate lambertian (l)
	vec3 N = normalize(inputFragment.surfaceNormal);
	float l = dot(N, lightDirection);

	// Calculate diffuse brightness / colour for fragment
	vec4 surfaceColour = texture2D(texture, inputFragment.texCoord);
	vec3 diffuseColour = surfaceColour.rgb * lightColour * l;

	fragColour = vec4(diffuseColour, 1.0);
	//fragColour = vec4(vec3(l, l, l), 1.0);
}

#version 410

// Diffuse texture - directional light

// Texture sampler (for diffuse surface colour)
uniform sampler2D diffuseTexture; // tex unit 0

// Texture sampler for normal map texture
uniform sampler2D normalMapTexture; // tex unit 1


// Directional light model - colour component only needed in the
// fragment shader - we used the lightDirection vector in the vertex
// shader and pickup the per-vectex light direction in the input
// packet below
uniform vec3 lightColour;


// Fragment input packet - contains interpolated direction to light
// vector calculated in the vertex shader.
in SimplePacket {
	
	vec3 surfaceWorldPos;
	vec3 tsLightDirection;
	vec2 texCoord;

} inputFragment;


layout (location=0) out vec4 fragColour;


void main(void) {

	// Get normal from normal map (RGB)
	vec3 N = texture2D(normalMapTexture, inputFragment.texCoord).xyz;
	
	// Map the RGB values back to the [-1, +1] coordinate range
	//N = vec3(2.0) * (N - vec3(0.5));
	N = (N - 0.5) * 2.0;

	// Ensure the normal is unit length (has length of 1)
	N = normalize(N);
	
	// Normalise light direction in tangent space
	vec3 L = normalize(inputFragment.tsLightDirection);
	
	// calculate lambertian (l)
	float l = dot(N, L);


	// Calculate diffuse brightness / colour for fragment
	vec4 surfaceColour = texture2D(diffuseTexture, inputFragment.texCoord);
	vec3 diffuseColour = surfaceColour.rgb * lightColour * l;


	// Set final output colour
	fragColour = vec4(diffuseColour, 1.0);
}

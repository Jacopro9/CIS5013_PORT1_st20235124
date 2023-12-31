#version 410

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

// Directional light model (dont't need colour vector in vertex shader)
// It's okay to split the relevant variables between the shaders that need them!
uniform vec3 lightDirection;

// Incomping vertex packet - now including tangent and bitanget in slots 4 and 5
layout (location=0) in vec3 vertexPos;
layout (location=2) in vec3 vertexTexCoord;
layout (location=3) in vec3 vertexNormal;
layout (location=4) in vec3 tangent;
layout (location=5) in vec3 bitangent;

// Output packet to pass onto the rasteriser / fragment shader.
// We don't output the normal here (this gets accessed in the normal map)
// in the fragment shader.  Instead we pass on the direction-to-light
// vector.  This is calculated here, in the vertex shader, as it avoids
// the  need to pass the entire (vertexNormal, tangent, bitangent) basis
// vector set onto the fragment shader.
out SimplePacket {

    vec3 surfaceWorldPos;
    vec3 tsLightDirection; // normals will come from the normal map - we interpolate light direction vec in surface tangent space
    vec2 texCoord;

} outputVertex;


void main(void) {

	outputVertex.texCoord = vertexTexCoord.st;

    // Calculte the inverse-transpose of the model matrix - this is
    // used to transform the normal and tangent vectors correctly!
    mat4 normalMatrix = transpose(inverse(modelMatrix));

    // Transform the normal and tangent vectors to correct orientation
    // to match the host object's orientation in world coordinates.
    vec3 n = (normalMatrix * vec4(vertexNormal, 0.0)).xyz;
    vec3 t = (normalMatrix * vec4(tangent, 0.0)).xyz;
    vec3 b = (normalMatrix * vec4(bitangent, 0.0)).xyz;

    // We know the direction to light vector from the 'lightDirection'
    // uniform.  We map this into the tangent space defined by the basis
    // vectors (vertexNormal, tangent, bitangent) 
    vec3 tVec;
	tVec.x = dot(lightDirection, t);
	tVec.y = dot(lightDirection, b);
	tVec.z = dot(lightDirection, n);
	outputVertex.tsLightDirection = normalize(tVec);

    // take vertexPos into world coords and pass onto fragment shader
    vec4 worldCoord = modelMatrix * vec4(vertexPos, 1.0);
    outputVertex.surfaceWorldPos = worldCoord.xyz; // don't need w element

    // take worldCoord rest of the way into clip coords and set in gl_Position
	gl_Position = projMatrix * viewMatrix * worldCoord;
}

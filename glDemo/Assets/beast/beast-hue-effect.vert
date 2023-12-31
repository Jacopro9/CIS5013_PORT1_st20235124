#version 410

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


layout (location=0) in vec3 vertexPos;
layout (location=2) in vec3 vertexTexCoord;


out SimplePacket {

    vec4 worldPos;
	vec2 texCoord;

} outputVertex;


void main(void) {

    outputVertex.worldPos = modelMatrix * vec4(vertexPos, 1.0);
	outputVertex.texCoord = vertexTexCoord.st;
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPos, 1.0);
}
#version 410

uniform mat4 transformMat;

layout (location=0) in vec3 vertexPos;
layout (location=2) in vec3 vertexTexCoord;


out SimplePacket {

	vec2 texCoord;

} outputVertex;


void main(void) {

	outputVertex.texCoord = vertexTexCoord.st;
	gl_Position = transformMat * vec4(vertexPos, 1.0);
}

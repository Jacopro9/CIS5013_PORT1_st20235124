#version 410

uniform mat4 mvpMatrix;


layout (location=0) in vec3 vertexPos;
layout (location=2) in vec3 vertexTexCoord;


out SimplePacket {

	vec2 texCoord;

} outputVertex;


void main(void) {

	outputVertex.texCoord = vertexTexCoord.st;
	
	gl_Position = mvpMatrix * vec4(vertexPos, 1.0);
}

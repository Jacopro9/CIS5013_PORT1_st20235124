#version 410

uniform mat4 mvpMatrix;

layout (location=0) in vec3 vertexPos;
//layout (location=1) in vec4 vertexColour;

//out SimplePacket {

//	vec4 colour;

//} outputVertex;


void main(void) {

	//outputVertex.colour = vertexColour;
	gl_Position = mvpMatrix * vec4(vertexPos, 1.0);
}

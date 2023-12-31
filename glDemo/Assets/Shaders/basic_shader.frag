#version 410

//in SimplePacket {

//	vec4 colour;

//} inputFragment;

layout (location=0) out vec4 fragColour;

void main(void) {

	//fragColour = inputFragment.colour;
	fragColour = vec4(0.7, 0.7, 0.7, 1.0);
}

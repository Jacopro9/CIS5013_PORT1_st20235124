#version 410

uniform sampler2D wave1Texture;
uniform sampler2D wave2Texture;

uniform float wave1Phase;
uniform float wave2Phase;


in SimplePacket {

	vec2 texCoord;

} inputFragment;


//uniform float randomValue;
//uniform vec3 centrePoint;

layout (location=0) out vec4 fragColour;


// rgb2hsv and hsv2rgb colour space conversion - from http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 rgb2hsv(vec3 c) {

    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c) {

    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main(void) {
	
    vec4 wave1 = texture2D(wave1Texture, inputFragment.texCoord+vec2(wave1Phase, 0.0));
    vec4 wave2 = texture2D(wave2Texture, inputFragment.texCoord+vec2(wave2Phase, 0.0));

    // lerp between the 2 using wave2 alpha
    vec3 wave = wave2.rgb * wave2.a + wave1.rgb * 0.5 * (1.0 - wave2.a);
    vec3 hsv = rgb2hsv(wave);
   /// hsv.x = 0.49;
    //hsv.y = 1.0;
    //hsv.z = 0.3;
    wave = hsv2rgb(hsv);

	float yn = mod(gl_FragCoord.y / 2.0, 3);

	fragColour = vec4(wave * yn, max(0.3, wave2.a));
}


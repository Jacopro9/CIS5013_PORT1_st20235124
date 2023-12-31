#version 410

uniform sampler2D texture;
uniform vec3 cylinderPos;


in SimplePacket {

   vec4 worldPos;
	vec2 texCoord;

} inputFragment;


layout (location=0) out vec4 fragColour;


// -------------------


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

// -------------------


void main(void) {

    float dist = distance(cylinderPos.xz, inputFragment.worldPos.xz);

    vec3 rgb = texture2D(texture, inputFragment.texCoord).rgb;
   
    vec3 hsv = rgb2hsv(rgb); // original texture colour
    vec3 hsvRed = vec3(0.0, hsv.yz); // copy hsv but set hue to 0 (red)

    // calculate blend parameter t that lies between 0 (hsv) and 1 (hsvRed)
    float t = step(dist, 1.25);
    //float t = smoothstep(1.5, 1.25, dist);

    // now lerp between hsv and hsvRed using the blend parameter t
    vec3 hsvFinal = mix(hsv, hsvRed, t);

    rgb = hsv2rgb(hsvFinal);

    fragColour = vec4(rgb, 1.0);
}

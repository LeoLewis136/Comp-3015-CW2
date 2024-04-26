#version 460

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;

in vec3 ViewPos; // The position of the camera

layout (binding = 0) uniform sampler2D albedoTexture; // GL_TEXTURE0 is object "albedo" texture
//layout (binding = 1) uniform sampler2D normalTexture; // GL_TEXTURE1 is object normal texture
//layout (binding = 2) uniform sampler2D albedoTexture2; // GL_TEXTURE2 is for texture mixing
layout (binding = 5) uniform sampler2D RenderTex; // Multi pass render texture
layout (binding = 6) uniform sampler2D LightTex; // Texture to store which pixels should have bloom

uniform float Weights[5];
uniform int Pass; // The current rendering pass 
uniform float BloomThreshold;
uniform float BloomIntensity;

layout (location = 0) out vec4 FragColor;

bool getBrightArea(){
    vec4 colour = texture(RenderTex, TexCoord);
    float luminence = (colour.r + colour.g + colour.b) / 3.0;

    return luminence > BloomThreshold;

}

vec4 pass1(){
    
    return texture(albedoTexture, TexCoord); 

}

// Calculates the areas that require bloom
vec4 pass2(){
    if (getBrightArea()){
        return vec4(1.0f);
    }

    return vec4(0.0f);
}

// Apply Gaussian blur to Light tex
vec4 pass3(){
    ivec2 pix = ivec2(gl_FragCoord.xy);

    vec4 sum = texelFetch(LightTex, pix, 0) * Weights[0];
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(0,1)), Weights[1]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(0,-1)), Weights[1]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(0,2)), Weights[2]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(0,-2)), Weights[2]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(0,3)), Weights[3]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(0,-3)), Weights[3]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(0,4)), Weights[4]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(0,-4)), Weights[4]);

    return sum;
}

vec4 pass4(){
    ivec2 pix = ivec2(gl_FragCoord.xy);

    vec4 sum = texelFetch(LightTex, pix, 0) * Weights[0];
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(1,0)), Weights[1]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(-1,0)), Weights[1]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(2,0)), Weights[2]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(-2,0)), Weights[2]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(3,0)), Weights[3]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(-3,0)), Weights[3]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(4,0)), Weights[4]);
    sum = mix(sum, texelFetchOffset(LightTex, pix, 0, ivec2(-4,0)), Weights[4]);

    return sum;
}

vec4 pass5(){
    vec4 sceneColor = texture(RenderTex, TexCoord);
    vec4 bloomColour = texture(LightTex, TexCoord);

    return sceneColor + (bloomColour * BloomIntensity);
}



void main() {
    if (Pass == 1) { FragColor = pass1(); }
    else if (Pass == 2) { FragColor = pass2(); }
    else if (Pass == 3) { FragColor = pass3(); }
    else if (Pass == 4) { FragColor = pass4(); }
    else if (Pass == 5) { FragColor = pass5(); }
}

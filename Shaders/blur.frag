#version 120

uniform sampler2D texture;
uniform float offset;

void main()
{
    vec2 texCoords = gl_TexCoord[0].xy;
    vec4 color = vec4(0.0);
    
    // Sample the neighboring pixels
    color += texture2D(texture, texCoords + vec2(-offset, -offset)) * 0.05;
    color += texture2D(texture, texCoords + vec2(0.0, -offset)) * 0.09;
    color += texture2D(texture, texCoords + vec2(offset, -offset)) * 0.05;
    
    color += texture2D(texture, texCoords + vec2(-offset, 0.0)) * 0.09;
    color += texture2D(texture, texCoords + vec2(0.0, 0.0)) * 0.36;
    color += texture2D(texture, texCoords + vec2(offset, 0.0)) * 0.09;
    
    color += texture2D(texture, texCoords + vec2(-offset, offset)) * 0.05;
    color += texture2D(texture, texCoords + vec2(0.0, offset)) * 0.09;
    color += texture2D(texture, texCoords + vec2(offset, offset)) * 0.05;

    gl_FragColor = color;
}

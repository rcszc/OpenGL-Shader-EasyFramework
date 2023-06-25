#version 460 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec2 vertexTexture;

uniform mat4 MatrixMVP;

uniform float Time;

out vec4 verBlendColor; 
out vec2 verTextureCoord;
 
void main()
{
    float angle = Time * -0.32;

    // ¼ÆËãÐý×ª¾ØÕó
    mat4 rotationMatrix = mat4(
        vec4(cos(angle), -sin(angle), 0.0, 0.0),
        vec4(sin(angle), cos(angle), 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    gl_Position = rotationMatrix * MatrixMVP * vec4(vertexPos, 1);
    
    verBlendColor = vertexColor;
    verTextureCoord = vertexTexture;
}
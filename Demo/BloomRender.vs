#version 460 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec2 vertexTexture;

uniform mat4 MatrixMVP;

out vec4 verBlendColor; 
out vec2 verTextureCoord;
 
void main()
{
    gl_Position = MatrixMVP * vec4(vertexPos, 1);
    
    verBlendColor = vertexColor;
    verTextureCoord = vertexTexture;
}
#version 460 core

in vec4 verBlendColor;
in vec2 verTextureCoord;

uniform sampler2D TextureA;

uniform float Time;

out vec4 FragColor;
 
void main()
{
    FragColor = texture(TextureA, verTextureCoord);
}
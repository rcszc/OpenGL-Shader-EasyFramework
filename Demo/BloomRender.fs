#version 460 core

in vec4 verBlendColor;
in vec2 verTextureCoord;

uniform sampler2D TextureWinA; // SourceScene.
uniform sampler2D TextureWinB; // BlurScene.

uniform vec2 screenSize; // WindowSize.

uniform float BloomBlur;
uniform float BloomSource;
uniform float TestFilter;

out vec4 FragColor;

const int blurRadius = 15;

void main()
{ 
    vec2 texelSize = 1.0 / screenSize;
    vec2 offset = vec2(1.0, 1.0) * texelSize * 2.5; // StepLen.
    vec4 blurColor = vec4(0.0);

    // ConvBlur.
    for (float i = -blurRadius; i <= blurRadius; i++) 
    {
        for (float j = -blurRadius; j <= blurRadius; j++)
        {
           vec2 blurOffset = vec2(i, j) * offset;
           blurColor += texture(TextureWinB, verTextureCoord + blurOffset);
        }
    }
    blurColor /= pow((blurRadius * 2.0 + 1.0), 2.0);

    vec4 AtexBlendColor = texture(TextureWinA, verTextureCoord);
    FragColor = blurColor * BloomBlur + AtexBlendColor * BloomSource;
}
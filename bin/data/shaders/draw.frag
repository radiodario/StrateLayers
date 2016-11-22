#version 330

in vec2 Vertex_UV;
in vec3 Colour_comp;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform float particleAlpha;

void main()
{
  vec4 spriteTex = texture(uTexture, Vertex_UV.xy);
  fragColor = spriteTex * vec4(Colour_comp, particleAlpha);
}

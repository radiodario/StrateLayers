#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 texCoordFromVertex[];
in float vertColor[];

uniform mat4 projectionMatrix;
uniform float imgWidth, imgHeight;
uniform float size;
uniform float lightness;
out vec2 Vertex_UV;
out vec3 Colour_comp;

float rand(float n){return fract(sin(n) * 43758.5453123);}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
  vec4 p1, p2, p3, p4;
  vec2 t1, t2, t3, t4;


  vec4 pos_in = gl_in[0].gl_Position;

  p1 = pos_in + vec4(-size, -size, 0.0, 0.0) * projectionMatrix;
  p2 = pos_in + vec4(size, -size, 0.0, 0.0) * projectionMatrix;
  p3 = pos_in + vec4(-size, size, 0.0, 0.0) * projectionMatrix;
  p4 = pos_in + vec4(size, size, 0.0, 0.0) * projectionMatrix;

  t1 = vec2(0, 0);
  t2 = vec2(imgWidth, 0);
  t3 = vec2(imgWidth, imgHeight);
  t4 = vec2(0, imgHeight);

  Colour_comp = hsv2rgb(vec3(
      1.0 - vertColor[0],
      vertColor[0],
      lightness
      ));

  gl_Position = p1;
  Vertex_UV = vec2(0.0, 0.0);
  EmitVertex();

  gl_Position = p2;
  Vertex_UV = vec2(0.0, 1.0);
  EmitVertex();

  gl_Position = p3;
  Vertex_UV = vec2(1.0, 0.0);
  EmitVertex();

  gl_Position = p4;
  Vertex_UV = vec2(1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

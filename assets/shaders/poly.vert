#version 330 core

layout(location = 0) in vec2 a_position;

uniform vec2 u_resolution;

void main()
{
  vec2 ndc = (a_position / u_resolution) * 2.0 - 1.0;
  ndc.y = -ndc.y;
  gl_Position = vec4(ndc, 0.0, 1.0);
}

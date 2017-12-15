#version 120

attribute vec3 vPosition;
attribute vec2 vTexCoord;

varying vec4 pos_view;
varying vec2 texCoord;

uniform mat4 model_view;
uniform mat4 projection;


void main() {
  vec4 vPosition4 = vec4(vPosition, 1.0);
  vec4 vPosview = model_view * vPosition4;
  pos_view = vPosview;                     // for fog
  gl_Position = projection * vPosview;
  texCoord = vTexCoord;
} 

#version 130

in vec3 vPosition;
in vec3 vColor;
out vec3 vert_color;
uniform mat4 model_view;
uniform mat4 projection;

void main() {
  vert_color = vColor;
  vec4 vPosition4 = vec4(vPosition, 1.0);
  gl_Position = projection * model_view * vPosition4;
} 

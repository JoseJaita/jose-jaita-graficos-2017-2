#version 130

in vec3 vert_color;
out vec4 fColor;

void main() { 
  fColor = vec4(vert_color,1.0);
} 

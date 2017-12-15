#version 120

//out vec4 fColor;
uniform vec3 vert_color;

void main() { 
  gl_FragColor = vec4(vert_color,1.0);
} 


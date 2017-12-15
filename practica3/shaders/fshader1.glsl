#version 120

varying vec2 texCoord;
varying vec4 pos_view;

//varying vec4 fColor;

uniform vec3 vert_color;

uniform int tex_flag;
uniform int fog_flag;

uniform sampler2D texture1;

void main() {
  // texture
  vec4 color;
  if (tex_flag == 1){
    color = vec4(vert_color,1.0)*texture2D(texture1, texCoord);
  }else{
    color = vec4(vert_color,1.0);
  }

  // fog
  float d;
  float f = 1.0;

  vec4 fog_color = vec4(0.7,0.7,0.7,0.5);
  float density = 0.09;

  // compute d
  d = length(pos_view); // pos_view sirve for compute distance
  
  if (fog_flag == 0){ // OFF
    
  }
  else if (fog_flag == 1){ // LINEAR
    f = (18-d)/(18-0);
    f = clamp(f, 0.0, 1.0);
  }
  else if (fog_flag == 2){
    f = 1.0/exp(d*density);
    f = clamp(f, 0.0, 1.0);
  }
  else if (fog_flag == 3) {
    f = 1.0/exp((d*density)*(d*density));
    f = clamp(f, 0.0, 1.0);
  }

  gl_FragColor = mix(fog_color, color, f);
  
} 


/************************************************************
 * Handout: rotate-cube-new.cpp (A Sample Code for Shader-Based OpenGL ---
                                 for OpenGL version 3.1 and later)
 * Originally from Ed Angel's textbook "Interactive Computer Graphics" 6th Ed
              sample code "example3.cpp" of Chapter 4.
 * Moodified by Yi-Jen Chiang to include the use of a general rotation function
   Rotate(angle, x, y, z), where the vector (x, y, z) can have length != 1.0,
   and also to include the use of the function NormalMatrix(mv) to return the
   normal matrix (mat3) of a given model-view matrix mv (mat4).

   (The functions Rotate() and NormalMatrix() are added to the file "mat-yjc-new.h"
   by Yi-Jen Chiang, where a new and correct transpose function "transpose1()" and
   other related functions such as inverse(m) for the inverse of 3x3 matrix m are
   also added; see the file "mat-yjc-new.h".)

 * Extensively modified by Yi-Jen Chiang for the program structure and user
   interactions. See the function keyboard() for the keyboard actions.
   Also extensively re-structured by Yi-Jen Chiang to create and use the new
   function drawObj() so that it is easier to draw multiple objects. Now a floor
   and a rotating cube are drawn.

** Perspective view of a color cube using LookAt() and Perspective()

** Colors are assigned to each vertex and then the rasterizer interpolates
   those colors across the triangles.
**************************************************************/
#include "Angel-yjc.h"
#include <fstream>
#include <string>
#include <cstring>

#define PI 3.14159265358979323846

using namespace std;

typedef Angel::vec3  color3;
typedef Angel::vec3  point3;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

string filename;

GLuint shader_1;       /* shader for single color */
GLuint shader_2;       /* shader several colors */
GLuint shader_1_shading;

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 15.0;

GLfloat angle = 0.0; // rotation angle in degrees
vec4 init_eye(7.0, 3.0, -10.0, 1.0); // initial viewer position
vec4 eye = init_eye;               // current viewer position

// flags for animation
int rollingFlag = 0; // 1: animation; 0: non-animation. Toggled by key 'a' or 'A'
int rollingToogleFlag = 1;
int wireFrameFlag = 1; // by default sphere is wire frame
int enableLightFlag = 0; //by default light is disable
int flatSmoothFlag = 0;   //0-> flat and 1-> smooth
int pointSpotFlag = 0;


int cubeFlag = 1;   // 1: solid cube; 0: wireframe cube. Toggled by key 'c' or 'C'
int floorFlag = 1;  // 1: solid floor; 0: wireframe floor. Toggled by key 'f' or 'F'

// axis variables
GLuint VAO_axis;
GLuint VBO_axis_data;
const int axis_NumVertices = 6;
point3 axis_points[axis_NumVertices];
point3 axis_colors[axis_NumVertices];
point3 vertices_for_axis[] = {
  point3(0.0, 0.0, 0.0),
  point3(10.0, 0.0, 0.0),
  point3(0.0, 10.0, 0.0),
  point3(0.0, 0.0, 10.0)};


//------------floor variables--------------------
GLuint VAO_floor;
GLuint VAO_floor_shading;
GLuint VBO_floor_data;
const int floor_NumVertices = 6;
point3 floor_points[floor_NumVertices];
vec3 floor_normal[floor_NumVertices];
color3 floor_color(0.0, 1.0, 0.0); //floor green
point3 vertices_for_floor[4] = {
  point3(5.0, 0.0, 8.0),
  point3(5.0, 0.0, -4.0),
  point3(-5.0, 0.0, -4.0),
  point3(-5.0, 0.0, 8.0)};
// floor for ligthing
float floor_shininess = 125.0;
color3 floor_ambient_color(0.2,0.2,0.2);
color3 floor_diffuse_color(0.0,1.0,0.0);
color3 floor_specular_color(0.0,0.0,0.0);

//---------------sphere variables-------------------------
GLuint VAO_sphere;
GLuint VAO_sphere_shading;
GLuint VBO_sphere_data;
float radius = 1.0f;
int sphere_NumVertices; // assign when read file
point3* sphere_points; // assign when read file
vec3* sphere_normal_flat;
vec3* sphere_normal_smooth;
color3 sphere_color(1.0, 0.84, 0.0); // sphere golden yelow
mat4 M = {1.0,0.0,0.0,0.0,
	  0.0,1.0,0.0,0.0,
	  0.0,0.0,1.0,0.0,
	  0.0,0.0,0.0,1.0};//save rotation history

// sphere route
const float delta_angle = 3.0f;
const float step = 0.02f; 
const int NUM_ROUTES = 3;
enum Route{A_B,B_C,C_A};
Route route;
const point3 STATION[NUM_ROUTES] = {point3(3.0,1.0,5.0),
				    point3(-1.0,1.0,-4.0),
				    point3(3.5,1.0,-2.5)};
point3 loc_sph = STATION[0];
point3 loc_sph_history[2] = {STATION[0],STATION[0]};
vec3 eje_r;
vec3 route_dir[NUM_ROUTES];
vec3 eje_rotation[NUM_ROUTES];

// sphere ligthing
color4 sphere_ambient_color(0.2,0.2,0.2,1.0);
color4 sphere_diffuse_color(1.0,0.84,0.0,1.0);
color4 sphere_specular_color(1.0,0.84,0.0,1.0);
float sphere_shininess = 125.0;

// RGBA colors
color3 vertex_colors[8] = {
    color3( 0.0, 0.0, 0.0),  // black
    color3( 1.0, 0.0, 0.0),  // red
    color3( 1.0, 1.0, 0.0),  // yellow
    color3( 0.0, 1.0, 0.0),  // green
    color3( 0.0, 0.0, 1.0),  // blue
    color3( 1.0, 0.0, 1.0),  // magenta
    color3( 1.0, 1.0, 1.0),  // white
    color3( 0.0, 1.0, 1.0)   // cyan
};

//----------- lights -------------------------
color3 global_light_color(1.0,1.0,1.0);
// directional light
color3 d_l_ambient_color(0.0,0.0,0.0);
color3 d_l_diffuse_color(0.8,0.8,0.8);
color3 d_l_specular_color(0.2,0.2,0.2);
vec3 d_l_direction(0.1,0.0,-1.0); // eye coordinate-system

// point and spot
float const_atte = 2.0;
float linear_atte = 0.01;
float quad_atte = 0.001;
// point light
color3 p_l_ambient_color(0.0,0.0,0.0);
color3 p_l_diffuse_color(1.0,1.0,1.0);
color3 p_l_specular_color(1.0,1.0,1.0);
point3 p_l_position(-14.0,12.0,-3.0); // world coordinate-system
// spot light
color3 s_l_ambient_color(0.0,0.0,0.0);
color3 s_l_diffuse_color(1.0,1.0,1.0);
color3 s_l_specular_color(1.0,1.0,1.0);
point3 s_l_position(-14.0,12.0,-3.0); // world coordinate-system
point3 s_l_dir_to(-6.0,0.0,-4.5); // point to pointing spot light
float s_l_exp = 15.0;
float s_l_angle = cos(20.0*PI/180.0);

//----------------------------------------------------------------
void readFile(){
  fstream in;
  int points;
  int num_t;
  float temp[3];
  
  in.open(filename);
  if (!in) {
    cerr << "Couldn't open " << filename << " !" << endl;
    exit(0);
  }
  in >> num_t;
  sphere_points = new point3[num_t * 3];
  int count = 0;
  for (int i = 0; i < num_t; i++) {
    in >> points;
    for (int j = 0; j < points; j++) {
      for (int k = 0; k < 3; k++) {
	in >> temp[k];	
      }
      point3 tmp( temp[0], temp[1], temp[2]);
      sphere_points[count++] = tmp;
    }
  }
  sphere_NumVertices = num_t * 3;
  in.close();
}
//----------------------------------------------------------------
void setNormalSmoothSphere(){
  sphere_normal_smooth = new vec3[sphere_NumVertices];
  for (int i = 0; i < sphere_NumVertices; i++) {
    sphere_normal_smooth[i] = normalize(sphere_points[i]);
  }
}
void setNormalFlatSphere(){
  sphere_normal_flat = new vec3[sphere_NumVertices];
  for (int i = 0; i < int(sphere_NumVertices/3); i++) {
    vec3 v1 = sphere_points[3*i+1] - sphere_points[3*i];
    vec3 v2 = sphere_points[3*i+2] - sphere_points[3*i];
    vec3 n = normalize(cross(v1,v2));
    sphere_normal_flat[3*i] = n;
    sphere_normal_flat[3*i+1] = n;
    sphere_normal_flat[3*i+2] = n;
  }
}
void setNormalFloor(){
  //floor_normal = new vec3[floor_NumVertices];
  for (int i = 0; i < int(floor_NumVertices/3); i++) {
    vec3 v1 = floor_points[3*i+1] - floor_points[3*i];
    vec3 v2 = floor_points[3*i+2] - floor_points[3*i];
    vec3 n = normalize(cross(v1,v2));
    floor_normal[3*i] = n;
    floor_normal[3*i+1] = n;
    floor_normal[3*i+2] = n;
  }
  
}

void setDirRoutes(){
  //set directions
  route_dir[A_B] = normalize(STATION[1]-STATION[0]);
  route_dir[B_C] = normalize(STATION[2]-STATION[1]);
  route_dir[C_A] = normalize(STATION[0]-STATION[2]);
  route = A_B;
  //set ejes
  vec3 eje_y(0.0,1.0,0.0);
  eje_rotation[A_B] = cross(eje_y,route_dir[A_B]);
  eje_rotation[B_C] = cross(eje_y,route_dir[B_C]);
  eje_rotation[C_A] = cross(eje_y,route_dir[C_A]);
  eje_r = eje_rotation[A_B];
}

void setPointsColorsAxis(){
  axis_points[0] = vertices_for_axis[0];
  axis_points[1] = vertices_for_axis[1]; // x
  axis_points[2] = vertices_for_axis[0];
  axis_points[3] = vertices_for_axis[2]; // y
  axis_points[4] = vertices_for_axis[0];
  axis_points[5] = vertices_for_axis[3]; // z
  
  axis_colors[0] = vertex_colors[1];
  axis_colors[1] = vertex_colors[1];
  axis_colors[2] = vertex_colors[5];
  axis_colors[3] = vertex_colors[5];
  axis_colors[4] = vertex_colors[4];
  axis_colors[5] = vertex_colors[4];  
}

// generate 2 triangles: 6 vertices for floor
void setPointsfloor(){
  floor_points[0] = vertices_for_floor[0];
  floor_points[1] = vertices_for_floor[1];
  floor_points[2] = vertices_for_floor[2];
  
  floor_points[3] = vertices_for_floor[2];
  floor_points[4] = vertices_for_floor[3];
  floor_points[5] = vertices_for_floor[0];
}

//----------------------------------------------------------------------------

// OpenGL initialization
void init(){
  // loading shaders
  shader_1 = InitShader("shaders/vshader1.glsl", "shaders/fshader1.glsl");
  shader_2 = InitShader("shaders/vshader2.glsl", "shaders/fshader2.glsl");
  shader_1_shading = InitShader("shaders/vshader1_shading.glsl", "shaders/fshader1_shading.glsl");
  
  //-------------floor-------------
  setPointsfloor();
  setNormalFloor();
  glGenVertexArrays(1, &VAO_floor);         //without shading
  glGenVertexArrays(1, &VAO_floor_shading); // with shading
  
  glGenBuffers(1, &VBO_floor_data);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_floor_data);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points)+sizeof(floor_normal), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points); // copy vertices
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_normal), floor_normal); // copy normal
  
  glBindVertexArray(VAO_floor); // without shading
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); //position
  glEnableVertexAttribArray(0);

  glBindVertexArray(VAO_floor_shading);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); //position
  glEnableVertexAttribArray(0);
 
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(floor_points)); //normal 
  glEnableVertexAttribArray(1);


  //-----------------axis-------------------
  setPointsColorsAxis();
  glGenVertexArrays(1, &VAO_axis);
  glGenBuffers(1, &VBO_axis_data);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_axis_data);
  glBufferData(GL_ARRAY_BUFFER, sizeof(axis_points)+sizeof(axis_colors), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(axis_points), axis_points);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(axis_points), sizeof(axis_colors), axis_colors);
  glBindVertexArray(VAO_axis);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0); // link positions
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(axis_points));
  glEnableVertexAttribArray(1); // link colors
  
  //----------------sphere-------------------------------
  readFile(); // read vertices and set sphere_points
  setNormalFlatSphere(); // set normals for flat 
  setNormalSmoothSphere();//and smooth
  setDirRoutes(); // routes for sphere
  
  glGenVertexArrays(1, &VAO_sphere);
  glGenVertexArrays(1, &VAO_sphere_shading);
  
  glGenBuffers(1, &VBO_sphere_data);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere_data);
  glBufferData(GL_ARRAY_BUFFER, 3*sphere_NumVertices*sizeof(point3), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sphere_NumVertices*sizeof(point3), sphere_points);
  glBufferSubData(GL_ARRAY_BUFFER, sphere_NumVertices*sizeof(point3), sphere_NumVertices*sizeof(point3), sphere_normal_flat);
  glBufferSubData(GL_ARRAY_BUFFER, 2*sphere_NumVertices*sizeof(point3), sphere_NumVertices*sizeof(point3), sphere_normal_smooth);
  
  glBindVertexArray(VAO_sphere);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0); // link position

  glBindVertexArray(VAO_sphere_shading);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); //position
  glEnableVertexAttribArray(0);
 
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sphere_NumVertices*sizeof(point3))); //normal flat
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(2*sphere_NumVertices*sizeof(point3))); //normal smooth
  glEnableVertexAttribArray(2);
    
  // opengl configuration  
  glEnable( GL_DEPTH_TEST );
  glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
  glLineWidth(2.0);
}


void display( void ){
  GLuint  modelView_id;
  GLuint  projection_id;
  
  //set background color
  glClearColor(0.529,0.807,0.92,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // matrix perpective and matrix view
  mat4 p = Perspective(fovy, aspect, zNear, zFar); //matrix perpective
  vec4 at(0.0, 0.0, 0.0, 1.0);
  vec4 up(0.0, 1.0, 0.0, 0.0);
  mat4 view = LookAt(eye, at, up);                   //matrix view

  
  if (enableLightFlag){
    //-----------------draw floor---------------------------
    glUseProgram(shader_1_shading);
    //send matrix
    mat4 floor_model_view = view;
    mat3 normal_matrix_eye = NormalMatrix(floor_model_view, 1);
    glUniformMatrix4fv(glGetUniformLocation(shader_1_shading, "model_view"), 1, GL_TRUE, floor_model_view);
    glUniformMatrix4fv(glGetUniformLocation(shader_1_shading, "projection"), 1, GL_TRUE, p);
    glUniformMatrix3fv(glGetUniformLocation(shader_1_shading, "normal_matrix_eye"), 1, GL_TRUE, normal_matrix_eye);
    //send material properties
    glUniform3fv(glGetUniformLocation(shader_1_shading, "material_ambient_color"), 1, floor_ambient_color);
    glUniform3fv(glGetUniformLocation(shader_1_shading, "material_diffuse_color"), 1, floor_diffuse_color);
    glUniform3fv(glGetUniformLocation(shader_1_shading, "material_specular_color"), 1, floor_specular_color);
    glUniform1f(glGetUniformLocation(shader_1_shading, "shininess"), floor_shininess);

    //send light properties for point light
    glUniform3fv(glGetUniformLocation(shader_1_shading, "light_ambient_color"), 1, p_l_ambient_color);
    glUniform3fv(glGetUniformLocation(shader_1_shading, "light_diffuse_color"), 1, p_l_diffuse_color);
    glUniform3fv(glGetUniformLocation(shader_1_shading, "light_specular_color"), 1, p_l_specular_color);
    point4 l_p_eye4 = view*point4(p_l_position,1.0); // convert position light to eye
    point3 l_p_eye3(l_p_eye4.x, l_p_eye4.y, l_p_eye4.z);
    glUniform3fv(glGetUniformLocation(shader_1_shading, "light_position"), 1, l_p_eye3);// to eye
  
    glUniform1f(glGetUniformLocation(shader_1_shading, "const_atte"), const_atte);
    glUniform1f(glGetUniformLocation(shader_1_shading, "linear_atte"), linear_atte);
    glUniform1f(glGetUniformLocation(shader_1_shading, "quad_atte"), quad_atte);
    
    //send directional and global light
    glUniform3fv(glGetUniformLocation(shader_1_shading, "global_light_color"), 1,global_light_color);
    glUniform3fv(glGetUniformLocation(shader_1_shading, "l_d_ambient_color"), 1, d_l_ambient_color);
    glUniform3fv(glGetUniformLocation(shader_1_shading, "l_d_diffuse_color"), 1, d_l_diffuse_color);
    glUniform3fv(glGetUniformLocation(shader_1_shading, "l_d_specular_color"), 1, d_l_specular_color);
    glUniform3fv(glGetUniformLocation(shader_1_shading, "l_d_direction"), 1, d_l_direction);//already in eye

    // flat or smooth dont work for floor
    glUniform1i(glGetUniformLocation(shader_1_shading, "flat_smooth_flag"), 0);
    // spot or point light
    if (pointSpotFlag == 0)
      glUniform1i(glGetUniformLocation(shader_1_shading, "point_spot_flag"), 0);
    else{
      glUniform1i(glGetUniformLocation(shader_1_shading, "point_spot_flag"), 1);
  
      l_p_eye4 = view*point4(s_l_dir_to,1.0);
      l_p_eye3 = point3(l_p_eye4.x, l_p_eye4.y, l_p_eye4.z);
  
      glUniform3fv(glGetUniformLocation(shader_1_shading, "light_direction_to"), 1, l_p_eye3);
      glUniform1f(glGetUniformLocation(shader_1_shading, "spot_exp"), s_l_exp);
      glUniform1f(glGetUniformLocation(shader_1_shading, "spot_angle"), s_l_angle);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(VAO_floor_shading);
    glDrawArrays(GL_TRIANGLES, 0, floor_NumVertices);

    //-----------------------draw sphere-----------------------------------------
    if (wireFrameFlag){
      glUseProgram(shader_1);
      glUniformMatrix4fv(glGetUniformLocation(shader_1,"projection"), 1, GL_TRUE, p);
      glUniform3fv(glGetUniformLocation(shader_1,"vert_color"),1,sphere_color);
      mat4 mt_s = Translate(loc_sph);
      //check if B was pressed
      if (rollingToogleFlag && rollingFlag)
	M = Rotate(delta_angle, eje_r.x,eje_r.y,eje_r.z)*M;
    
      mat4 mv_s = view*mt_s*M; // translacion(mt_s)*rotacion(M)
      glUniformMatrix4fv(glGetUniformLocation(shader_1,"model_view"), 1, GL_TRUE, mv_s);
      
      glBindVertexArray(VAO_sphere);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glDrawArrays(GL_TRIANGLES, 0, sphere_NumVertices);
    }
    else{
      glUseProgram(shader_1_shading);
      mat4 mt_s = Translate(loc_sph);
      if (rollingToogleFlag && rollingFlag)
	M = Rotate(delta_angle, eje_r.x,eje_r.y,eje_r.z)*M;
      mat4 sphere_model_view = view*mt_s*M;
      normal_matrix_eye = NormalMatrix(sphere_model_view, 1);
  
      glUniformMatrix4fv(glGetUniformLocation(shader_1_shading, "model_view"), 1, GL_TRUE, sphere_model_view);
      glUniformMatrix4fv(glGetUniformLocation(shader_1_shading, "projection"), 1, GL_TRUE, p);
      glUniformMatrix3fv(glGetUniformLocation(shader_1_shading, "normal_matrix_eye"), 1, GL_TRUE, normal_matrix_eye);
      //send material properties
      glUniform3fv(glGetUniformLocation(shader_1_shading, "material_ambient_color"), 1, sphere_ambient_color);
      glUniform3fv(glGetUniformLocation(shader_1_shading, "material_diffuse_color"), 1, sphere_diffuse_color);
      glUniform3fv(glGetUniformLocation(shader_1_shading, "material_specular_color"), 1, sphere_specular_color);
      glUniform1f(glGetUniformLocation(shader_1_shading, "shininess"), sphere_shininess);

      //send flags flat and smooth, this only work for sphere
      if (flatSmoothFlag == 0)
	glUniform1i(glGetUniformLocation(shader_1_shading, "flat_smooth_flag"), 0);
      else
	glUniform1i(glGetUniformLocation(shader_1_shading, "flat_smooth_flag"), 1);

      glBindVertexArray(VAO_sphere_shading);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glDrawArrays(GL_TRIANGLES, 0, sphere_NumVertices);
    }
  }
  
  // Enable light OFF 
  else{
    //-----------------draw floor---------------------------
    glUseProgram(shader_1); // Use the shader program
    modelView_id = glGetUniformLocation(shader_1, "model_view" );
    projection_id = glGetUniformLocation(shader_1, "projection" );
  
    glUniform3fv(glGetUniformLocation(shader_1,"vert_color"),1, floor_color);
    glUniformMatrix4fv(modelView_id, 1, GL_TRUE, view);
    glUniformMatrix4fv(projection_id, 1, GL_TRUE, p);
  
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(VAO_floor);
    glDrawArrays(GL_TRIANGLES, 0, floor_NumVertices);

    //------------------draw sphere -------------------------
    glUseProgram(shader_1);
    // same shader1, p
    glUniform3fv(glGetUniformLocation(shader_1,"vert_color"),1,sphere_color);
    mat4 mt_s = Translate(loc_sph);
    //check if B was pressed
    if (rollingToogleFlag && rollingFlag) {
      M = Rotate(delta_angle, eje_r.x,eje_r.y,eje_r.z)*M;
    }
    
    mat4 mv_s = view*mt_s*M; // translacion(mt_s)*rotacion(M)
    glUniformMatrix4fv(modelView_id, 1, GL_TRUE, mv_s);
    glBindVertexArray(VAO_sphere);
    if (wireFrameFlag) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLES, 0, sphere_NumVertices);

    
  }

  //------------------draw axis---------------------------------
  glUseProgram(shader_2); // Use the shader program
  glUniformMatrix4fv(glGetUniformLocation(shader_2, "projection"), 1, GL_TRUE, p);// same p, mv
  glUniformMatrix4fv(glGetUniformLocation(shader_2, "model_view"), 1, GL_TRUE, view);
  glBindVertexArray(VAO_axis);
  glDrawArrays(GL_LINES, 0, axis_NumVertices);

  glutSwapBuffers();

  // save last location of sphere
  point3 tmp = loc_sph_history[1];
  loc_sph_history[0] = tmp;
  loc_sph_history[1] = loc_sph;
}
//---------------------------------------------------------------------------
void idle (void){
  //angle += 0.02;
  //angle += delta_angle;  //YJC: change this value to adjust the cube rotation speed.
  
  //sphere translation
  switch (route) {
  case A_B: {
    //compute new location, 
    loc_sph += route_dir[A_B]*radius*delta_angle*0.0174532925f;
    if (loc_sph.x < STATION[1].x){
      loc_sph = STATION[1];
      route = B_C;
      eje_r = eje_rotation[B_C];
    }
    break;
  }
  case B_C: {
    loc_sph += route_dir[B_C]*radius*delta_angle*0.0174532925f;
    if (loc_sph.x > STATION[2].x){
      loc_sph = STATION[2];
      route = C_A;
      eje_r = eje_rotation[C_A];
    }
    break;
  }
  case C_A: {
    loc_sph += route_dir[C_A]*radius*delta_angle*0.0174532925f;
    if (loc_sph.x < STATION[0].x){
      loc_sph = STATION[0];
      route = A_B;
      eje_r = eje_rotation[A_B];
    }
    break;
  }
  default:
    break;
  }
  glutPostRedisplay();
}
//function menu
void mainMenu(int id){
  switch (id) {
  case 1: { // default View Point
    eye = init_eye;
    break;
  }
  case 6: {
    exit(0);
    break;
  }
default:
    break;
  }
  glutPostRedisplay();
};
void enableLighthingMenu(int id){
  switch (id) {
  case 1: { // No
    enableLightFlag = 0;
    break;
  }
  case 2: { // Yes
    enableLightFlag = 1;
    break;
  }
  default:
    break;
  }
  glutPostRedisplay();
}
void shadingMenu(int id){
  switch (id) {
  case 1: { //
    if (enableLightFlag){
      wireFrameFlag = 0;
      flatSmoothFlag = 0;
    }
    break;
  }
  case 2: { // 
    if (enableLightFlag){
      wireFrameFlag = 0;
      flatSmoothFlag = 1;
    }
    break;
  }
  default:
    break;
  }
  glutPostRedisplay();
}
void lightSourceMenu(int id){
  switch (id) {
  case 1: { // spot
    if (enableLightFlag)
      pointSpotFlag = 1;
    break;
  }
  case 2: { // point
    if (enableLightFlag)
      pointSpotFlag = 0;
    break;
  }
  default:
    break;
  }
  glutPostRedisplay();
}
void wireFrameMenu(int id){
  switch (id) {
  case 1: { // No, draw sphere as a solid
    wireFrameFlag = 0;
    break;
  }
  case 2: { // Yes, draw as a wire-frame
    wireFrameFlag = 1;
    break;
  }
  default:
    break;
  }
  glutPostRedisplay();
}


//---------------------------------------------------------------------------
void addItemsMenu(){
  int enable_lighting_menu_id = glutCreateMenu(enableLighthingMenu);
  glutAddMenuEntry("No",1);
  glutAddMenuEntry("Yes",2);
  
  int shading_menu_id = glutCreateMenu(shadingMenu);
  glutAddMenuEntry("Flat Shading", 1);
  glutAddMenuEntry("Smooth Shading", 2);

  int light_source_id = glutCreateMenu(lightSourceMenu);
  glutAddMenuEntry("Spot Light", 1);
  glutAddMenuEntry("Point Light", 2);

  int wire_frame_id = glutCreateMenu(wireFrameMenu);
  glutAddMenuEntry("No", 1);
  glutAddMenuEntry("Yes", 2);

  glutCreateMenu(mainMenu);
  glutAddMenuEntry("Default View Point", 1);
  glutAddSubMenu("Enable Lighting", enable_lighting_menu_id);
  glutAddSubMenu("Shading", shading_menu_id);
  glutAddSubMenu("Light Source", light_source_id);
  glutAddSubMenu("Wire Frame", wire_frame_id);
  glutAddMenuEntry("Quit", 6);
  glutAttachMenu(GLUT_LEFT_BUTTON);
}

//---------------------------------------------------------------------------
void mouse(int button, int state, int x, int y){
  switch (button) {
  case GLUT_RIGHT_BUTTON: { // enable and disable rolling
    //button only have effect when B already was pressed
    if (state ==GLUT_DOWN  && rollingFlag){ 
      rollingToogleFlag = 1-rollingToogleFlag;
      if (rollingToogleFlag) {
	loc_sph = loc_sph_history[0];
	glutIdleFunc(idle);
      }
      else glutIdleFunc(NULL);
    }
    break;
  }
default:
    break;
  }
  //glutPostRedisplay();
}

//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y){
  switch(key) {
  case 033: // Escape Key
  case 'q': case 'Q':
    exit( EXIT_SUCCESS );
    break;
    
  case 'X': eye[0] += 1.0; break;
  case 'x': eye[0] -= 1.0; break;
  case 'Y': eye[1] += 1.0; break;
  case 'y': eye[1] -= 1.0; break;
  case 'Z': eye[2] += 1.0; break;
  case 'z': eye[2] -= 1.0; break;
    
  case 'b': case 'B': // rolling the sphere
    rollingFlag = 1 ;
    rollingToogleFlag = 1;
    glutIdleFunc(idle);
    break;
    
  }
  glutPostRedisplay();
}
//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat) width  / (GLfloat) height;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main(int argc, char **argv){
  int err;
  if (argc < 2){
    cout << "indique el file a leer, eg. sphere/sphere.1024" << "\n";
    exit(0);
  }
  filename = argv[1];
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(600, 600);
  // glutInitContextVersion(3, 2);
  // glutInitContextProfile(GLUT_CORE_PROFILE);
  glutCreateWindow("Color Cube");
  
  /* Call glewInit() and error checking */
  err = glewInit();
  if (GLEW_OK != err){
    printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
    exit(1);
  }
  addItemsMenu();
  glutMouseFunc(mouse);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(NULL);
  glutKeyboardFunc(keyboard);
  
  init();
  glutMainLoop();
  return 0;
}

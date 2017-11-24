// Para compilar en ubuntu
// g++ ejercicio2.cpp -o ejercicio2 -lGL -lglut -lGLU -lGLEW && ./ejercicio2

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define NUMVTX (8)

static int winwidth,winheight;
static int mx,my;
static int flag=0;
static float rotx=0.0f, roty=0.0f;

int shaderProgram;
unsigned int VBO, VAO, EBO;
unsigned int VBO_H, VAO_H;
unsigned int VBO_A, VAO_A;

const char *vertexShaderSource = "#version 130 \n"
  "in vec3 Pos;\n"
  "in vec3 Color;\n"
  "in vec3 Normal;\n"

  "out vec3 vertColor;\n"
  "out vec3 vertPosWorld;\n"
  "out vec3 normal;\n"

  "uniform mat3 normalMatrix;\n"
  "uniform mat4 model;\n"
  "uniform mat4 view;\n"
  "uniform mat4 projection;\n"
  "void main()\n"
  "{\n"
  "   vertColor = Color;\n"
  "   normal =  normalMatrix*Normal;\n"
  "   vertPosWorld = vec3(model*vec4(Pos, 1.0));\n"
  "   gl_Position = projection*view*vec4(vertPosWorld, 1.0);\n"
  "}\0";
const char *fragmentShaderSource = "#version 130 \n"
  "out vec4 FragColor;\n"
  
  "in vec3 vertColor;\n"
  "in vec3 vertPosWorld;\n"
  "in vec3 normal;\n"
  
  "uniform vec3 lightColor;\n"
  "//uniform vec3 lightLoc;\n"
  "uniform vec3 lightDirection;\n"

  "void main()\n"
  "{\n"
  "   float ka = 0.3;\n"
  "   vec3 ambient = ka*lightColor;\n"
  "   vec3 norm = normalize(normal);\n"
  "   //vec3 light = normalize(lightLoc-vertPosWorld);\n"
  "   vec3 light = normalize(lightDirection);\n"
  "   float cos_theta = max(dot(norm,light),0.0);\n"
  "   vec3 diffuse = cos_theta*lightColor;\n"
  "   FragColor = vec4(vertColor*(ambient+diffuse),1.0);\n"
  "}\n\0";

GLfloat J[] = {-0.5f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       -0.5f, -0.5f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.0f, -0.5f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       0.0f, -0.5f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       -0.5f, -0.5f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.0f, -0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,

	       -0.5f, -0.4999f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       -0.5f, -0.4999f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.0f, -0.4999f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       0.0f, -0.4999f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       -0.5f, -0.4999f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.0f, -0.4999f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

	       0.0f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       0.0f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       0.0f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       0.0f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       0.0f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       0.0f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

	       -0.0001f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       -0.0001f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       -0.0001f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	       -0.0001f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	       -0.0001f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	       -0.0001f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

	       -0.5f, 0.5f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       -0.5f, 0.5f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.5f, 0.5f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       0.5f, 0.5f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       -0.5f, 0.5f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.5f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

	       -0.5f, 0.4999f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       -0.5f, 0.4999f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.5f, 0.4999f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       0.5f, 0.4999f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       -0.5f, 0.4999f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.5f, 0.4999f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       
};

GLfloat H[] = {-0.25f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	       -0.25f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       -0.25f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	       -0.25f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	       -0.25f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       -0.25f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

	       -0.2499f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	       -0.2499f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       -0.2499f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       -0.2499f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       -0.2499f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       -0.2499f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

	       0.25f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	       0.25f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       0.25f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       0.25f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       0.25f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       0.25f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

	       0.2499f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	       0.2499f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       0.2499f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	       0.2499f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	       0.2499f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       0.2499f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

	       -0.25f, 0.0f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       -0.25f, 0.0f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.25f, 0.0f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       0.25f, 0.0f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       -0.25f, 0.0f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.25f, 0.0f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,

	       -0.25f, 0.0001f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       -0.25f, 0.0001f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.25f, 0.0001f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       0.25f, 0.0001f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       -0.25f, 0.0001f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.25f, 0.0001f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f
};
  
GLfloat A[] = {-0.25f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	       -0.25f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       -0.25f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	       -0.25f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	       -0.25f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       -0.25f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

	       -0.2499f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	       -0.2499f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       -0.2499f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       -0.2499f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       -0.2499f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       -0.2499f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

	       0.25f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	       0.25f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       0.25f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       0.25f,  -0.5f, -0.25f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	       0.25f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	       0.25f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

	       0.2499f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	       0.2499f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       0.2499f,  -0.5f, -0.25f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f, 0.0f,
	       0.2499f,  -0.5f, -0.25f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f, 0.0f,
	       0.2499f, 0.5f, 0.25f,  0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	       0.2499f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

	       -0.25f, 0.0f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       -0.25f, 0.0f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.25f, 0.0f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       0.25f, 0.0f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       -0.25f, 0.0f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.25f, 0.0f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

	       -0.25f, -0.0001f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       -0.25f, -0.0001f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.25f, -0.0001f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       0.25f, -0.0001f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       -0.25f, -0.0001f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.25f, -0.0001f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,

	       -0.25f, 0.5f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       -0.25f, 0.5f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.25f, 0.5f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       0.25f, 0.5f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	       -0.25f, 0.5f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	       0.25f, 0.5f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

	       -0.25f, 0.4999f, 0.25f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       -0.25f, 0.4999f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.25f, 0.4999f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       0.25f, 0.4999f, 0.25f,   0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	       -0.25f, 0.4999f, -0.25f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	       0.25f, 0.4999f, -0.25f,   1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
};


void init(){
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success){
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "Error en el vertex shader: \n" << infoLog << std::endl;
   }
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
   // comprobar errores en los shader al compilar
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "Error en el fragment shader: \n" << infoLog << std::endl;
  }
  // link shaders
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "Error al linkear el programa: \n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  //glEnable(GL_DEPTH_TEST);
  //VAO for J
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(J), J, GL_STATIC_DRAW);

  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

  //VAO for H
  glGenVertexArrays(1, &VAO_H);
  glGenBuffers(1, &VBO_H);
  glBindVertexArray(VAO_H);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_H);
  glBufferData(GL_ARRAY_BUFFER, sizeof(H), H, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

  //VAO for A
  glGenVertexArrays(1, &VAO_A);
  glGenBuffers(1, &VBO_A);
  glBindVertexArray(VAO_A);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_A);
  glBufferData(GL_ARRAY_BUFFER, sizeof(A), A, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6*sizeof(float)));
  glEnableVertexAttribArray(2);
  //glBindBuffer(GL_ARRAY_BUFFER, 0); 
  //glBindVertexArray(0);
}

void drawgraphix(){
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  /*glPushMatrix();
  glTranslatef(0.0f,0.0f,-3.0f);
  glRotatef(rotx,0.0f,1.0f,0.0f);
  glRotatef(roty,1.0f,0.0f,0.0f);*/

  //turn on shader
  glUseProgram(shaderProgram);

  //send location of source of light to fragment
  //glm::vec3 lightLocation(0.0f,0.0f,1.5f);
  glm::vec3 lightDirection(0.0f,0.0f,1.0f);
  glUniform3fv(glGetUniformLocation(shaderProgram,"lightDirection"),1,&lightDirection[0]);
  //send ambient light to fragment
  glm::vec3 lighC(1.0f,1.0f,1.0f);
  glUniform3fv(glGetUniformLocation(shaderProgram,"lightColor"),1,&lighC[0]);

  //add transformations matrix
  glm::mat4 model, model_j1, view, projection;
  model = glm::rotate(model, glm::radians(rotx), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(roty), glm::vec3(1.0f, 0.0f, 0.0f));

  model_j1 = glm::translate(model, glm::vec3(-1.5f ,0.0f, 0.0f)); 

  //compute normal matrix
  glm::mat3 normalMatrix = glm::mat3(transpose(inverse(model)));
  glUniformMatrix3fv(glGetUniformLocation(shaderProgram,"normalMatrix"),1,GL_FALSE,&normalMatrix[0][0]);
  
  
  view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -6.0f));
  projection = glm::perspective(glm::radians(45.0f), (float)winwidth / (float)winheight, 0.1f, 100.0f);
  unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
  unsigned int viewLoc  = glGetUniformLocation(shaderProgram, "view");
  unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model_j1[0][0]);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projLoc,1,GL_FALSE, &projection[0][0]);
  //render
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  
  //render 2th J with translate
  glm::mat4 model_j2 = glm::translate(model,glm::vec3(0.5f ,0.0f, 0.0f));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model_j2[0][0]);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  
  //render H
  glm::mat4 model_h;
  model_h = glm::translate(model, glm::vec3(-0.5f ,0.0f, 0.0f));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model_h[0][0]);
  glBindVertexArray(VAO_H);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  //render A
  glm::mat4 model_a;
  model_a = glm::translate(model, glm::vec3(1.5f ,0.0f, 0.0f));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model_a[0][0]);
  glBindVertexArray(VAO_A);
  glDrawArrays(GL_TRIANGLES, 0, 48);
  
  
  //glDrawElements(GL_TRIANGLES, 36,GL_UNSIGNED_INT,0);
  //glPopMatrix();
  glutSwapBuffers();
}


void reshapefunc(int width,int height)
{
  winwidth=width;
  winheight=height;
  
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  
  glViewport(0,0,width,height);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,(float)width/height,1.0,10.0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0f,0.0f,3.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
}

void mousefunc(int button,int state,int x,int y)
{
	if (button==GLUT_LEFT_BUTTON) {
		if (state==GLUT_DOWN) {
			flag=1;
		} else {
			flag=0;
		}
	}
}

void motionfunc(int x,int y)
{
  
  if (flag>0) {
    if (flag>1) {
      rotx+=360.0f*(x-mx)/winwidth;
      roty+=360.0f*(y-my)/winheight;
    }
    
    mx=x;
    my=y;
    
    drawgraphix();
    
    flag=2;
  }
  //printf("x: %f\n", rotx);
  //printf("y: %f\n", roty);
}

void keyboardfunc(unsigned char key,int x,int y)
{
	if (key=='q' || key==27) exit(0);
}


void idlefunc()
{
	glutPostRedisplay();
}

int main(int argc,char **argv)
{  
   winwidth=winheight=512;

   glutInit(&argc,argv);
   glutInitWindowSize(winwidth,winheight);
   glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
   glutCreateWindow("Practica 1 - Ejercicio 1.1");
   glewInit();
   init();
   
   glutDisplayFunc(drawgraphix);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(mousefunc);
   glutMotionFunc(motionfunc);
   glutKeyboardFunc(keyboardfunc);
   glutIdleFunc(idlefunc);

   glutMainLoop();
   return(0);
}

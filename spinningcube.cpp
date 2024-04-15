/**
 * @file spinningcube.cpp
 *
 * @brief OpenGL sipnning cube with a texture over 1 face.
 *
 *
 * @author Jaime Cabero Creus
 * @author Marcos Mosquera Miranda
 * @author Marcos Rial Troncoso
 *
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// GLM library to deal with matrix operations
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int gl_width = 640;
int gl_height = 480;

void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void render(double);

GLuint shader_program = 0; // shader program to set render pipeline
GLuint vao = 0; // Vertext Array Object to set input data
GLuint texture = 0; // Texture to paste on polygon
GLint mv_location, proj_location; // Uniforms for transformation matrices

int main() {
  // Start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(gl_width, gl_height, "My spinning cube - Texture over 1 face", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwSetWindowSizeCallback(window, glfw_window_size_callback);
  glfwMakeContextCurrent(window);

  // Start GLEW extension handler
  // glewExperimental = GL_TRUE;
  glewInit();

  // Get version info
  const GLubyte* vendor = glGetString(GL_VENDOR); // get vendor string
  const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte* glversion = glGetString(GL_VERSION); // version as a string
  const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION); // version as a string
  printf("Vendor: %s\n", vendor);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", glversion);
  printf("GLSL version supported %s\n", glslversion);
  printf("Starting viewport: (width: %d, height: %d)\n", gl_width, gl_height);

  // Enable Depth test: only draw onto a pixel if fragment closer to viewer
  glEnable(GL_DEPTH_TEST);
  // Set a smaller value as "closer" (default value)
  glDepthFunc(GL_LESS); 

  // Vertex Shader
  const char* vertex_shader =
    "#version 410\n"

    "in vec4 v_pos;"
    "in vec2 text_coord;"
    "in float text_alpha;"

    "out vec2 vs_text_coord;"
    "out float vs_text_alpha;"
    "out vec4 vs_color;"

    "uniform mat4 mv_matrix;"
    "uniform mat4 proj_matrix;"

    "void main() {"
    "  gl_Position = proj_matrix * mv_matrix * v_pos;"
    "  vs_text_coord = text_coord;"
    "  vs_text_alpha = text_alpha;"
    "  vs_color = v_pos * 2.0 + vec4(0.4, 0.4, 0.4, 1.0);"
    "}";

  // Fragment Shader
  const char* fragment_shader =
    "#version 410\n"

    "in vec2 vs_text_coord;"
    "in float vs_text_alpha;"
    "in vec4 vs_color;"

    "uniform sampler2D texture1;"

    "out vec4 frag_col;"

    "void main() {"
    "  vec4 text1 = texture(texture1,vs_text_coord);"
    "  frag_col = (1 - vs_text_alpha) * vs_color + vs_text_alpha * text1;"
    "}";

  // Shaders compilation
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);

  // Create program, attach shaders to it and link it
  shader_program = glCreateProgram();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);

  // Release shader objects
  glDeleteShader(vs);
  glDeleteShader(fs);

  // Vertex Array Object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Cube to be rendered
  //
  //                        0 -------- 3
  //                      / |        / |
  //                     7 -------- 4  |    <-- top-right-near
  // bottom-left-far     |  |       |  |                   (4)
  // (1) --->            |  1 ------|- 2
  //                     | /        | /
  //                     6 -------- 5
  //

  const GLfloat vertex_points[] = {
    // Cube faces (combination of 2 triangles)

    // BACK FACE
    -0.25f, -0.25f, -0.25f, // 1
    -0.25f,  0.25f, -0.25f, // 0
     0.25f, -0.25f, -0.25f, // 2

     0.25f,  0.25f, -0.25f, // 3
     0.25f, -0.25f, -0.25f, // 2
    -0.25f,  0.25f, -0.25f, // 0

    // RIGTH FACE
     0.25f, -0.25f, -0.25f, // 2
     0.25f,  0.25f, -0.25f, // 3
     0.25f, -0.25f,  0.25f, // 5

     0.25f,  0.25f,  0.25f, // 4
     0.25f, -0.25f,  0.25f, // 5
     0.25f,  0.25f, -0.25f, // 3

    // FRONT FACE
     0.25f, -0.25f,  0.25f, // 5
     0.25f,  0.25f,  0.25f, // 4
    -0.25f, -0.25f,  0.25f, // 6

    -0.25f,  0.25f,  0.25f, // 7
    -0.25f, -0.25f,  0.25f, // 6
     0.25f,  0.25f,  0.25f, // 4

    // LEFT FACE
    -0.25f, -0.25f,  0.25f, // 6
    -0.25f,  0.25f,  0.25f, // 7
    -0.25f, -0.25f, -0.25f, // 1

    -0.25f,  0.25f, -0.25f, // 0
    -0.25f, -0.25f, -0.25f, // 1
    -0.25f,  0.25f,  0.25f, // 7

    // BOTTOM FACE
     0.25f, -0.25f, -0.25f, // 2
     0.25f, -0.25f,  0.25f, // 5
    -0.25f, -0.25f, -0.25f, // 1

    -0.25f, -0.25f,  0.25f, // 6
    -0.25f, -0.25f, -0.25f, // 1
     0.25f, -0.25f,  0.25f, // 5

    // TOP FACE
     0.25f,  0.25f,  0.25f, // 4
     0.25f,  0.25f, -0.25f, // 3
    -0.25f,  0.25f,  0.25f, // 7

    -0.25f,  0.25f, -0.25f, // 0
    -0.25f,  0.25f,  0.25f, // 7
     0.25f,  0.25f, -0.25f  // 3

  };

  const GLfloat vertex_texcoords[] = {
    // Texture coordinates (setting for all faces, as it will be useful if needed to change wich one is active)

    // BACK FACE
    0.0f, 0.0f, // 1
    0.0f, 1.0f, // 0
    1.0f, 0.0f, // 2

    1.0f, 1.0f, // 3
    1.0f, 0.0f, // 2
    0.0f, 1.0f, // 0

    // RIGHT FACE
    1.0f, 0.0f, // 2
    1.0f, 1.0f, // 3
    0.0f, 0.0f, // 5

    0.0f, 1.0f, // 4
    0.0f, 0.0f, // 5
    1.0f, 1.0f, // 3
    
    // FRONT FACE
    1.0f, 0.0f, // 5
    1.0f, 1.0f, // 4
    0.0f, 0.0f, // 6

    0.0f, 1.0f, // 7
    0.0f, 0.0f, // 6
    1.0f, 1.0f, // 4
    
    // LEFT FACE
    1.0f, 0.0f, // 6
    1.0f, 1.0f, // 7
    0.0f, 0.0f, // 1

    0.0f, 1.0f, // 0
    0.0f, 0.0f, // 1
    1.0f, 1.0f, // 7
   
    // BOTTOM FACE
    1.0f, 1.0f, // 2
    1.0f, 0.0f, // 5
    0.0f, 1.0f, // 1

    0.0f, 0.0f, // 6
    0.0f, 1.0f, // 1
    1.0f, 0.0f, // 5
     
    // TOP FACE
    1.0f, 0.0f, // 4
    1.0f, 1.0f, // 3
    0.0f, 0.0f, // 7

    0.0f, 1.0f, // 0
    0.0f, 0.0f, // 7
    1.0f, 1.0f  // 3

  };

  const GLfloat vertex_textalpha[] = {
    // Texture alpha (this even could be useful to apply alpha interpolation to any face)

    // BACK FACE
    0.f, 0.f, 0.f,      // 1-0-2
    0.f, 0.f, 0.f,      // 3-2-0

    // RIGTH FACE
    0.f, 0.f, 0.f,      // 2-3-5
    0.f, 0.f, 0.f,      // 4-5-6

    // FRONT FACE       <-- This will be the face with the texture
    1.f, 1.f, 1.f,      // 5-4-6     
    1.f, 1.f, 1.f,      // 7-6-4

    // LEFT FACE
    0.f, 0.f, 0.f,      // 6-7-1
    0.f, 0.f, 0.f,      // 0-1-7

    // BOTTOM FACE
    0.f, 0.f, 0.f,      // 2-5-1
    0.f, 0.f, 0.f,      // 6-1-5

    // TOP FACE
    0.f, 0.f, 0.f,      // 4-3-7 
    0.f, 0.f, 0.f       // 0-7-3

  };


  // VAO, VBOs
  GLuint vbo[3];
  glGenVertexArrays(1, &vao);
  glGenBuffers(3, vbo);

  glBindVertexArray(vao);

  // VBO: 3D vertices
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_points), vertex_points, GL_STATIC_DRAW);
  // 0: Vertex position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  // VBO: Texture coords
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texcoords), vertex_texcoords, GL_STATIC_DRAW);
  // 1: Vertex texCoord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  // VBO: Texture alpha
  glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_textalpha), vertex_textalpha, GL_STATIC_DRAW);
  // 1: Vertex texAlpha attribute
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(2);

  // Unbind vbo (it was conveniently registered by VertexAttribPointer)
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Unbind vao
  glBindVertexArray(0);

  // Create texture object
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glUniform1i(glGetUniformLocation(shader_program, "texture1"), 0);

  // Load image for texture
  int width, height, nrChannels;
  // Before loading the image, we flip it vertically because
  // Images: 0.0 top of y-axis  OpenGL: 0.0 bottom of y-axis
  stbi_set_flip_vertically_on_load(1);
  unsigned char *data = stbi_load("texture.jpg", &width, &height, &nrChannels, 0);
  // Image from http://www.flickr.com/photos/seier/4364156221
  // CC-BY-SA 2.0
  if (data) {
    // Generate texture from image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load texture\n");
  }

  // Free image once texture is generated
  stbi_image_free(data);

  // Uniforms
  // - Model-View matrix
  // - Projection matrix
  mv_location = glGetUniformLocation(shader_program, "mv_matrix");
  proj_location = glGetUniformLocation(shader_program, "proj_matrix");

  // Render loop
  while(!glfwWindowShouldClose(window)) {

    processInput(window);

    render(glfwGetTime());

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}

void render(double currentTime) {
  float f = (float)currentTime * 0.3f;

  // Clear buffer color before draw next scene frame
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  // Set render window size
  glViewport(0, 0, gl_width, gl_height);

  // Active shader_program
  glUseProgram(shader_program);
  glBindVertexArray(vao);

  glm::mat4 mv_matrix, proj_matrix;

  mv_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.0f, -4.0f));
  mv_matrix = glm::translate(mv_matrix,
                             glm::vec3(sinf(2.1f * f) * 0.5f,
                                       cosf(1.7f * f) * 0.5f,
                                       sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));

  mv_matrix = glm::rotate(mv_matrix,
                          glm::radians((float)currentTime * 45.0f),
                          glm::vec3(0.0f, 1.0f, 0.0f));
  mv_matrix = glm::rotate(mv_matrix,
                          glm::radians((float)currentTime * 81.0f),
                          glm::vec3(1.0f, 0.0f, 0.0f));

  glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

  proj_matrix = glm::perspective(glm::radians(50.0f),
                                 (float) gl_width / (float) gl_height,
                                 0.1f, 1000.0f);
  glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glDrawArrays(GL_TRIANGLES, 0, 36);
}

// Function to stop the application when pressed ESC
void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
}

// Callback function to track window size and update viewport
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
  gl_width = width;
  gl_height = height;
  printf("New viewport: (width: %d, height: %d)\n", width, height);
}

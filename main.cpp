// Computer Graphics HW-SWGL 2026
// simple modeling tool
// Transformation and Projection

#include <iostream>

#include <GL/glu.h>
#include <GLFW/glfw3.h>

// OpenGL Mathematics (GLM)  https://glm.g-truc.net/
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace std;
using namespace glm;

const bool STEP2 = false;
const bool STEP3 = false;

float theta = 3.14159f / 4.0f;
float tho = 3.14159f / 4.0f;

int winWidth = 1280;
int winHeight = 720;

mat4x4 transformMat = mat4x4(1);

mat4x4 ViewMat = mat4x4(1);
mat4x4 ProjectionMat = mat4x4(1);

void printMat(const char *name, mat4x4 m) {
  printf("%s:\n", name);
  for (int row = 0; row < 4; row++) {
    printf("  [%.3f  %.3f  %.3f  %.3f]\n", m[0][row], m[1][row], m[2][row],
           m[3][row]);
  }
}

vec3 default_tetrahedron_vertices[4] = {vec3(1, 0, 0), vec3(0, 1, 0),
                                        vec3(0, 0, 1), vec3(0, 0, 0)};
vec3 tetrahedron_verts[4];

// NOTE: GLM uses COLUMN-MAJOR order: mat[col][row]
// A translation matrix looks like:
//   mat[0] = {1, 0, 0, 0}   (column 0)
//   mat[1] = {0, 1, 0, 0}   (column 1)
//   mat[2] = {0, 0, 1, 0}   (column 2)
//   mat[3] = {x, y, z, 1}   (column 3 holds translation)
// So translation T_x is at mat[3][0], T_y at mat[3][1], T_z at mat[3][2]

// step1: implement Translate Matrix
mat4x4 swTranslate(float x, float y, float z) {
  mat4x4 Translate = mat4x4(1);
  Translate[3][0] = x;
  // todo: y z

  return Translate;
}

// step1: implement Rotate Matrix
mat4x4 swRotateX(float angle) {
  mat4x4 Rotate = mat4x4(1);

  return Rotate;
}

mat4x4 swRotateY(float angle) {
  mat4x4 Rotate = mat4x4(1);

  return Rotate;
}

mat4x4 swRotateZ(float angle) {
  mat4x4 Rotate = mat4x4(1);

  return Rotate;
}

// optinal
mat4x4 swRotate(float angle, float x, float y, float z) {
  mat4x4 Rotate = mat4x4(1);

  return Rotate;
}

// step1: implement Scale(x, y, z)
mat4x4 swScale(float x, float y, float z) {
  mat4x4 Scale = mat4x4(1);

  return Scale;
}

// step2:
mat4x4 swLookAt(float eyeX, float eyeY, float eyeZ, float centerX,
                float centerY, float centerZ, float upX, float upY, float upZ) {
  mat4x4 V = mat4x4(0);
  // todo: fill in V

  return V;
}

// step3: implement perspective projection matrix
mat4x4 swPerspective(float fovyDeg, float aspect, float near, float far) {
  mat4x4 P = mat4x4(0);
  // todo: fill in P
  return P;
}

void swTriangle(vec3 color, vec3 in_v1, vec3 in_v2, vec3 in_v3,
                mat4x4 Modelmatrix) {
  // vertices start in OBJECT SPACE
  vec4 v1(in_v1.x, in_v1.y, in_v1.z, 1);
  vec4 v2(in_v2.x, in_v2.y, in_v2.z, 1);
  vec4 v3(in_v3.x, in_v3.y, in_v3.z, 1);

  // OBJECT -> WORLD SPACE  (model transform, step1)
  v1 = Modelmatrix * v1;
  v2 = Modelmatrix * v2;
  v3 = Modelmatrix * v3;

  // WORLD -> VIEW (CAMERA) SPACE  (step2: apply view matrix, replace gluLookAt)
  if (STEP2) {
    v1 = ViewMat * v1;
    // todo: v2, v3
  }

  // VIEW -> CLIP SPACE -> NDC  (step3: projection + perspective divide)
  if (STEP3) {
    v1 = ProjectionMat * v1;
    // todo: v2, v3
    // todo: perspective division (divide x,y,z by w)
  }

  // result passed to OpenGL as NDC or world coords depending on active steps
  glColor3f(color.r, color.g, color.b);
  glVertex3f(v1.x, v1.y, v1.z);
  glVertex3f(v2.x, v2.y, v2.z);
  glVertex3f(v3.x, v3.y, v3.z);
}

void Draw_Tetrahedron() {
  vec3 color(1, 1, 0);
  // glColor3f(1, 1, 0);
  glBegin(GL_TRIANGLES);

  swTriangle(vec3(1, 0, 0), tetrahedron_verts[0], tetrahedron_verts[1],
             tetrahedron_verts[2], transformMat);

  swTriangle(vec3(0, 0, 1), tetrahedron_verts[3], tetrahedron_verts[0],
             tetrahedron_verts[1], transformMat);

  swTriangle(vec3(0, 1, 0), tetrahedron_verts[2], tetrahedron_verts[3],
             tetrahedron_verts[0], transformMat);

  swTriangle(vec3(1, 1, 0), tetrahedron_verts[1], tetrahedron_verts[2],
             tetrahedron_verts[3], transformMat);

  glEnd();
}

void DrawGrid(int size = 10) {
  glBegin(GL_LINES);
  glColor3f(0.3, 0.3, 0.3);
  for (int i = 1; i < size; i++) {
    glVertex3f(i, -size, 0);
    glVertex3f(i, size, 0);
    glVertex3f(-i, -size, 0);
    glVertex3f(-i, size, 0);
    glVertex3f(-size, i, 0);
    glVertex3f(size, i, 0);
    glVertex3f(-size, -i, 0);
    glVertex3f(size, -i, 0);
  }
  glEnd();

  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(size, 0, 0);
  glColor3f(0.4, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(-size, 0, 0);

  glColor3f(0, 1, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, size, 0);
  glColor3f(0, 0.4, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, -size, 0);

  glColor3f(0, 0, 1);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, size);
  glEnd();
}

void Display(GLFWwindow *window) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, winWidth, winHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 0.1, 50);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(10 * cos(theta), -10 * sin(theta), 10, 0, 0, 0, 0, 0, 1);

  DrawGrid();

  // step 3: PROJECTION
  if (STEP3 == true) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho(0, winWidth, 0, winHeight, -2.0, 2.0);
    // ProjectionMat = mat4x4(1);
    ProjectionMat = swPerspective(60, (float)winWidth / (float)winHeight, 0.1,
                                  50); // uses your implementation
  }

  // step 2: viewing
  if (STEP2 == true) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    ViewMat = swLookAt(10 * cos(theta), -10 * sin(theta), 10, 0, 0, 0, 0, 0,
                       1); // sets ViewMat
  }

  Draw_Tetrahedron();

  glFlush();
  glfwSwapBuffers(window);
}

void init() {
  glClearColor(0, 0, 0, 0);
  glClearDepth(1.0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
}

// Converted special key function for GLFW.
void SpecialKey(GLFWwindow *window, int key, int scancode, int action,
                int mods) {
  // Process only key press and repeated actions.
  if (action != GLFW_PRESS && action != GLFW_REPEAT)
    return;

  switch (key) {
  case GLFW_KEY_F1:
    glfwSetWindowTitle(window, "F1: add a tetrahedron");
    for (int i = 0; i < 4; i++) {
      tetrahedron_verts[i][0] = default_tetrahedron_vertices[i][0];
      tetrahedron_verts[i][1] = default_tetrahedron_vertices[i][1];
      tetrahedron_verts[i][2] = default_tetrahedron_vertices[i][2];
    }
    break;

  case GLFW_KEY_F2:
    glfwSetWindowTitle(window, "F2: add a cube or somthing");
    // Add additional functionality here.
    break;

  case GLFW_KEY_F5:
    glfwSetWindowTitle(window, "F5: SAVE");
    // Add save functionality here.

    break;

  case GLFW_KEY_F6:
    glfwSetWindowTitle(window, "F6: LOAD");
    // Add load functionality here.

    break;

  default:
    break;
  }
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  // Here you can handle both regular and special keys.
  // Call your special key function for function keys.
  if (key == GLFW_KEY_F1 || key == GLFW_KEY_F2 || key == GLFW_KEY_F5 ||
      key == GLFW_KEY_F6) {
    SpecialKey(window, key, scancode, action, mods);
  }

  // Add handling for other keys as needed.

  // Only handle press and repeat events.
  if (action != GLFW_PRESS && action != GLFW_REPEAT)
    return;

  switch (key) {
  case GLFW_KEY_ESCAPE: // ESC key
    exit(0);
    break;

  // rotate world
  case GLFW_KEY_9:
    theta += 3.14159f / 90.0f;
    break;
  case GLFW_KEY_0:
    theta -= 3.14159f / 90.0f;
    break;
  case GLFW_KEY_MINUS:
    transformMat = mat4x4(1);
    break;

  // translate +x (handles both 'q' and 'Q')
  case GLFW_KEY_Q:
    glfwSetWindowTitle(window, "translate +x");
    transformMat = swTranslate(1, 0, 0) * transformMat;
    break;

  // translate -x (handles both 'a' and 'A')
  case GLFW_KEY_A:
    glfwSetWindowTitle(window, "translate -x");
    transformMat = swTranslate(-1, 0, 0) * transformMat;
    break;

  // translate +y (handles both 'w' and 'W')
  case GLFW_KEY_W:
    glfwSetWindowTitle(window, "translate +y");
    transformMat = swTranslate(0, 1, 0) * transformMat;
    break;

  // translate -y (handles both 's' and 'S')
  case GLFW_KEY_S:
    glfwSetWindowTitle(window, "translate -y");
    transformMat = swTranslate(0, -1, 0) * transformMat;
    break;

    // Add other keys as needed.

  default:
    break;
  }
}

int main(void) {
  // Initialize GLFW
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  // Create a windowed mode window and its OpenGL context
  GLFWwindow *window = glfwCreateWindow(
      winWidth, winHeight, "trans: Press F1 to add a tetrahedron", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);

  // Set callback for keyboard events
  glfwSetKeyCallback(window, KeyCallback);

  // Initialize OpenGL
  init();

  // Timing for periodic updates (~33ms interval)
  double previousTime = glfwGetTime();
  const double interval = 0.033; // ~33ms

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Check if it's time to update (simulate timer)
    double currentTime = glfwGetTime();
    if (currentTime - previousTime >= interval) {
      // Here you can include any periodic update logic.
      // In GLUT you used glutPostRedisplay(), but in GLFW,
      // since you're in control of the loop, just call Display().
      previousTime = currentTime;
    }

    // Render here
    Display(window);

    // Poll for and process events
    glfwPollEvents();
  }

  // Clean up and exit
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
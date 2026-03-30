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

// const bool STEP2 = false;
const bool STEP2 = true;
// const bool STEP3 = false;
const bool STEP3 = true;

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

// 定義一個 Cube 的 8 個頂點 (設定在 -0.5 ~ 0.5 的範圍，使其置中)
vec3 default_cube_vertices[8] = {
    vec3(-0.5, -0.5,  0.5), // 0:前左下
    vec3( 0.5, -0.5,  0.5), // 1:前右下
    vec3( 0.5,  0.5,  0.5), // 2:前右上
    vec3(-0.5,  0.5,  0.5), // 3:前左上
    vec3(-0.5, -0.5, -0.5), // 4:後左下
    vec3( 0.5, -0.5, -0.5), // 5:後右下
    vec3( 0.5,  0.5, -0.5), // 6:後右上
    vec3(-0.5,  0.5, -0.5)  // 7:後左上
};
vec3 cube_verts[8];

int current_shape = 1; // 1: Tetrahedron, 2: Cube

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
  Translate[3][1] = y;
  Translate[3][2] = z;
  // GLM函式庫是使用Column-Major，Translate[3][0]代表把平移量放在第四行第一位
  return Translate;
}

// step1: implement Rotate Matrix
mat4x4 swRotateX(float angle) {
  float rad = angle * M_PI / 180.0f; // 將角度轉為弧度
  mat4x4 Rotate = mat4x4(1);
  Rotate[1][1] = cos(rad);
  Rotate[1][2] = sin(rad);  // Col 1, Row 2，記得是Column-Major
  Rotate[2][1] = -sin(rad); // Col 2, Row 1
  Rotate[2][2] = cos(rad);
  return Rotate;
}

mat4x4 swRotateY(float angle) {
  float rad = angle * M_PI / 180.0f; // 將角度轉為弧度
  mat4x4 Rotate = mat4x4(1);
  Rotate[0][0] = cos(rad);
  Rotate[0][2] = -sin(rad);
  Rotate[2][0] = sin(rad);
  Rotate[2][2] = cos(rad);
  return Rotate;
}

mat4x4 swRotateZ(float angle) {
  float rad = angle * M_PI / 180.0f; // 將角度轉為弧度
  mat4x4 Rotate = mat4x4(1);
  Rotate[0][0] = cos(rad);
  Rotate[0][1] = sin(rad);
  Rotate[1][0] = -sin(rad);
  Rotate[1][1] = cos(rad);
  return Rotate;
}

// optinal
mat4x4 swRotate(float angle, float x, float y, float z) { // 實作對任意軸旋轉
  mat4x4 Rotate = mat4x4(1);
  float rad = angle * M_PI / 180.0f;
  float c = cos(rad);
  float s = sin(rad);
  float k = 1.0f - c;

  // 對輸入的旋轉軸 (x,y,z) 進行正規化 (Normalize)
  float len = sqrt(x * x + y * y + z * z);
  if (len < 0.00001f)
    return Rotate; // 若向量長度為 0 則不旋轉
  x /= len;
  y /= len;
  z /= len;

  // 第 1 行 (Column 0)
  Rotate[0][0] = x * x * k + c;
  Rotate[0][1] = y * x * k + z * s;
  Rotate[0][2] = z * x * k - y * s;

  // 第 2 行 (Column 1)
  Rotate[1][0] = x * y * k - z * s;
  Rotate[1][1] = y * y * k + c;
  Rotate[1][2] = z * y * k + x * s;

  // 第 3 行 (Column 2)
  Rotate[2][0] = x * z * k + y * s;
  Rotate[2][1] = y * z * k - x * s;
  Rotate[2][2] = z * z * k + c;

  return Rotate;
}

// step1: implement Scale(x, y, z)
mat4x4 swScale(float x, float y, float z) {
  mat4x4 Scale = mat4x4(1);
  Scale[0][0] = x;
  Scale[1][1] = y;
  Scale[2][2] = z;
  return Scale;
}

// step2:
mat4x4 swLookAt(float eyeX, float eyeY, float eyeZ, float centerX,
                float centerY, float centerZ, float upX, float upY, float upZ) {
  mat4x4 V = mat4x4(1);

  // 把輸入轉成向量方便計算
  vec3 eye(eyeX, eyeY, eyeZ);
  vec3 center(centerX, centerY, centerZ);
  vec3 up(upX, upY, upZ);

  // todo: fill in V
  // 1. 計算相機座標系的三個正交基底 (Forward, Right, Up)
  vec3 f = normalize(center - eye); // Forward: 從眼睛看向目標點
  vec3 s = normalize(cross(f, up)); // Right: 透過外積算出右邊方向
  vec3 u = cross(s, f);             // Up: 再外積一次確保這三個軸完全垂直

  // 2. 填入 View Matrix (注意 Column-Major 的寫法)
  // 第 1 到 3 行 (Column 0, 1, 2) 的前三個元素負責旋轉
  V[0][0] = s.x;
  V[1][0] = s.y;
  V[2][0] = s.z;
  V[0][1] = u.x;
  V[1][1] = u.y;
  V[2][1] = u.z;
  V[0][2] = -f.x;
  V[1][2] = -f.y;
  V[2][2] = -f.z;

  // 第 4 行 (Column 3) 負責平移 (Translate)
  V[3][0] = -dot(s, eye);
  V[3][1] = -dot(u, eye);
  V[3][2] = dot(f, eye);

  return V;
}

// step3: implement perspective projection matrix
mat4x4 swPerspective(float fovyDeg, float aspect, float zNear, float zFar) {
  // 將 near, far 參數改名為 zNear, zFar，避免跟 Windows 標頭檔的巨集衝突
  mat4x4 P = mat4x4(0); // 投影矩陣大部分都是 0，所以從全 0 開始

  float rad = fovyDeg * 3.1415926535f / 180.0f;
  float f = 1.0f / tan(rad / 2.0f); // 就是 cot(fovy / 2)

  // todo: fill in P
  // 依照 Column-Major 的規則填入
  P[0][0] = f / aspect;
  P[1][1] = f;

  // Z 的深度映射會計算到 [-1, 1] (或 [0, 1] 視 OpenGL 實作，這裡用標準 OpenGL
  // 的 [-1, 1])
  P[2][2] = -(zFar + zNear) / (zFar - zNear);
  P[2][3] = -1.0f; // 讓輸出的 w 值變成原本的 -z (或是 z)，這是 Perspective
                   // Divide 的關鍵

  P[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);

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
    v2 = ViewMat * v2;
    v3 = ViewMat * v3;
  }

  // VIEW -> CLIP SPACE -> NDC  (step3: projection + perspective divide)
  if (STEP3) {
    v1 = ProjectionMat * v1;
    // todo: v2, v3
    v2 = ProjectionMat * v2;
    v3 = ProjectionMat * v3;

    // todo: perspective division (divide x,y,z by w)
    // 把 4D 齊次座標 (Homogeneous Coordinates) 轉換為 3D 的 Normalized Device
    // Coordinates (NDC)
    if (v1.w != 0.0f) {
      v1.x /= v1.w;
      v1.y /= v1.w;
      v1.z /= v1.w;
    }
    if (v2.w != 0.0f) {
      v2.x /= v2.w;
      v2.y /= v2.w;
      v2.z /= v2.w;
    }
    if (v3.w != 0.0f) {
      v3.x /= v3.w;
      v3.y /= v3.w;
      v3.z /= v3.w;
    }
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

void Draw_Cube() {
  glBegin(GL_TRIANGLES);

  // 一個正方體有 6 個面，每個面 2 個三角形，以逆時針方向定義頂點
  int indices[36] = {
    0, 1, 2,  2, 3, 0, // 前面
    1, 5, 6,  6, 2, 1, // 右面
    5, 4, 7,  7, 6, 5, // 後面
    4, 0, 3,  3, 7, 4, // 左面
    3, 2, 6,  6, 7, 3, // 上面
    4, 5, 1,  1, 0, 4  // 下面
  };

  // 給每個面不同的顏色以便區分
  vec3 colors[6] = {
    vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1),
    vec3(1, 1, 0), vec3(1, 0, 1), vec3(0, 1, 1)
  };

  for (int i = 0; i < 6; i++) {
    // 每個面有兩個三角形
    swTriangle(colors[i], cube_verts[indices[i*6]], cube_verts[indices[i*6+1]], cube_verts[indices[i*6+2]], transformMat);
    swTriangle(colors[i], cube_verts[indices[i*6+3]], cube_verts[indices[i*6+4]], cube_verts[indices[i*6+5]], transformMat);
  }

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

  // 根據選擇的形狀來渲染
  if (current_shape == 1) {
    Draw_Tetrahedron();
  } else if (current_shape == 2) {
    Draw_Cube();
  }

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
    current_shape = 1;
    transformMat = mat4x4(1); // 切換形狀時可順便重置變換矩陣
    for (int i = 0; i < 4; i++) {
      tetrahedron_verts[i][0] = default_tetrahedron_vertices[i][0];
      tetrahedron_verts[i][1] = default_tetrahedron_vertices[i][1];
      tetrahedron_verts[i][2] = default_tetrahedron_vertices[i][2];
    }
    break;

  case GLFW_KEY_F2:
    glfwSetWindowTitle(window, "F2: add a cube");
    current_shape = 2;
    transformMat = mat4x4(1);
    for (int i = 0; i < 8; i++) {
      cube_verts[i][0] = default_cube_vertices[i][0];
      cube_verts[i][1] = default_cube_vertices[i][1];
      cube_verts[i][2] = default_cube_vertices[i][2];
    }
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

  // translate +z / -z
  case GLFW_KEY_E:
    glfwSetWindowTitle(window, "translate +z");
    transformMat = swTranslate(0, 0, 1) * transformMat;
    break;
  case GLFW_KEY_D:
    glfwSetWindowTitle(window, "translate -z");
    transformMat = swTranslate(0, 0, -1) * transformMat;
    break;

  // 分別沿 X, Y, Z 軸縮放
  // X 軸縮放
  case GLFW_KEY_U:
    glfwSetWindowTitle(window, "scale +x");
    transformMat = swScale(1.1f, 1.0f, 1.0f) * transformMat;
    break;
  case GLFW_KEY_J:
    glfwSetWindowTitle(window, "scale -x");
    transformMat = swScale(0.9f, 1.0f, 1.0f) * transformMat;
    break;

  // Y 軸縮放
  case GLFW_KEY_I:
    glfwSetWindowTitle(window, "scale +y");
    transformMat = swScale(1.0f, 1.1f, 1.0f) * transformMat;
    break;
  case GLFW_KEY_K:
    glfwSetWindowTitle(window, "scale -y");
    transformMat = swScale(1.0f, 0.9f, 1.0f) * transformMat;
    break;

  // Z 軸縮放
  case GLFW_KEY_O:
    glfwSetWindowTitle(window, "scale +z");
    transformMat = swScale(1.0f, 1.0f, 1.1f) * transformMat;
    break;
  case GLFW_KEY_L:
    glfwSetWindowTitle(window, "scale -z");
    transformMat = swScale(1.0f, 1.0f, 0.9f) * transformMat;
    break;

  // rotate X
  case GLFW_KEY_R:
    glfwSetWindowTitle(window, "rotate +x");
    transformMat = swRotateX(5.0f) * transformMat;
    break;
  case GLFW_KEY_F:
    glfwSetWindowTitle(window, "rotate -x");
    transformMat = swRotateX(-5.0f) * transformMat;
    break;

  // rotate Y
  case GLFW_KEY_T:
    glfwSetWindowTitle(window, "rotate +y");
    transformMat = swRotateY(5.0f) * transformMat;
    break;
  case GLFW_KEY_G:
    glfwSetWindowTitle(window, "rotate -y");
    transformMat = swRotateY(-5.0f) * transformMat;
    break;

  // rotate Z
  case GLFW_KEY_Y:
    glfwSetWindowTitle(window, "rotate +z");
    transformMat = swRotateZ(5.0f) * transformMat;
    break;
  case GLFW_KEY_H:
    glfwSetWindowTitle(window, "rotate -z");
    transformMat = swRotateZ(-5.0f) * transformMat;
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
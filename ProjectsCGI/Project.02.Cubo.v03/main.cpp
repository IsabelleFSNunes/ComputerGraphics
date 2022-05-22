    /*

        Copyright 2010 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Tutorial 15 - Camera Rotation With Quaternions
*/

#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "camera.h"
#include "world_transform.h"

#define WINDOW_WIDTH  980
#define WINDOW_HEIGHT 720

struct Vertex {
    Vector3f pos;
    Vector3f color;

    Vertex() {}

    Vertex(float x, float y, float z)
    {
        pos = Vector3f(x, y, z);

        float red   = (float)rand() / (float)RAND_MAX;
        float green = (float)rand() / (float)RAND_MAX;
        float blue  = (float)rand() / (float)RAND_MAX;
        color = Vector3f(red, green, blue);
    }

    Vertex(float x, float y, float z, float red, float green, float blue)
    {
        pos = Vector3f(x, y, z);
        color = Vector3f(red, green, blue);
    }
};



GLuint VAO;             // Vertex Array Object
GLuint VBO;             // Vertex Buffer Object
GLuint IBO;             // Index Buffer Object
GLuint gWVPLocation;

WorldTrans CubeWorldTransform;
Vector3f CameraPos(0.0f, 0.0f, -1.0f);
Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
Vector3f CameraUp(0.0f, 1.0f, 0.0f);
Camera GameCamera(WINDOW_WIDTH, WINDOW_HEIGHT, CameraPos, CameraTarget, CameraUp);

float FOV = 45.0f;
float zNear = 1.0f;
float zFar = 100.0f;
PersProjInfo PersProjInfo = { FOV, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, zNear, zFar };

// Declarations of external files 
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

// -------------- Methods implemented ------------------------
static void CreateVertexBuffer();
static void CreateIndexBuffer();

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
static void CompileShaders();
static void RenderSceneCB();

static void SpecialKeyboardCB(int key, int mouse_x, int mouse_y);
static void KeyboardCB(unsigned char key, int mouse_x, int mouse_y);
static void PassiveMouseCB(int x, int y);
static void InitializeGlutCallbacks();

// -----------------------------------------------------------


int main(int argc, char** argv)
{
#ifdef _WIN64
    srand(GetCurrentProcessId());
#else
    srandom(getpid());
#endif

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    int x = 200;
    int y = 100;
    glutInitWindowPosition(x, y);
    int win = glutCreateWindow("Project 02 - cube with mouse moviments");
    printf("window id: %d\n", win);

    char game_mode_string[64];
    // Game mode string example: <Width>x<Height>@<FPS>
    snprintf(game_mode_string, sizeof(game_mode_string), "%dx%d@60", WINDOW_WIDTH, WINDOW_HEIGHT);
    glutGameModeString(game_mode_string);
    glutEnterGameMode();

    InitializeGlutCallbacks();

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
    glClearColor(Red, Green, Blue, Alpha);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_FRONT);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);

    CreateVertexBuffer();
    CreateIndexBuffer();

    CompileShaders();

    glutMainLoop();

    return 0;
}


static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GameCamera.OnRender();

#ifdef _WIN64
    float YRotationAngle = 0.1f;
#else
    float YRotationAngle = 1.0f;
#endif

    CubeWorldTransform.SetPosition(0.0f, 0.0f, 2.0f);
    CubeWorldTransform.Rotate(0.01f, YRotationAngle, 0.00f);
    Matrix4f World = CubeWorldTransform.GetMatrix();

    Matrix4f View = GameCamera.GetMatrix();

    Matrix4f Projection;
    Projection.InitPersProjTransform(PersProjInfo);

    Matrix4f WVP = Projection * View * World;

    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, &WVP.m[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glutPostRedisplay();

    glutSwapBuffers();
}

static void KeyboardCB(unsigned char key, int mouse_x, int mouse_y)
{
    if (key == 'q' || key == 27) {
        exit(0);
    }

    GameCamera.OnKeyboard(key);
}


static void SpecialKeyboardCB(int key, int mouse_x, int mouse_y)
{
    GameCamera.OnKeyboard(key);
}


static void PassiveMouseCB(int x, int y)
{
    GameCamera.OnMouse(x, y);
}


static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    glutKeyboardFunc(KeyboardCB);
    glutSpecialFunc(SpecialKeyboardCB);
    glutPassiveMotionFunc(PassiveMouseCB);
}


static void CreateVertexBuffer()
{
    
    //Create an object that stores all of the state needed to suppl vertex data
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    Vertex Vertices[8];
    /*
    *      p7------------- p6
    *    /  |             / |
    *   p3 ------------ p2  |
    *   |   |           |   |
    *   |   |           |   |
    *   |   p4--------- | - |
    *   |  /            | /  p5
    *   p0 ------------ p1 
    */
    float alpha_linear = 0.5; 
    Vertices[0] = Vertex(-1.0f * alpha_linear, -1.0f * alpha_linear, 1.0f * alpha_linear,
                            1.0f, 0.0f, 0.0f);   // face - frente p0
    Vertices[1] = Vertex( 1.0f * alpha_linear, -1.0f * alpha_linear, 1.0f * alpha_linear,
                            1.0f,  0.0f, 0.0f);    // face - frente p1
    Vertices[2] = Vertex( 1.0f * alpha_linear,  1.0f * alpha_linear, 1.0f * alpha_linear,
                            1.0f,  0.0f, 0.0f);     // face - frente p2
    Vertices[3] = Vertex(-1.0f * alpha_linear,  1.0f * alpha_linear, 1.0f * alpha_linear,
                            1.0f,  0.0f, 0.0f);    // face - frente p3
                
    Vertices[4] = Vertex(-1.0f * alpha_linear, -1.0f * alpha_linear, -1.0f * alpha_linear,
                            1.0f,  1.0f, 1.0f);   // face - tr�s p4
    Vertices[5] = Vertex( 1.0f * alpha_linear, -1.0f * alpha_linear, -1.0f * alpha_linear,
                            1.0f,  1.0f, 1.0f);    // face - tr�s p5
    Vertices[6] = Vertex( 1.0f * alpha_linear,  1.0f * alpha_linear, -1.0f * alpha_linear,
                            1.0f,  1.0f, 1.0f);     // face - tr�s p6
    Vertices[7] = Vertex(-1.0f * alpha_linear,  1.0f * alpha_linear, -1.0f * alpha_linear,
                            1.0f,  1.0f, 1.0f);    // face -tr�s  p7


    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
  unsigned int Indices[] = {   
                                2,3,0,  // frente
                                2,0,1,

                                7,4,3, // lado esq
                                3,4,0,

                                6,5,4,  // tras
                                7,6,4,

                                6,2,5,  //lado dir
                                2,1,5,

                                0,4,5, // baixo
                                1,0,5,

                                3,6,7, // cima
                                3,2,6
                            };

  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(1);
    }

    const GLchar* p[1];
    p[0] = pShaderText;

    GLint Lengths[1];
    Lengths[0] = (GLint)strlen(pShaderText);

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}


static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    std::string vs, fs;

    if (!ReadFile(pVSFileName, vs)) {
        exit(1);
    };

    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

    if (!ReadFile(pFSFileName, fs)) {
        exit(1);
    };

    AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram);

    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVP");
    if (gWVPLocation == -1) {
        printf("Error getting uniform location of 'gWVP'\n");
        exit(1);
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);
}

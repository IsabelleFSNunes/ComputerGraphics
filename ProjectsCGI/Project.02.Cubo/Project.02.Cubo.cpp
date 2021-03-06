/*
* by: @IsabelleFSNunes.   
*/

#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"

// Initializing some global variables

//  connections between local objects on CPU with GPU format
GLuint VAO;             // Vertex Array Object
GLuint VBO;             // Vertex Buffer Object
GLuint IBO;             // Index Buffer Object
GLuint gWorldLocation;
GLint  gScaleLocation;

// Declarations of external files 
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

// -------------- Methods implemented ------------------------
static void CreateVertexBuffer();
static void CreateIndexBuffer();

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
static void CompileShaders();
static void RenderSceneCB();
// -----------------------------------------------------------

// Main program ----------------------------------------------
int main(int argc, char** argv)
{

// used by random color of cube

#ifdef _WIN64
    srand(GetCurrentProcessId());
#else
    srandom(getpid());
#endif

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    int width = 1920;
    int height = 1080;
    glutInitWindowSize(width, height);

    int x = 200;
    int y = 100;
    glutInitWindowPosition(x, y);
    int win = glutCreateWindow("Project 02 - Cubo");
    printf("window id: %d\n", win);

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CW);
    //glCullFace(GL_BACK);

    CreateVertexBuffer();
    CreateIndexBuffer();
    
    CompileShaders();

    glutDisplayFunc(RenderSceneCB);

    glutMainLoop();

    return 0;
}


/* ------- Methods    ---------------------------------------*/

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);
    static float Scale = 0.0f;

#ifdef _WIN64
    Scale += 0.0005f;
#else
    Scale += 0.02f;
#endif

    Matrix4f Rotation(cosf(Scale), 0.0f, -sinf(Scale), 0.0f,
                      0.0f,        1.0f, 0.0f        , 0.0f,
                      sinf(Scale), 0.0f, cosf(Scale),  0.0f,
                      0.0f,        0.0f, 0.0f,         1.0f);

    Matrix4f Translation(1.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 2.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);

    float VFOV = 45.0f;
    float tanHalfVFOV = tanf(ToRadian(VFOV / 2.0f));
    float d = 1/tanHalfVFOV;

    float ar = (float)1920/ (float)1080;

    printf("Aspect ratio %f\n", ar);

    float NearZ = 1.0f;
    float FarZ = 100.0f;

    float zRange = NearZ - FarZ;

    float A = (-FarZ - NearZ) / zRange;
    float B = 0.5f * FarZ * NearZ / zRange;

    Matrix4f Projection(d/ar, 0.0f, 0.0f, 0.0f,
                        0.0f, d,    0.0f, 0.0f,
                        0.0f, 0.0f, A,    B,
                        0.0f, 0.0f, 1.0f, 0.0f);

    Matrix4f FinalMatrix = Projection * Translation * Rotation;

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &FinalMatrix.m[0][0]);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));


    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glutPostRedisplay();
    
    glutSwapBuffers();
}


static void CreateVertexBuffer()
{
    
    //Create an object that stores all of the state needed to suppl vertex data
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    Vector3f Vertices[8];
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
    Vertices[0] = Vector3f(-1.0f * alpha_linear, -1.0f * alpha_linear, 1.0f * alpha_linear);   // face - frente p0
    Vertices[1] = Vector3f(1.0f * alpha_linear, -1.0f * alpha_linear, 1.0f * alpha_linear);    // face - frente p1
    Vertices[2] = Vector3f(1.0f * alpha_linear, 1.0f * alpha_linear, 1.0f * alpha_linear);     // face - frente p2
    Vertices[3] = Vector3f(-1.0f * alpha_linear, 1.0f * alpha_linear, 1.0f * alpha_linear);    // face - frente p3

    Vertices[4] = Vector3f(-0.75f * alpha_linear, -0.75f * alpha_linear, -1.0f * alpha_linear);   // face - tr???s p4
    Vertices[5] = Vector3f(1.25f * alpha_linear, -0.75f * alpha_linear, -1.0f * alpha_linear);    // face - tr???s p5
    Vertices[6] = Vector3f(1.25f * alpha_linear, 0.75f * alpha_linear, -1.0f * alpha_linear);     // face - tr???s p6
    Vertices[7] = Vector3f(-0.75f * alpha_linear, 0.75f * alpha_linear, -1.0f * alpha_linear);    // face -tr???s  p7


    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
  unsigned int Indices[] = {    
                                0,2,1,      // face frente
                                0,3,2,
                                0,3,7,      // face lado esquerdo
                                0,7,4,
                                4,7,6,      // face de tras
                                4,6,5,      
                                5,6,2,      // face lado direito
                                5,2,1,
                                2,7,6,      // face de cima
                                2,3,7,
                                4,0,1,      // face de baixo
                                4,1,5

                                // 0, 2, 3,
                                // 0, 1, 2,
                                // 1, 6, 2,
                                // 1, 5, 6,
                                // 5, 7, 6,
                                // 5, 4, 7,
                                // 4, 3, 7,
                                // 4, 0, 3,
                                // 5, 1, 4,
                                // 1, 0, 4,
                                // 2, 3, 7,
                                // 2, 7, 6,
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

    gWorldLocation  = glGetUniformLocation(ShaderProgram, "gWorld");
    if (gWorldLocation  == -1) {
        printf("Error getting uniform location of 'gWorld'\n");
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


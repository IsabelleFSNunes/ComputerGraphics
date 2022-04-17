/*
* by: @IsabelleFSNunes.   
*/

#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "Polyedron.h"

// Initializing some global variables

//  connections between local objects on CPU with GPU format
GLuint VAO;             // Vertex Array Object
GLuint VBO;             // Vertex Buffer Object
GLuint IBO;             // Index Buffer Object
GLuint gWorldLocation;

// Declarations of external files 
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

// Declarations of global variables
const int NVERTICES = 12;
const int NELEMENTS = 1;
const int NINDEX = 60;

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
    int width = 1080;
    int height = 840;
    glutInitWindowSize(width, height);

    int x = 0.1*width;
    int y = 0.1*height;

    glutInitWindowPosition(x, y);
    int win = glutCreateWindow("Project 05 - Icosahedron");
    printf("window id: %d\n", win);

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

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

    
    static float Scale = 0.00f;
    static float ScaleY = 0.5f;
    static float ScaleZ = 0.0f;


#ifdef _WIN64
    ScaleY += 0.0005f;
#else
    ScaleY += 0.02f;
#endif
    Matrix4f RotationX(1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, cosf(Scale), sinf(Scale),
                        0.0f, 0.0f, -sinf(Scale), cosf(Scale),
                        0.0f, 0.0f, 0.0f, 1.0f);

    Matrix4f RotationY ( cosf(ScaleY), 0.0f, -sinf(ScaleY), 0.0f,
                         0.0f, 1.0f, 0.0f, 0.0f,
                        sinf(ScaleY), 0.0f, cosf(ScaleY), 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);

    Matrix4f RotationZ( cosf(ScaleZ), -sinf(ScaleZ), 0.0f, 0.0f,
                        sinf(ScaleZ), cosf(ScaleZ), 0.0f,0.0f,
                        0.0f,0.0f,1.0f,0.0f,
                        0.0f,0.0f,0.0f,1.0f);

    Matrix4f Translation(1.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 6.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);

    float VFOV = 45.0f;
    float tanHalfVFOV = tanf(ToRadian(VFOV / 2.0f));
    float d = 1/tanHalfVFOV;

    float ar = (float)1080 / (float)840;

    printf("Aspect ratio %f\n", ar);

    float NearZ = 1.0f;
    float FarZ = 100.0f;

    float zRange = NearZ - FarZ;

    float A = (-FarZ - NearZ) / zRange;
    float B = 6.0f * FarZ * NearZ / zRange;

    Matrix4f Projection(d/ar, 0.0f, 0.0f, 0.0f,
                        0.0f, d,    0.0f, 0.0f,
                        0.0f, 0.0f, A,    B,
                        0.0f, 0.0f, 1.0f, 0.0f);
    

    Matrix4f FinalMatrix = Projection * Translation * RotationX * RotationY;

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &FinalMatrix.m[0][0]);

  
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));


    glDrawElements(GL_LINE_LOOP, 60, GL_UNSIGNED_INT, 0);
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

    // Vector with connection with GPU
    Vector3f icosahedroVector[(NELEMENTS*NVERTICES)];        // icosahedron
   
    // Local variables to use in adjusts before send the datas to GPU
    Polyedron p(1); 
    Vertex *icosahedroLocalSort = p.Icosahedron();
    
    for(int i = 0; i < NVERTICES ; i++){       
        icosahedroVector[i] = Vector3f(icosahedroLocalSort[i].x, icosahedroLocalSort[i].y, icosahedroLocalSort[i].z );
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(icosahedroVector), icosahedroVector, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
  unsigned int Indices[] = {4,0,5,
                                4,10,0,
                                10,1,0,
                                0,1,8,
                                0,8,5,

                                8,9,5,
                                8,7,9,
                                1,7,8,
                                1,6,7,
                                6,3,7,

                                3,9,7,
                                3,2,9,
                                3,11,2,
                                11,4,2,
                                4,5,2,

                                2,5,9,
                                10,4,11,
                                10,11,6,
                                10,6,1,
                                3,6,11,
                                };
  //0,1,2,3, 4, 5,6,7, 8,9,10,11
  //4,5,6,7, 0, 1,2,3, 8,10,9,11 -- meu
//   unsigned int Indices[][3] = {{0,4,1},{0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
//    {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
//    {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
//    {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
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


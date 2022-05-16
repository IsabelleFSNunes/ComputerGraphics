/*
* by: @IsabelleFSNunes.   
*/

#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "Icosahedron.h"

// Initializing some global variables
#define ICOSAHEDRON 12


struct Vertex {
    float x;
    float y;
    float z;
    //Vector3f color;
};

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
const int NINDEX = 30;
//int Index[NINDEX];
//std::vector<int> Indices(30);

// -------------- Methods implemented ------------------------
static void CreateVertexBuffer();
static void CreateIndexBuffer();

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
static void CompileShaders();
static void RenderSceneCB();

void convertVectorVertices(std::vector<float> inputVector[3*NVERTICES], std::vector<Vertex> vectorVertices[NVERTICES]);
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

    // glEnable(GL_CULL_FACE);
    // // glFrontFace(GL_CCW);
    // glCullFace(GL_BACK);

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

    
    static float angle = 0.00f;
    static float angleY = 0.5f;
    static float angleZ = 0.5f;


#ifdef _WIN64
    angle += 0.0005f;
    angleY += 0.0005f;
    angleZ += 0.0005f;

#else
    angleY += 0.02f;
#endif
    Matrix4f RotationX(1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, cosf(angle), sinf(angle),
                        0.0f, 0.0f, -sinf(angle), cosf(angle),
                        0.0f, 0.0f, 0.0f, 1.0f);

    Matrix4f RotationY ( cosf(angleY), 0.0f, -sinf(angleY), 0.0f,
                         0.0f, 1.0f, 0.0f, 0.0f,
                        sinf(angleY), 0.0f, cosf(angleY), 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);

    Matrix4f RotationZ( cosf(angleZ), -sinf(angleZ), 0.0f, 0.0f,
                        sinf(angleZ), cosf(angleZ), 0.0f,0.0f,
                        0.0f,0.0f,1.0f,0.0f,
                        0.0f,0.0f,0.0f,1.0f);

    float scalar = 0.25f;
    Matrix4f Translation(scalar, 0.0f, 0.0f, 0.0f,
                         0.0f, scalar, 0.0f, 0.0f,
                         0.0f, 0.0f, scalar, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);

    // float VFOV = 45.0f;
    // float tanHalfVFOV = tanf(ToRadian(VFOV / 2.0f));
    // float d = 1/tanHalfVFOV;

    // float ar = (float)1080 / (float)840;

    // printf("Aspect ratio %f\n", ar);

    // float NearZ = 1.0f;
    // float FarZ = 100.0f;

    // float zRange = NearZ - FarZ;

    // float A = (-FarZ - NearZ) / zRange;
    // float B = 6.0f * FarZ * NearZ / zRange;

    // Matrix4f Projection(d/ar, 0.0f, 0.0f, 0.0f,
    //                     0.0f, d,    0.0f, 0.0f,
    //                     0.0f, 0.0f, A,    B,
    //                     0.0f, 0.0f, 1.0f, 0.0f);
    

    // Matrix4f FinalMatrix = Projection * Translation * RotationX * RotationY;
    // Matrix4f FinalMatrix = Translation * RotationX * RotationY;
    Matrix4f FinalMatrix = Translation * RotationX;

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &FinalMatrix.m[0][0]);

  
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    //glDrawElements(GL_LINES, 120, GL_UNSIGNED_INT, 0);
     glDrawElements(GL_TRIANGLES, 400, GL_UNSIGNED_INT, 0);

     // glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
     // glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
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
    
    float X = 0.525731112119133606;
    float Z = .850650808352039932;

    static Vertex vdata[NVERTICES] = {
       {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
       {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
       {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
    };

    for(int i = 0; i < NVERTICES ; i++){    
         icosahedroVector[i] = Vector3f(vdata[i].x, vdata[i].y, vdata[i].z );
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(icosahedroVector), icosahedroVector, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
    //Icosahedron icosahedron2(0.5f); // radius
    unsigned int Indices[60]{
               0,4,1, 0,9,4, 9,5,4, 4,5,8, 4,8,1,
               8,10,1, 8,3,10, 5,3,8, 5,2,3, 2,7,3,
               7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
               6,1,10, 9,0,11, 9,11,2, 9,2,5, 7,2,11 };


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

void convertVectorVertices(std::vector<float> inputVector[3*NVERTICES], std::vector<Vertex> vectorVertices[NVERTICES]){
    for(int i = 0; i < 36; i+=3){
        vectorVertices->at(i/3).x = inputVector->at(i);
        vectorVertices->at(i/3).y = inputVector->at(i+1);
        vectorVertices->at(i/3).z = inputVector->at(i+2);
    }
    printf("Conversao finalizada...\n");
}
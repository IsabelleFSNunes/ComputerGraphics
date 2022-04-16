/*
* by: @IsabelleFSNunes.   
*/

#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "Cuboide.h"

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

// Declarations of global variables
const int NVERTICES = 8;
const int NLEGS = 4;
const int NELEMENTS = 5;
const int NINDEX = 36;

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
    int win = glutCreateWindow("Project 04 - Simple Table");
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

    
    static float Scale = 0.01f;
    static float ScaleY = 2.5f;
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
                         0.0f, 0.0f, 1.0f, 4.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);

    float VFOV = 45.0f;
    float tanHalfVFOV = tanf(ToRadian(VFOV / 2.0f));
    float d = 1/tanHalfVFOV;

    float ar = (float)1080 / (float)840;

    printf("Aspect ratio %f\n", ar);

    float NearZ = 1.0f;
    float FarZ = 10.0f;

    float zRange = NearZ - FarZ;

    float A = (-FarZ - NearZ) / zRange;
    float B = 4.0f * FarZ * NearZ / zRange;

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


    glDrawElements(GL_TRIANGLES, NINDEX*NELEMENTS, GL_UNSIGNED_INT, 0);
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
    Vector3f Table[(NELEMENTS*NVERTICES)];
   
    // Local variables to use in adjusts before send the datas to GPU
    Vertex topTable[NVERTICES];
    Vertex legsTable[NLEGS][NVERTICES];
    Vertex origin;
    float lengthTop, heigthTop, depthTop;
    float length, heigth, depth;


    lengthTop = 1.0; heigthTop = 0.1; depthTop = 1.0;
    Cuboide Top(lengthTop, heigthTop, depthTop);
    origin.x = 0; origin.y = 0; origin.z = 0;
    Top.Dimensions(topTable, origin);
    

    // restart the variables 
    length = 0.1,  heigth = 0.8,  depth = 0.1;
    Cuboide leg1Table(length, heigth, depth);
    Cuboide leg2Table(length, heigth, depth);
    Cuboide leg3Table(length, heigth, depth);
    Cuboide leg4Table(length, heigth, depth);

    origin.x = topTable[0].x - length/2;  // 0.05 - 0.1/2
    origin.y = topTable[0].y - heigth/2;  // 0.05 - 1.0/2 = 0
    origin.z = topTable[0].z - depth /2;   // 0.5 - 0.1/2
    leg1Table.Dimensions(legsTable[0], origin);

    origin.x = topTable[2].x + length/2; 
    origin.y = topTable[2].y - heigth/2;
    origin.z = topTable[2].z - depth/2;
    leg2Table.Dimensions(legsTable[1], origin);
    
    origin.x = topTable[4].x - length/2; 
    origin.y = topTable[4].y - heigth/2;
    origin.z = topTable[4].z + depth/2;
    leg3Table.Dimensions(legsTable[2], origin);
    
    origin.x = topTable[6].x + length/2; 
    origin.y = topTable[6].y - heigth/2;
    origin.z = topTable[6].z + depth/2;
    leg4Table.Dimensions(legsTable[3], origin);
    
    for(int i = 0; i < NVERTICES ; i++){       
        Table[i] = Vector3f(topTable[i].x, topTable[i].y, topTable[i].z );
        for(int j = 0; j < NLEGS; j++){
            Table[i + (j+1)*NVERTICES] = Vector3f(legsTable[j][i].x, legsTable[j][i].y, legsTable[j][i].z);
        }
        // printf("Table[%d] => x = %f y = %f e z = %f\t", i, Table[i].x, Table[i].y, Table[i].z);
        // printf("Table[%d] => x = %f y = %f e z = %f\n", i+8, Table[i+8].x, Table[i+8].y, Table[i+8].z);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(Table), Table, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
  unsigned int Indices[] = {    
                             4,7,6,
                             7,1,3,
                             5,0,1,
                             2,1,0,
                             6,3,2,
                             4,2,0,
                             4,5,7,
                             7,5,1,
                             5,4,0,
                             2,3,1,
                             6,7,3,
                             4,6,2,
                            };
    unsigned int Indices2[NELEMENTS*NINDEX];
    


    for (int i = 0; i < NELEMENTS; i++)
    {
        for (int j = 0; j < NINDEX; j++)
        {
            if(i == 0)
                Indices2[j] =  Indices[j];
            else
                Indices2[i*NINDEX + j] =  Indices[j] + i*NVERTICES;
        }
        
    }
    
    // printf("Indices ------------ \n\n ");
    // for (int i = 0; i < 36; i++)
    // {
    //     printf("ListaCubo1[%d] = %d \t ListaCubo2[%i] = %d\n", i, Indices2[i], i+36, Indices2[i+36]);
    // }

  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2), Indices2, GL_STATIC_DRAW);
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


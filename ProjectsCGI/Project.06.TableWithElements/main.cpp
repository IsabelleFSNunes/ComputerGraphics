/*
* by: @IsabelleFSNunes.   
*/

#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "Models.h"

// Initializing some global variables
#define ICOSAHEDRON 12

//  connections between local objects on CPU with GPU format
GLuint VAO;             // Vertex Array Object
GLuint VBO;             // Vertex Buffer Object
GLuint IBO;             // Index Buffer Object
GLuint gWorldLocation;

// Declarations of external files 
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

#define tam NELEMENTS_TABLE * NINDEX_CUBOID
int a;

// -------------- Methods implemented ------------------------
static void CreateVertexBuffer(Models m);
static void CreateIndexBuffer(Models m, int nShift);

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
static void CompileShaders();
static void RenderSceneCB();

// used by icosahedron model
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
    int win = glutCreateWindow("Project 06 - Table with other elements");
    printf("window id: %d\n", win);

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_CULL_FACE);
    //glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    Vertex originTable;
    originTable.x = 0.0; originTable.y = 0.0; originTable.z = 0.0;
    float c[] = {1.0, 0.0, 0.0, 0.0};
    Models table(originTable, c);
    CreateVertexBuffer(table);
    CreateIndexBuffer(table, 0);

    originTable.x = 2.0; originTable.y = 1.0; originTable.z = 0.5;
    //float c[] = {0.0, 0.0, 1.0, 0.0};
    Models table2(originTable, c);
    CreateVertexBuffer(table2);
    CreateIndexBuffer(table2, 40);
    
    CompileShaders();

    glutDisplayFunc(RenderSceneCB);

    glutMainLoop();

    return 0;
}


/* ------- Methods    ---------------------------------------*/

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    static float Scale = 0.01f;
    static float ScaleY = 2.5f;
    static float ScaleZ = 0.2f;


#ifdef _WIN64
    ScaleZ += 0.001f;
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
                         0.0f, 0.0f, 1.0f, -4.0f,
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
    
    // Matrix4f FinalMatrix = Projection * Translation * RotationX * RotationY * RotationZ;
    Matrix4f FinalMatrix =  Translation;

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &FinalMatrix.m[0][0]);

  
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    //tam =  NELEMENTS_TABLE * NINDEX_CUBOID;
    glDrawElements(GL_TRIANGLES, 2*tam, GL_UNSIGNED_INT, 0);

    //glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glutPostRedisplay();
    
    glutSwapBuffers();
}


static void CreateVertexBuffer(Models m)
{
    Vector3f tableArray[NELEMENTS_TABLE * NVERTICES_CUBOIDE]; 
    m.createTableBuffer(tableArray);

    //Create an object that stores all of the state needed to suppl vertex data
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(tableArray), 0, GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(m.Buffer), 0, GL_STATIC_DRAW);
    // // objects index separated
    glBufferSubData(GL_ARRAY_BUFFER, a, sizeof(tableArray), tableArray);

}

static void CreateIndexBuffer(Models m, int nShift)
{   
    int tableArray[NELEMENTS_TABLE * NINDEX_CUBOID];
    m.createTableIndices(tableArray);
    m.setIndices(nShift, sizeof(m.Indices));
    
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tableArray), tableArray, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, NELEMENTS_TABLE * NINDEX_CUBOID, tableArray);

//   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m.Indices), 0, GL_STATIC_DRAW);
//   // objects index separated
//   glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(m.Indices), m.Indices);
//   glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2) ,sizeof(IndicesIcosahedron), IndicesIcosahedron );

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
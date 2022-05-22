/*
* by: @IsabelleFSNunes.   
*/	

#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "camera.h"
#include "world_transform.h"
#include "ogldev_pipeline.h"
#include "ogldev_glut_backend.h"
#include "Models.h"

// Initializing some global variables

//  connections between local objects on CPU with GPU format
GLuint VAO;             // Vertex Array Object
GLuint VBO;             // Vertex Buffer Object
GLuint IBO;             // Index Buffer Object
GLuint gWVPLocation;

// Declarations of external files 
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

// To manipulate camera with mouse
Camera* pGameCamera;
PersProjInfo gPersProjInfo;

WorldTrans CubeWorldTransform;
Vector3f CameraPos(0.0f, 0.0f, -1.0f);
Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
Vector3f CameraUp(0.0f, 1.0f, 0.0f);
Camera GameCamera(WINDOW_WIDTH, WINDOW_HEIGHT, CameraPos, CameraTarget, CameraUp);

float FOV = 45.0f;
float zNear = 1.0f;
float zFar = 100.0f;
PersProjInfo PersProjInfo = { FOV, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, zNear, zFar };


#define tam NELEMENTS_TABLE * NINDEX_CUBOID
int a;

// -------------- Methods implemented ------------------------
static void CreateVertexBuffer(Models m);
static void CreateIndexBuffer(Models m, int nShift);

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
static void CompileShaders();
static void RenderSceneCB();

static void _SpecialKeyboardCB(int Key, int x, int y);
static void InitializeGlutCallbacks();

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

    int win = glutCreateWindow("Project 06. v2 - Table with icosahedron");
    printf("window id: %d\n", win);

    InitializeGlutCallbacks();

    Vector3f CameraPos(0.0f, 0.0f, -1.0f);
    Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
    Vector3f CameraUp(0.0f, 1.0f, 0.0f);
    Camera GameCamera(width, height, CameraPos, CameraTarget, CameraUp);    

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

    Vertex originTable;
    originTable.x = 0.0; originTable.y = 0.0; originTable.z = 0.0;
    float c[] = {1.0, 0.0, 0.0, 0.0};
    
    // Models table(originTable, c);
    // CreateVertexBuffer(table);
    // CreateIndexBuffer(table, 0);
    

    Models ico(originTable, c);
    CreateVertexBuffer(ico);
    CreateIndexBuffer(ico, 0);

    CompileShaders();

    gPersProjInfo.FOV = 60.0f;
    gPersProjInfo.Height = height;
    gPersProjInfo.Width = width;
    gPersProjInfo.zNear = 1.0f;
    gPersProjInfo.zFar = 100.0f;

    // glutDisplayFunc(RenderSceneCB);

    glutMainLoop();

    return 0;
}


/* ------- Methods    ---------------------------------------*/

static void _SpecialKeyboardCB(int Key, int x, int y)
{
    OGLDEV_KEY OgldevKey = GLUTKeyToOGLDEVKey(Key);
    pGameCamera->OnKeyboard(OgldevKey);
}

static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
    glutSpecialFunc(_SpecialKeyboardCB);
}


static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.1f;
#ifdef _WIN64
    Scale += 0.001f;
#else
    Scale += 0.02f;
#endif
    Vector3f CameraPos(0.0f, 0.0f, -1.0f);
    Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
    Vector3f CameraUp(0.0f, 1.0f, 0.0f);

    Pipeline p;
    p.Rotate(0.0f, Scale, 0.0f);
    p.WorldPos(0.0f, 0.0f, 3.0f);
    p.SetCamera(CameraPos, CameraTarget, CameraUp);
    p.SetPerspectiveProj(gPersProjInfo);

    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetWVPTrans());

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // glutPostRedisplay();
    
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

    string vs, fs;

    if (!ReadFile(pVSFileName, vs)) {
        exit(1);
    };

    if (!ReadFile(pFSFileName, fs)) {
        exit(1);
    };

    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
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

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);

    gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVP");
    assert(gWVPLocation != 0xFFFFFFFF);
}

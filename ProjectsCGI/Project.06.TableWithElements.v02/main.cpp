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
#include "Models.h"

#define WINDOW_WIDTH  980
#define WINDOW_HEIGHT 720

GLuint VAO;             // Vertex Array Object
GLuint VBO[3];             // Vertex Buffer Object
GLuint IBO[3];             // Index Buffer Object

GLuint gWVP;

int passive_mouse = 1;

WorldTrans WorldTransform;
WorldTrans TableTransform;
WorldTrans IcoTransform; 
WorldTrans CubeTransform; 

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
static void CreateVertexBuffer(Vertex *array, int tam, int n);
static void CreateIndexBuffer(int *Indices, int tam, int n);

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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    int width = 1080;
    int height = 840;
    glutInitWindowSize(width, height);

    int x = 0.1*width;
    int y = 0.1*height;

    glutInitWindowPosition(x, y);
    int win = glutCreateWindow("Project 06 - Table with other elements");
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

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);

    // Mesa ---------------------------------------------------------------------------------
    Vertex tableOrigin(0.0, 0.0, 0.0);

    int obj = 1;
    float c2[] = { 0.57, 0.28, 0.15 };
    Models table(tableOrigin, c2);
    Vertex arrayTable[NELEMENTS_TABLE * NVERTICES_CUBOIDE];
    int arrayTableIndex[NELEMENTS_TABLE * NINDEX_CUBOID];
    table.createTableBuffer(arrayTable);
    for (int i = 0; i < NELEMENTS_TABLE * NVERTICES_CUBOIDE; i++)
        arrayTable[i] = table.Buffer[i];
    CreateVertexBuffer(arrayTable, NELEMENTS_TABLE * NVERTICES_CUBOIDE, obj);
    table.createTableIndices(arrayTableIndex);
    CreateIndexBuffer(table.Indices, NELEMENTS_TABLE * NINDEX_CUBOID, obj);

    // Icosaedro-----------------------------------------------------------------------------
    Vertex icoOrigin(0.0, 0.0, 0.0);
    obj = 2;
    float c[] = {-1, -1, -1};  // azul claro
    Models ico(icoOrigin, c);
    Vertex arrayIco[NVERTICES_ICO];
    int arrayIcoIndex[60];
    ico.createIcosahedroBuffer();
    for (int i = 0; i < NVERTICES_ICO; i++)
        arrayIco[i] = ico.Buffer[i];
    CreateVertexBuffer(arrayIco, NVERTICES_ICO, obj);
    ico.createIcosahedroIndexFaces(arrayIcoIndex);
    CreateIndexBuffer(ico.Indices, 60, obj);


    // Cube -----------------------------------------------------------------------------
    Vertex cubeOrigin(0.0, 0.0, 0.0);
    obj = 3;
    float c3[] = {0.0, 0.0, 0.5};  // azul claro
    Models cube(cubeOrigin, c3);
    Vertex arrayCube[NVERTICES_CUBOIDE];
    int arrayCubeIndex[NINDEX_CUBOID];
    cube.createCubeBuffer(arrayCube, 0.35f);
    for (int i = 0; i < NVERTICES_CUBOIDE; i++)
        arrayCube[i] = cube.Buffer[i];
    CreateVertexBuffer(arrayCube, NVERTICES_CUBOIDE, obj);
    cube.createCubeIndices(arrayCubeIndex);
    CreateIndexBuffer(cube.Indices, NINDEX_CUBOID, obj);


    CompileShaders();

    glutMainLoop();

    return 0;
}


static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GameCamera.OnRender();

#ifdef _WIN64
    float YRotationAngle = 0.01f;
#else
    float YRotationAngle = 1.0f;
#endif

    WorldTransform.SetPosition(0.0f, 0.0f, 0.0f);
    WorldTransform.Rotate(0.0f, YRotationAngle, 0.00f);
    Matrix4f World = WorldTransform.GetMatrix();

    Matrix4f View = GameCamera.GetMatrix();

    Matrix4f Projection;
    Projection.InitPersProjTransform(PersProjInfo);

    // Matrix4f WVP = Projection * View * World;

    // glUniformMatrix4fv(gWVP, 1, GL_TRUE, &WVP.m[0][0]);

    
    // position
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[0]);

    glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  6 * sizeof(float), 0);
    
        TableTransform.SetPosition(0.0f, 0.0f, 0.0f);
        TableTransform.Rotate(0.0f, 0.0f, 0.0f);
        TableTransform.SetScale(0.8);
        Matrix4f TableTransformMatrix = TableTransform.GetMatrix();
        Matrix4f WVP = Projection * View * TableTransformMatrix*World;
        glUniformMatrix4fv(gWVP, 1, GL_TRUE, &WVP.m[0][0]);
        
        // color
        glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        
        glDrawElements(GL_TRIANGLES, NELEMENTS_TABLE * NINDEX_CUBOID , GL_UNSIGNED_INT, 0); // table
    glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    /* ------------------------------------------------------------------------------------       */

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[1]);
    
    glEnableVertexAttribArray(0);
        glVertexAttribPointer(0 , 3, GL_FLOAT, GL_FALSE,  6 * sizeof(float), 0);
        IcoTransform.SetPosition(0.1f, 0.145f, 0.0f);
        IcoTransform.Rotate(0.0f, 0.0f, 0.0f);
        IcoTransform.SetScale(0.5);
        Matrix4f icoTransfMatrix = IcoTransform.GetMatrix();
        Matrix4f WVP2 =  Projection * View * icoTransfMatrix * World;
        glUniformMatrix4fv(gWVP, 1, GL_TRUE, &WVP2.m[0][0]);

        // color
        glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        
        glDrawElements(GL_TRIANGLES, 60 , GL_UNSIGNED_INT, 0); //
    glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    /* ------------------------------------------------------------------------------------       */

    /* ------------------------------------------------------------------------------------       */

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[2]);
    
    glEnableVertexAttribArray(0);
        glVertexAttribPointer(0 , 3, GL_FLOAT, GL_FALSE,  6 * sizeof(float), 0);
        CubeTransform.SetPosition(-0.2f, 0.128f, 0.0f);
        CubeTransform.Rotate(0.0f, 0.0f, 0.0f);
        CubeTransform.SetScale(0.5);
        Matrix4f cubeTransfMatrix = CubeTransform.GetMatrix();
        Matrix4f WVP3 = Projection * View * cubeTransfMatrix * World ;
        glUniformMatrix4fv(gWVP, 1, GL_TRUE, &WVP3.m[0][0]);

        // color
        glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        
        glDrawElements(GL_TRIANGLES, NINDEX_CUBOID , GL_UNSIGNED_INT, 0); //
    glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    /* ------------------------------------------------------------------------------------       */

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
    glutMotionFunc(PassiveMouseCB);
}


static void CreateVertexBuffer(Vertex *array, const int tam, int n)
{
    
    if(n == 1){
        Vertex arrayAux[NELEMENTS_TABLE * NVERTICES_CUBOIDE];
    
        for (int i = 0; i < tam; i++){
            arrayAux[i] = *array;
            array++;
        }

        //Create an object that stores all of the state needed to suppl vertex data
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(arrayAux), arrayAux, GL_STATIC_DRAW);
    }
    else if (n == 2){
        Vertex arrayAux[12];
    
        for (int i = 0; i < tam; i++){
            arrayAux[i] = *array;
            array++;
        }

        //Create an object that stores all of the state needed to suppl vertex data
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(arrayAux), arrayAux, GL_STATIC_DRAW);
    }
    else if (n == 3){
         Vertex arrayAux[NVERTICES_CUBOIDE];
    
        for (int i = 0; i < tam; i++){
            arrayAux[i] = *array;
            array++;
        }

        //Create an object that stores all of the state needed to suppl vertex data
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO[2]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(arrayAux), arrayAux, GL_STATIC_DRAW);
    }
}

static void CreateIndexBuffer(int *Indices, int tam, int n)
{
    if(n == 1){
        int arrayAux[NELEMENTS_TABLE * NINDEX_CUBOID];

        for (int i = 0; i < tam; i++) {
            arrayAux[i] = *Indices;
            Indices++;
        }
        glGenBuffers(1, &IBO[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[0]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrayAux), arrayAux, GL_STATIC_DRAW);
    }
    else if(n==2){
        int arrayAux[60];

        for (int i = 0; i < tam; i++) {
            arrayAux[i] = *Indices;
            Indices++;
        }
        glGenBuffers(2, &IBO[1]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrayAux), arrayAux, GL_STATIC_DRAW);
    }
    else if (n==3){
         int arrayAux[NINDEX_CUBOID];

        for (int i = 0; i < tam; i++) {
            arrayAux[i] = *Indices;
            Indices++;
        }
        glGenBuffers(2, &IBO[2]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrayAux), arrayAux, GL_STATIC_DRAW);
    }
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

    gWVP = glGetUniformLocation(ShaderProgram, "gWVP");
    if (gWVP == -1) {
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

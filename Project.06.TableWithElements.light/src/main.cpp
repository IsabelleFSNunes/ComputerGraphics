/* @IsabelleFSNunes */


#include <stdio.h>
#include <string.h>

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogldev_math_3d.h"
#include "../inc/camera.h"
#include "../inc/world_transform.h"
#include "../inc/Models.h"
#include "../inc/color.h"

#define WINDOW_WIDTH  980
#define WINDOW_HEIGHT 720

GLuint VAO;                // Vertex Array Object
GLuint VBO[3];             // Vertex Buffer Object
GLuint IBO[3];             // Index Buffer Object

GLuint gWVP;
GLuint projection, modelview, normalMat; 
GLuint mode, Ka, Kd, Ks, shininessVal; 
GLuint ambientColor, diffuseColor, specularColor, lightPos;

int var_mode = 0;

float var_Ka = 0.1f;
float var_Kd = 0.3f;
float var_Ks = 0.9f;

float var_shininessVal = 0.5f;

Vector3f var_ambienteColor(1.0f, 0.0f, 0.5f); 
Vector3f var_diffuseColor(1.0f, 1.0f, 0.5f);
Vector3f var_specularColor(1.0f, 1.0f, 1.0f);

Vector3f var_lightPos(0.5f, 0.0f, -0.5f);

Models table, cube, ico;

WorldTrans WorldTransform;
WorldTrans TableTransform;
WorldTrans IcoTransform; 
WorldTrans CubeTransform; 

Vector3f CameraPos(0.0f, 0.0f, -1.0f);
Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
Vector3f CameraUp(0.0f, 1.0f, 0.0f);
Camera GameCamera(WINDOW_WIDTH, WINDOW_HEIGHT, CameraPos, CameraTarget, CameraUp);

Matrix4f Projection(0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            
float FOV = 45.0f;
float zNear = 1.0f;
float zFar = 100.0f;
PersProjInfo persProjInfo = { FOV, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, zNear, zFar };
float posLeft, posRight, posTop, posBotton; 
OrthoProjInfo orthoProjInfo = { 2.0f, -2.0f, -2.0f, 2.0f, zNear, zFar };

// Declarations of external files 
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

// -------------- Methods implemented ------------------------
static void CreateVertexBuffer(Vertex *array, int tam, int n, bool is_update=false);
static void CreateIndexBuffer(int *Indices, int tam, int n);

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
static void CompileShaders();
static void RenderSceneCB();

void drawTable();
void drawCube();
void drawIcosahedron();

static void KeyboardCB(unsigned char key, int mouse_x, int mouse_y);
static void SpecialKeyboardCB(int key, int mouse_x, int mouse_y);
static void ScrollMouseClipping(int button, int dir, int x, int y);
static void PassiveMouseCB(int x, int y);
static void InitializeGlutCallbacks();
static void ShowMenu();
static void Menu(int id);

int updateColorHSVModel( Models inputModel, float value, char codColor);
int updateColorRGBModel(float value, char codColor, int op);
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
    int win = glutCreateWindow("Project 06 - Table, cube and icosahedron. [CLIPPING]");
    printf("window id: %d\n", win);

    char game_mode_string[64];
    // Game mode string example: <Width>x<Height>@<FPS>
    snprintf(game_mode_string, sizeof(game_mode_string), "%dx%d@60", WINDOW_WIDTH, WINDOW_HEIGHT);
    glutGameModeString(game_mode_string);
    glutEnterGameMode();

    InitializeGlutCallbacks();
    ShowMenu();

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

    drawTable();
    drawIcosahedron();
    drawCube();

    CompileShaders();

    glutMainLoop();

    return 0;
}




void drawTable(){
    // Mesa --------------------------------------------------------------------------------- 
    Vertex tableOrigin(0.0, 0.0, 0.0);
    int obj = 1;
    float cTable[] = { 0.57, 0.28, 0.15 }; // color
    table = Models(tableOrigin, cTable);
    Vertex arrayTable[NELEMENTS_TABLE * NVERTICES_CUBOIDE*3];
    int arrayTableIndex[NELEMENTS_TABLE * NINDEX_CUBOID];
    table.createTableBuffer(arrayTable);
    for (int i = 0; i < NELEMENTS_TABLE * NVERTICES_CUBOIDE*3; i++)
        arrayTable[i] = table.Buffer[i];
    CreateVertexBuffer(arrayTable, NELEMENTS_TABLE * NVERTICES_CUBOIDE*3, obj, false);
    table.createTableIndices(arrayTableIndex);
    CreateIndexBuffer(table.Indices, NELEMENTS_TABLE * NINDEX_CUBOID, obj);
    table.setNormalVectorPoints();
}

void drawIcosahedron(){
    // Icosaedro-----------------------------------------------------------------------------
    Vertex icoOrigin(0.0, 0.0, 0.0);
    float cIco[] = {-1, -1, -1};  // random color
    int obj = 2;
    ico = Models(icoOrigin, cIco);
    Vertex arrayIco[NVERTICES_ICO];
    int arrayIcoIndex[60];
    ico.createIcosahedroBuffer();
    for (int i = 0; i < NVERTICES_ICO; i++)
        arrayIco[i] = ico.Buffer[i];
    CreateVertexBuffer(arrayIco, NVERTICES_ICO, obj, false);
    ico.createIcosahedroIndexFaces(arrayIcoIndex);
    CreateIndexBuffer(ico.Indices, 60, obj);

}

void drawCube(){
    // Cube -----------------------------------------------------------------------------
    Vertex cubeOrigin(0.0, 0.0, 0.0);
    float cCube[] = {-1.0f, 0.0f, 0.5f};  // azul claro
    int obj = 3;
    cube = Models(cubeOrigin, cCube);
    Vertex arrayCube[NVERTICES_CUBOIDE * 3];
    int arrayCubeIndex[NINDEX_CUBOID];
    cube.createCubeBuffer(arrayCube, 0.35f);
    for (int i = 0; i < cube.sizeBuffer ; i++)
        arrayCube[i] = cube.Buffer[i];
    CreateVertexBuffer(arrayCube, cube.sizeBuffer , cube.id, false);
    cube.createCubeIndices(arrayCubeIndex);
    CreateIndexBuffer(cube.Indices, NINDEX_CUBOID, cube.id);
    cube.setNormalVectorPoints();
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

    // WorldTransform.SetPosition(0.0f, 0.0f, 0.0f);
    // WorldTransform.Rotate(0.0f, YRotationAngle, 0.0f);
    // Matrix4f World = WorldTransform.GetMatrix();


    // Matrix4f View = GameCamera.GetMatrix();

    // Matrix4f var_projection = Projection;
    // Matrix4f var_modelview =  View * World;

    // // glUniformMatrix4fv(gWVP, 1, GL_TRUE, &WVP.m[0][0]);
    // glUniformMatrix4fv(projection, 1, GL_TRUE, &var_projection.m[0][0]);
    // glUniformMatrix4fv(modelview, 1, GL_TRUE, &var_modelview.m[0][0]);

    glUniform1f(Ka, var_Ka);
    glUniform1f(Ks, var_Ks);
    glUniform1f(Kd, var_Kd);

    glUniform1d(mode, var_mode);
    
    //  var_ambienteColor = Vector3f(1.0f, 0.0f, 0.5f); 
    // glUniform3f(ambientColor, var_ambienteColor.x, var_ambienteColor.y, var_ambienteColor.z);
    glUniform3f(diffuseColor, var_diffuseColor.x, var_diffuseColor.y, var_diffuseColor.z);
    glUniform3f(specularColor, var_specularColor.x, var_specularColor.y, var_specularColor.z);

    glUniform3f(lightPos, var_lightPos.x, var_lightPos.y, var_lightPos.z);
    
    glUniform1f(shininessVal, var_shininessVal);


     /* ------------------------------------------------------------------------------------       */
    // TABLE
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[0]);

    glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  9 * sizeof(float), 0);
        
         var_ambienteColor = Vector3f(1.0f, 0.0f, 0.5f); 
        glUniform3f(ambientColor, var_ambienteColor.x, var_ambienteColor.y, var_ambienteColor.z);
        
        TableTransform.Rotate( 0.0f, YRotationAngle, 0.0f);
        TableTransform.SetPosition(0.0f, 0.0f, 0.0f);
        TableTransform.SetScale(0.8);
        Matrix4f TableTransformMatrix = TableTransform.GetMatrix();
        Matrix4f m_modelview = /*View */ TableTransformMatrix;
        Matrix4f m_projection = Projection;
        Matrix4f m_normalMat = (m_projection*m_modelview).Transpose();


        //printf("Reder Scene %d\n", teste +=1);
        glUniformMatrix4fv(modelview, 1, GL_TRUE, &m_modelview.m[0][0]);
        glUniformMatrix4fv(projection, 1, GL_TRUE, &m_projection.m[0][0]);
        glUniformMatrix4fv(normalMat, 1, GL_TRUE, &m_normalMat.m[0][0]);
        
        // color
        glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        
        // normal
        glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        
        glDrawElements(GL_TRIANGLES, NELEMENTS_TABLE * NINDEX_CUBOID , GL_UNSIGNED_INT, 0); // table
    glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

    /* ------------------------------------------------------------------------------------       */
    // ICOSAHEDRON
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[1]);
    
    glEnableVertexAttribArray(0);
        glVertexAttribPointer(0 , 3, GL_FLOAT, GL_FALSE,  9 * sizeof(float), 0);
         var_ambienteColor = Vector3f(0.0f, 1.0f, 0.5f); 
        glUniform3f(ambientColor, var_ambienteColor.x, var_ambienteColor.y, var_ambienteColor.z);
        IcoTransform.SetPosition(0.1f, 0.145f, 0.0f);
        //IcoTransform.Rotate(0.0f, 0.0f, 0.0f);
        IcoTransform.SetScale(0.5);
        Matrix4f icoTransfMatrix = IcoTransform.GetMatrix();
    
        Matrix4f m_modelview2 = /* View *  World*/TableTransformMatrix*icoTransfMatrix;
        Matrix4f m_projection2 = Projection;
        Matrix4f m_normalMat2 = (m_projection2 * m_modelview2).Transpose(); 

        //printf("Reder Scene %d\n", teste +=1);
        glUniformMatrix4fv(modelview, 1, GL_TRUE, &m_modelview2.m[0][0]);
        glUniformMatrix4fv(projection, 1, GL_TRUE, &m_projection2.m[0][0]);
        glUniformMatrix4fv(normalMat, 1, GL_TRUE, &m_normalMat2.m[0][0]);


        // color
        glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        
        // // normal
        // glEnableVertexAttribArray(2);
        //     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        
        glDrawElements(GL_TRIANGLES, 60 , GL_UNSIGNED_INT, 0); //
    glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        // glDisableVertexAttribArray(2);
        
    /* ------------------------------------------------------------------------------------       */

    /* ------------------------------------------------------------------------------------       */
    // CUBE 
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[2]);
    
    glEnableVertexAttribArray(0);
        glVertexAttribPointer(0 , 3, GL_FLOAT, GL_FALSE,  9 * sizeof(float), 0);
        var_ambienteColor = Vector3f(0.0f, 0.0f, 1.0f); 
        glUniform3f(ambientColor, var_ambienteColor.x, var_ambienteColor.y, var_ambienteColor.z);
        
        CubeTransform.SetPosition(-0.2f, 0.128f, 0.0f);
        //CubeTransform.Rotate(0.0f, 0.0f, 0.0f);
        CubeTransform.SetScale(0.5);
        Matrix4f cubeTransfMatrix = CubeTransform.GetMatrix();

        Matrix4f m_modelview3 =/* View * World*/TableTransformMatrix* cubeTransfMatrix;
        //Matrix4f m_projection3 = Projection;
        Matrix4f m_normalMat3 = (/*m_projection3 */ m_modelview3).Transpose(); 

        glUniformMatrix4fv(modelview, 1, GL_TRUE, &m_modelview3.m[0][0]);
        //glUniformMatrix4fv(projection, 1, GL_TRUE, &m_projection3.m[0][0]);
        glUniformMatrix4fv(normalMat, 1, GL_TRUE, &m_normalMat3.m[0][0]);

        // color
        glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        
        // normal
        glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        
        glDrawElements(GL_TRIANGLES, NINDEX_CUBOID , GL_UNSIGNED_INT, 0); //
    glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

    /* ------------------------------------------------------------------------------------       */

    glutPostRedisplay();

    glutSwapBuffers();
}

static void CreateVertexBuffer(Vertex* array, int tam, int n, bool is_update )
{
    
    if(n == 1){
        Vertex arrayAux[NELEMENTS_TABLE * NVERTICES_CUBOIDE*3];
    
        for (int i = 0; i < tam; i++){
            arrayAux[i] = *array;
            array++;
        }
        if(! is_update){
            //Create an object that stores all of the state needed to suppl vertex data
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
        }
        
        glGenBuffers(1, &VBO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(arrayAux), arrayAux,  GL_DYNAMIC_DRAW);
    }
    else if (n == 2){
        Vertex arrayAux[12];
    
        for (int i = 0; i < tam; i++){
            arrayAux[i] = *array;
            array++;
        }

         if(! is_update){
            //Create an object that stores all of the state needed to suppl vertex data
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
        }
        glGenBuffers(1, &VBO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(arrayAux), arrayAux,  GL_DYNAMIC_DRAW);
    }
    else if (n == 3){
         
        Vertex arrayAux[NVERTICES_CUBOIDE * 3];
    
        for (int i = 0; i < tam; i++){
            arrayAux[i] = *array;
            array++;
        }

        if(! is_update){
            //Create an object that stores all of the state needed to suppl vertex data
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
        }

        glGenBuffers(1, &VBO[2]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(arrayAux), arrayAux,  GL_DYNAMIC_DRAW);
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrayAux), arrayAux, GL_DYNAMIC_DRAW);
    }
    else if(n==2){
        int arrayAux[60];

        for (int i = 0; i < tam; i++) {
            arrayAux[i] = *Indices;
            Indices++;
        }
        glGenBuffers(1, &IBO[1]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrayAux), arrayAux,  GL_DYNAMIC_DRAW);
    }
    else if (n==3){
         int arrayAux[NINDEX_CUBOID];

        for (int i = 0; i < tam; i++) {
            arrayAux[i] = *Indices;
            Indices++;
        }
        glGenBuffers(1, &IBO[2]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrayAux), arrayAux,  GL_DYNAMIC_DRAW);
    }
}

static void SpecialKeyboardCB(int key, int mouse_x, int mouse_y)
{
    GameCamera.OnKeyboard(key);
}

void printColor(rgb rcolor, hsv hcolor = {0.0, 0.0, 0.0}){
    std::cout << "-------------------------------------------\n" << endl; 
    std::cout << "rgb.R =" << rcolor.r << "\t rgb.G = " << rcolor.g << "\t rgb.B =" << rcolor.b << endl;
    std::cout << "hsv.H =" << hcolor.h << "\t hsv.S =" << hcolor.s << "\t hsv.V =" << hcolor.v  << endl;
    std::cout << "-------------------------------------------\n" << endl;

}

int updateColorHSVModel( Models inputModel, float value, char codColor){
    Vector3f colorBuffer;
    rgb color_in;
    hsv color_out;

    for (int i = 0; i < inputModel.sizeBuffer; i++){
        colorBuffer = inputModel.Buffer[i].color;
        color_in = {(double)colorBuffer[0], (double)colorBuffer[1], (double)colorBuffer[2]};
        color_out = rgb2hsv(color_in);

        switch (codColor){
        case 'h':
            color_out.h += (double) value;
            if(color_out.h >= 1.0) color_out.h = 0.0;
            if(color_out.h <= 0.0) color_out.h = 1.0; 
            break;
        case 's':
            color_out.s += (double) value;
            if(color_out.s >= 1.0) color_out.s = 0.0;
            if(color_out.s <= 0.0) color_out.s = 1.0; 
            break;
        case 'v':
            color_out.v += (double) value;
            if(color_out.v >= 1.0) color_out.v = 0.0;
            if(color_out.v <= 0.0) color_out.v = 1.0; 
            break;
        default:
            printf("DEFAULT\n\n");
            return -1;
        }

        colorBuffer.x = (float)hsv2rgb(color_out).r;
        colorBuffer.y = (float)hsv2rgb(color_out).g; 
        colorBuffer.z = (float)hsv2rgb(color_out).b;

    }
    printColor(color_in, color_out);   

    return 0;
}

int updateColorRGBModel(float value, char codColor, int op){
    Vector3f actualColor; 
    Models inputModel;
    switch (op)
    {
    case 1:
        inputModel = table;
        break;
    case 2:
        inputModel = ico;
        break;
    case 3:
        inputModel = cube;
        break;
    default:
        return 0;
        break;
    }

    for (int i = 0; i < inputModel.sizeBuffer; i++){
        actualColor = inputModel.Buffer[i].color;
        switch (codColor)
        {
        case 'r':
            actualColor.x += value;
            if(actualColor.x >= 1.0) actualColor.x = 0.0;
            if(actualColor.x <= 0.0) actualColor.x = 1.0;
            break;
        case 'g':
            actualColor.y += value;
            if(actualColor.y >= 1.0) actualColor.y = 0.0;
            if(actualColor.y <= 0.0) actualColor.y = 1.0;
            break;
        case 'b':
            actualColor.z += value;
            if(actualColor.z >= 1.0) actualColor.z = 0.0;
            if(actualColor.z <= 0.0) actualColor.z = 1.0;
            break;
        default:
            return 0;
            break;
        }

        inputModel.Buffer[i].color = actualColor;
    }

    return 0;

}
int op = 0;
static void KeyboardCB(unsigned char key, int mouse_x, int mouse_y)
{   
    float value = 0.1f;
    float valueRGB = 0.1f;
    bool updateBuffer = true;
    bool keyChangeColor = false; 
    
    switch (key)
    {
    case 'q':
        exit(0);
        break;
    
    case 'h':
        updateColorHSVModel(table, +value, 'h'); 
        updateColorHSVModel(cube, +value, 'h'); 
        updateColorHSVModel(ico, +value, 'h'); 
        keyChangeColor = true;
        break;
    case 'H':
        updateColorHSVModel(table, -value, 'h'); 
        updateColorHSVModel(cube, -value, 'h');
        updateColorHSVModel(ico, -value, 'h'); 
        keyChangeColor = true;
        break;

    case 's':
        updateColorHSVModel(table, +value, 's'); 
        updateColorHSVModel(cube, +value, 's'); 
        updateColorHSVModel(ico, +value, 's'); 
        keyChangeColor = true;
        break;
        
    case 'S':
        updateColorHSVModel(table, -value, 's'); 
        updateColorHSVModel(cube, -value, 's'); 
        updateColorHSVModel(ico, -value, 's'); 
        keyChangeColor = true;
        break;

    case 'l':
        updateColorHSVModel(table, +value, 'v'); 
        updateColorHSVModel(cube, +value, 'v'); 
        updateColorHSVModel(ico, +value, 'v'); 
        keyChangeColor = true;
        break;
    
    case 'L':
        updateColorHSVModel(table, -value, 'v'); 
        updateColorHSVModel(cube, -value, 'v'); 
        updateColorHSVModel(ico, -value, 'v'); 
        keyChangeColor = true;
        break;
    /* ------------------------------------RGB -----------------------------*/
    case 'r':
        updateColorRGBModel(valueRGB, 'r', op);
        keyChangeColor = true;
        break;
    case 'R':
        updateColorRGBModel(-valueRGB, 'r', op);
        keyChangeColor = true;
        break;
    case 'g':
        updateColorRGBModel(valueRGB, 'g', op);
        keyChangeColor = true;
        break;
    case 'G':
        updateColorRGBModel(-valueRGB, 'g', op);
        keyChangeColor = true;
        break;
    case 'b':
        updateColorRGBModel(valueRGB, 'b', op);
        keyChangeColor = true;
        break;  
    case 'B':
        updateColorRGBModel(-valueRGB, 'b', op);
        keyChangeColor = true;
        break; 
    case 't': op = 1; break; 
    case 'i': op = 2; break;   
    case 'c': op = 3; break;   

    default:
        break;
    }

    if(keyChangeColor){
        CreateVertexBuffer(table.Buffer, table.sizeBuffer, table.id, updateBuffer); 
        CreateVertexBuffer(cube.Buffer, cube.sizeBuffer, cube.id, updateBuffer); 
        CreateVertexBuffer(ico.Buffer, ico.sizeBuffer, ico.id, updateBuffer); 

    }
    
    glutDisplayFunc(RenderSceneCB);
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
    glutMouseWheelFunc(ScrollMouseClipping);
}

static void ScrollMouseClipping(int button, int dir, int x, int y){
    /*printf("button = %d\n", button);
    printf("dir = %d\n", dir);
    printf("x = %d\n", x);
    printf("y = %d\n", y);*/

    GameCamera.ScrollMouse(dir);
}


static void ShowMenu(){
    glutCreateMenu(Menu);
    glutAttachMenu(GLUT_RIGHT_BUTTON); 

    glutAddMenuEntry("Sem Projecao",1);  
    glutAddMenuEntry("Projecao Perspectiva",2);  
    glutAddMenuEntry("Projecao Ortogonal", 3);
   /* glutAddMenuEntry("Change color", 4);
    glutAddSubMenu("Table", submenu1);
    glutAddSubMenu("Icosahedron", submenu1);
    glutAddSubMenu("Cube", submenu1);*/

    glutDisplayFunc(RenderSceneCB);


}

int idprev;
static void Menu(int id){
    
    switch(id)      
   {  
        case 1:
            printf("Option 1 selected\n"); 
            Projection = Matrix4f(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            GameCamera.OnKeyboard('r'); //reset
            if( idprev == 2)
                GameCamera.OnKeyboard('r'); //reset
            break;  
        case 2: 
            printf("Option 2 selected\n");
            Projection.SetZero();  
            Projection.InitPersProjTransform(persProjInfo); 
            if( idprev == 2)
                GameCamera.OnKeyboard('r'); //reset
            GameCamera.OnKeyboard('p');
            
            break;

        case 3:
            printf("Option 3 selected\n");
            Projection.SetZero();  
            Projection.InitOrthoProjTransform(orthoProjInfo); 
            if( idprev != 3){
                GameCamera.OnKeyboard('p');
                GameCamera.OnKeyboard('p');
            }
            break;

        idprev = id;
     }

     
    glutPostRedisplay();
    glutSwapBuffers();
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

    projection = glGetUniformLocation(ShaderProgram, "projection");
    if (projection == -1) {
        printf("Error getting uniform location of 'projection'\n");
        exit(1);
    }

    modelview = glGetUniformLocation(ShaderProgram, "modelview");
    if (modelview == -1) {
        printf("Error getting uniform location of 'modelview'\n");
        exit(1);
    }

    normalMat = glGetUniformLocation(ShaderProgram, "normalMat");
    if (normalMat == -1) {
        printf("Error getting uniform location of 'normalMat'\n");
        exit(1);
    }

    mode = glGetUniformLocation(ShaderProgram, "mode");
    if (mode == -1) {
        printf("Error getting uniform location of 'mode'\n");
        exit(1);
    }

    Ka = glGetUniformLocation(ShaderProgram, "Ka");
    if (Ka == -1) {
        printf("Error getting uniform location of 'Ka'\n");
        exit(1);
    }

    Kd = glGetUniformLocation(ShaderProgram, "Kd");
    if (Kd == -1) {
        printf("Error getting uniform location of 'Kd'\n");
        exit(1);
    }

    Ks = glGetUniformLocation(ShaderProgram, "Ks");
    if (Ks == -1) {
        printf("Error getting uniform location of 'Ks'\n");
        exit(1);
    }

    shininessVal = glGetUniformLocation(ShaderProgram, "shininessVal");
    if (shininessVal == -1) {
        printf("Error getting uniform location of 'shininessVal'\n");
        exit(1);
    }

    ambientColor = glGetUniformLocation(ShaderProgram, "ambientColor");
    if (ambientColor == -1) {
        printf("Error getting uniform location of 'ambientColor'\n");
        exit(1);
    }

    diffuseColor = glGetUniformLocation(ShaderProgram, "diffuseColor");
    if (diffuseColor == -1) {
        printf("Error getting uniform location of 'diffuseColor'\n");
        exit(1);
    }

    specularColor = glGetUniformLocation(ShaderProgram, "specularColor");
    if (specularColor == -1) {
        printf("Error getting uniform location of 'specularColor'\n");
        exit(1);
    }

    lightPos = glGetUniformLocation(ShaderProgram, "lightPos");
    if (lightPos == -1) {
        printf("Error getting uniform location of 'lightPos'\n");
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

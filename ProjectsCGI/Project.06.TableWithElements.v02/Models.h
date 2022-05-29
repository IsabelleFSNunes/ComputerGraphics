#pragma once
#include "ogldev_math_3d.h"
#include "Cuboide.h"
//#include "def.h"

// Declarations of global variables
const int NVERTICES_CUBOIDE = 8;
const int NELEMENTS_TABLE = 5;          // vertices de cada componente da mesa separadamente
const int NLEGS = 4;
const int NINDEX_CUBOID = 36;

const int NVERTICES_ICO = 12;
const int NELEMENTS_ICO = 1;
const int NINDEXICO = 30;

// Declarations of global variables
const int NVERTICES = 8;
const int NELEMENTS = 5;


class Models
{
public:
    Vertex *Buffer;
    int *Indices;
    Vertex originCenter;
    float color[3];

public:
    Models(Vertex origin, float color[3]);
    void createCubeBuffer(Vertex Cube[NVERTICES_CUBOIDE], float size);
    void createCubeIndices(int Indices2[NINDEX_CUBOID]);
    void createTableBuffer(Vertex Table[( NELEMENTS_TABLE * NVERTICES_CUBOIDE )]);
    void createTableIndices(int Indices2[NELEMENTS_TABLE * NINDEX_CUBOID]);
    void createIcosahedroBuffer();
    void createIcosahedroIndexLine(int Indices2[62]);
    void createIcosahedroIndexFaces(int Indices2[60]);
    void setIndices(int n, int tam);
};

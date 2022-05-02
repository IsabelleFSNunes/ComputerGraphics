#pragma once
#include "ogldev_math_3d.h"
#include "Cuboide.h"
#include "Icosahedron.h"
//#include "def.h"

// Declarations of global variables
const int NVERTICES_CUBOIDE = 8;
const int NELEMENTS_TABLE = 5;          // vertices de cada componente da mesa separadamente
const int NLEGS = 4;
const int NINDEX_CUBOID = 36;

const int NVERTICES_ICO = 12;
const int NELEMENTS_ICO = 5;
const int NINDEXICO = 30;

// Declarations of global variables
const int NVERTICES = 8;
const int NELEMENTS = 5;


class Models
{
public:
    Vector3f *Buffer;
    int *Indices;
    Vertex originCenter;
    float color[4];

public:
    Models(Vertex origin, float color[4], int choice);
    void createTableBuffer();
    void createTableIndices();
    void createIcosahedroIndexLine();
    void setIndices(int n, int tam);
};

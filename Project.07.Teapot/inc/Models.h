#pragma once
#ifndef MODELS_H
#define MODELS_H

#include "ogldev_math_3d.h"
#include "../inc/Cuboide.h"
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
    int sizeBuffer;
    int sizeIndices;
    int id;         // identificador do objeto  
    // 1 - table ; 2 - Icosahedron; 3 - Cube

public:
    Models();
    Models(int id);
    Models(Vertex origin, float color[3]);
    void createCubeBuffer(Vertex Cube[NVERTICES_CUBOIDE * 3], float size);
    void createCubeIndices(int Indices2[NINDEX_CUBOID]);
    void createTableBuffer(Vertex Table[( NELEMENTS_TABLE * NVERTICES_CUBOIDE )]);
    void createTableIndices(int Indices2[NELEMENTS_TABLE * NINDEX_CUBOID]);
    void createIcosahedroBuffer();
    void createIcosahedroIndexLine(int Indices2[62]);
    void createIcosahedroIndexFaces(int Indices2[60]);
    void setIndices(int n, int tam);
};
#endif
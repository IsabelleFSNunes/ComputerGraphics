#pragma once
#ifndef VERTEX_STRUCT_H
#define VERTEX_STRUCT_H

#include "ogldev_math_3d.h"
#include <stdlib.h>

struct Vertex {
    Vector3f pos;
    Vector3f color;
    Vector3f normal;

    Vertex() {}

    Vertex(Vector3f v){
        Vertex(v.x, v.y, v.z);
    }

    Vertex(Vector3f v, float cor[3]) {
        Vertex(v.x, v.y, v.z, cor[0], cor[1], cor[2]);
    }

    Vertex(Vector3f v, Vector3f cor){
        Vertex(v.x, v.y, v.z, cor);
    }

     Vertex(Vector3f v, Vector3f cor, Vector3f normal){
        Vertex(v.x, v.y, v.z, cor);
        this->normal = normal;
    }

    Vertex(float x, float y, float z)
    {
        pos = Vector3f(x, y, z);

        float red   = (float)rand() / (float)RAND_MAX;
        float green = (float)rand() / (float)RAND_MAX;
        float blue  = (float)rand() / (float)RAND_MAX;
        color = Vector3f(red, green, blue);
    }

    Vertex(float x, float y, float z, float red, float green, float blue)
    {
        pos = Vector3f(x, y, z);
        color = Vector3f(red, green, blue);
    }

    Vertex(float x, float y, float z, Vector3f cor)
    {
        pos = Vector3f(x, y, z);
        color = Vector3f(cor);
    }
};

#endif
#pragma once
#ifndef VERTEX_STRUCT_H
#define VERTEX_STRUCT_H

#include "ogldev_math_3d.h"
#include <stdlib.h>

struct Vertex {
    Vector3f pos;
    Vector3f color;

    Vertex() {}

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
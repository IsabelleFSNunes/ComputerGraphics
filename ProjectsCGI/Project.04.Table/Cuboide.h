#pragma once
#include <iostream>
#include <ogldev_math_3d.h>

#define N_VERTICES 8

struct Vertex {
    float x;
    float y;
    float z;
    //Vector3f color;
};


/*
*      p6------------- p4
*    /  |             / |
*   p2 ------------ p0  |
*   |   |           |   |
*   |   |           |   |
*   |   p7--------- | - |
*   |  /            | /  p5
*   p3 ------------ p1
*/
// Function to generate cuboide coordinates 


class Cuboide{       // The class
public:                        // Access specifier
    float lenght;
    float heigth;
    float depth;
   // float color[N_VERTICES];

    Cuboide(float lenght, float heigth, float depth) {
        this->lenght = lenght;
        this->depth = depth;
        this->heigth = heigth;
    };

    void Initialize(float l, float h, float d);

    void Dimensions(Vertex coordinates[N_VERTICES], Vertex origin);
};

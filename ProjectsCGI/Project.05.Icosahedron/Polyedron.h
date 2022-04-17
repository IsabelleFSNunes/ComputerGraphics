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

class Polyedron
{

public:
	Polyedron(int option)
	{
        if (option == 1) {
            Icosahedron();
        }
	}

    Vertex* Icosahedron();
};


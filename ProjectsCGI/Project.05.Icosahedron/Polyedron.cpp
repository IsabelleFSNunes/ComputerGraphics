#include "Polyedron.h"

const int NICOSAHEDRON = 12;

Vertex* Polyedron::Icosahedron()
{
    /*
    *
    *   (0,+-1, +-phi)
    *   (+-1, +-phi, 0)
    *   (+-phi, 0,+-1)
    *  
    *   ----------------
    *   0  +1  +phi
    *   0  +1  -phi
    *   0  -1  +phi
    *   0  -1  -phi
    * 
    *    +phi 0  +1         
    *    -phi 0  +1
    *    +phi 0  -1
    *    -phi 0  -1
    *
    *    +1  +phi  0
    *    +1  -phi  0
    *    -1  +phi  0
    *    -1  -phi  0
    *
    */

   
//    static GLfloat vdata[NVERTEX][3] = {    
//    {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
//    {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
//    {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 

    Vertex P[NICOSAHEDRON];
    const float phi = (1 + sqrt(5)) / 2;
    float signalOne = 1.0;
    float signalPhi = 1.0;

    
    for (int i = 0; i < 4; i++) {
        P[i].x = 0;
        
        if (i > 1)
            signalOne = -1.0;
        P[i].y = signalOne;

        signalPhi *= -1.0;
        P[i].z = signalPhi * phi;
    }
    
    signalOne *= -1.0;
    for (int i = 4; i < 8; i++) {
        signalPhi *= -1;
        P[i].x = signalPhi * phi;

        P[i].y = 0;

        if (i > 5)
            signalOne = -1.0;
        P[i].z = signalOne;
    }

    signalOne *= -1;
    for (int i = 8; i < 12; i++) {
        if (i > 9)
            signalOne = -1;
        P[i].x = signalOne;
        
        signalPhi *= -1;
        P[i].y = signalPhi * phi;

        P[i].z = 0;
    }

    return P;
}

#include "Cuboide.h"
using namespace std;


void Cuboide::Dimensions(Vertex coordinates[N_VERTICES], Vertex origin)
    {
       int signalX = 1;   // negative or positive signal 
       int signalY = 1;   // negative or positive signal 
       int signalZ = 1;   // negative or positive signal 

       for (uint i = 0; i < 4; i++)
       {

           if (i / 2 != 0)
               signalZ = -1;

           coordinates[2 * i].x = origin.x + lenght / 2.0 * signalX;
           coordinates[2 * i].y = origin.y + heigth / 2.0 * signalY;
           coordinates[2 * i].z = origin.z + depth / 2.0 * signalZ;

           signalY *= -1;
           coordinates[2 * i + 1].x = origin.x + lenght / 2.0 * signalX;
           coordinates[2 * i + 1].y = origin.y + heigth / 2.0 * signalY;
           coordinates[2 * i + 1].z = origin.z + depth / 2.0 * signalZ;

           signalX *= -1;
           signalY *= -1;
       }
    }

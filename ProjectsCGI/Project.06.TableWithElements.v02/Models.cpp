#include "Models.h"

Models::Models( Vertex origin, float color[4])
{
    this->originCenter = origin;
    this->color[0] = color[0];
    this->color[1] = color[1];
    this->color[2] = color[2];
    this->color[3] = color[3];
}

void Models::createTableBuffer(Vector3f Table[( NELEMENTS_TABLE * NVERTICES_CUBOIDE )]/*,  Vertex origin, float color[4]*/)
{
    // Table  --------------------------------------------------------

    // Vector with connection with GPU
    //Vector3f Table[( NELEMENTS_TABLE * NVERTICES_CUBOIDE )];
    Vertex topTable[NVERTICES_CUBOIDE];

    // Local variables to use in adjusts before send the datas to GPU
    Vertex legsTable[NLEGS][NVERTICES_CUBOIDE];
    Vertex origin;
    float lengthTop, heigthTop, depthTop;
    float length, heigth, depth;

    lengthTop = 1.0; heigthTop = 0.1; depthTop = 1.0;
    Cuboide Top(lengthTop, heigthTop, depthTop);
    originCenter.x = 0; originCenter.y = 0; originCenter.z = 0;
    Top.Dimensions(topTable, originCenter);

    // restart the variables 
    length = 0.1,  heigth = 0.8,  depth = 0.1;
    Cuboide leg1Table(length, heigth, depth);
    Cuboide leg2Table(length, heigth, depth);
    Cuboide leg3Table(length, heigth, depth);
    Cuboide leg4Table(length, heigth, depth);

    origin.x = topTable[0].x - length/2;  // 0.05 - 0.1/2
    origin.y = topTable[0].y - heigth/2;  // 0.05 - 1.0/2 = 0
    origin.z = topTable[0].z - depth /2;   // 0.5 - 0.1/2
    leg1Table.Dimensions(legsTable[0], origin);

    origin.x = topTable[2].x + length/2; 
    origin.y = topTable[2].y - heigth/2;
    origin.z = topTable[2].z - depth/2;
    leg2Table.Dimensions(legsTable[1], origin);
    
    origin.x = topTable[4].x - length/2; 
    origin.y = topTable[4].y - heigth/2;
    origin.z = topTable[4].z + depth/2;
    leg3Table.Dimensions(legsTable[2], origin);
    
    origin.x = topTable[6].x + length/2; 
    origin.y = topTable[6].y - heigth/2;
    origin.z = topTable[6].z + depth/2;
    leg4Table.Dimensions(legsTable[3], origin);
    
    for (int i = 0; i < NVERTICES; i++) {
        Table[i] = Vector3f(topTable[i].x, topTable[i].y, topTable[i].z);
        for (int j = 0; j < NLEGS; j++) {
            Table[i + (j + 1) * NVERTICES] = Vector3f(legsTable[j][i].x, legsTable[j][i].y, legsTable[j][i].z);
        }
        // printf("Table[%d] => x = %f y = %f e z = %f\t", i, Table[i].x, Table[i].y, Table[i].z);
        // printf("Table[%d] => x = %f y = %f e z = %f\n", i+8, Table[i+8].x, Table[i+8].y, Table[i+8].z);
    }

    // Buffer = NULL;

    this->Buffer = (Vector3f *)calloc(NELEMENTS_TABLE * NVERTICES_CUBOIDE , sizeof(Vector3f));
    if (!this->Buffer) {
        cout << "Memory Allocation Failed";
        exit(1);
    }
    else {
        int i;
        for (i = 0; i < NELEMENTS_TABLE * NVERTICES_CUBOIDE; i++)
        {
            *this->Buffer = Table[i];
            this->Buffer++;
        }
        this->Buffer -= i;
    }
}

void Models::createTableIndices(int Indices2[NELEMENTS_TABLE * NINDEX_CUBOID])
{   
    this->Indices = Indices2;

    int IndicesCuboid[] = {    
                               4,7,6,
                             7,1,3,
                             5,0,1,
                             2,1,0,
                             6,3,2,
                             4,2,0,
                             4,5,7,
                             7,5,1,
                             5,4,0,
                             2,3,1,
                             6,7,3,
                             4,6,2,
                            };

    for (int i = 0; i < NELEMENTS_TABLE; i++)
    {
        for (int j = 0; j < NINDEX_CUBOID; j++)
        {
            if (i == 0)
                *this->Indices = IndicesCuboid[j];
                
            else
                *this->Indices =  IndicesCuboid[j] + i*NVERTICES;
            
            printf("Indices[%d][%d] = %d\n", i, j, *Indices);
            this->Indices++;
        }
        
    }
    this->Indices -= NELEMENTS_TABLE * NINDEX_CUBOID;
      
}

void Models::createIcosahedroBuffer(){
    float scale = 0.25f;
    float X = 0.525731112119133606 * scale;
    float Z = .850650808352039932 * scale;

    static Vector3f vdata[NVERTICES_ICO] = {
       {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
       {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
       {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
    };
    // int tri[3] = { 4,8,1 };
    // for(int i = 0; i < NVERTICES_ICO; i++){
    //     icosahedroVector[i] = Vertex(vdata[i].x, vdata[i].y, vdata[i].z);
    // }
    // Vector3f redPure = Vector3f(1.0, 0.0, 0.0);
    // Vector3f greenPure = Vector3f(0.0, 1.0, 0.0);
    // Vector3f bluePure = Vector3f(0.0, 0.0, 1.0);
    // Vector3f whitePure = Vector3f(1.0, 1.0, 1.0);

    // int tri[3] = { 4,8,1 };
    // for(int i = 0; i < NVERTICES ; i++){    
    //     if(i == tri[0]){
    //         icosahedroVector[i] = Vertex(vdata[i].x, vdata[i].y, vdata[i].z, redPure);
    //     }
    //     else if (i == tri[1])
    //         icosahedroVector[i] = Vertex(vdata[i].x, vdata[i].y, vdata[i].z, greenPure);

    //     else if (i == tri[2])
    //         icosahedroVector[i] = Vertex(vdata[i].x, vdata[i].y, vdata[i].z, bluePure);

    //     else
    //         icosahedroVector[i] = Vertex(vdata[i].x, vdata[i].y, vdata[i].z, whitePure);

    // }

    this->Buffer = (Vector3f *)calloc(NVERTICES_ICO, sizeof(Vector3f));
    if (!this->Buffer) {
        cout << "Memory Allocation Failed";
        exit(1);
    }
    else {
        int i;
        for (i = 0; i < NVERTICES_ICO; i++)
        {
            *this->Buffer = vdata[i];
            this->Buffer++;
        }
        this->Buffer -= i;
    }

}

void Models::createIcosahedroIndexLine(int Indices2[62]){
     int IndicesIcosahedron[] = {
                                // Linhas
                                0, 1,  1, 2,
                                0, 2, 2, 3,
                                0, 3, 3, 4, 
                                0, 4, 4, 5,
                                0, 5, 5, 1, 

                                11, 10, 10, 9, 
                                11, 9, 9, 8,
                                11, 8, 8, 7, 
                                11, 7, 7, 6,
                                11, 6, 6, 10,

                                1, 5, 1, 6,  
                                2, 6, 2, 7,
                                3, 7, 3, 8,
                                4, 8, 4, 9,
                                5, 9, 5, 10,
                                1, 10,
                                                                 
    };
      this->Indices = (int *)calloc(62, sizeof(int));
    if (!this->Indices) {
        cout << "Memory Allocation Failed";
        exit(1);
    }
    else {
        int i;
        for (i = 0; i < 62; i++)
        {
            Indices2[i] = IndicesIcosahedron[i];
            *this->Indices =IndicesIcosahedron[i];
            this->Indices++;
        }
        this->Indices -= i;
    }
}

void Models::createIcosahedroIndexFaces(int Indices2[60]){
    int IndicesIcosahedron[] = {
               0,4,1,   0,9,4,   9,5,4,   4,5,8,  4,8,1,
               8,10,1,  8,3,10,  5,3,8,   5,2,3,  2,7,3,
               7,10,3,  7,6,10,  7,11,6,  11,0,6,  0,1,6,
               6,1,10,  9,0,11,  9,11,2,  9,2,5,  7,2,11                                                                 
    };


    this->Indices = (int *)calloc(60, sizeof(int));
    if (!this->Indices) {
        cout << "Memory Allocation Failed";
        exit(1);
    }
    else {
        int i;
        for (i = 0; i < 60; i++)
        {
            Indices2[i] = IndicesIcosahedron[i];
            *this->Indices =IndicesIcosahedron[i];
            this->Indices++;
        }
        this->Indices -= i;
    }
    
}

void Models:: setIndices(int n, int tam){
   
    int* p = this->Indices;

    for (  int i = 0; i < tam; i++)
    {
        *p += n;
        p++;
    }
    
}
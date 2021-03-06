#include "../inc/Models.h"

Models::Models() {}

Models::Models(int id){
    this->id = id;
}

Models::Models( Vertex origin, float color[3])
{
    this->originCenter = origin;
    this->color[0] = color[0];
    this->color[1] = color[1];
    this->color[2] = color[2];
}

void Models::createCubeBuffer(Vertex CubeOutput[NVERTICES_CUBOIDE*3], float size)
{   // variable size is size of dimensions of cubes sides.
    Vertex Cube[N_VERTICES];
    Vertex origin(0.0f, 0.0f, 0.0f);
    Cuboide c(size, size, size);

    // Buffer = NULL; 
    c.Dimensions(Cube, origin);
    for(int i=0; i < NVERTICES_CUBOIDE*3; i+=3){
        if(color[i] == -1){
            CubeOutput[i] = Vertex(Cube[i/3].pos.x, Cube[i/3].pos.y, Cube[i/3].pos.z);
            CubeOutput[i+1] = Vertex(Cube[i/3].pos.x, Cube[i/3].pos.y, Cube[i/3].pos.z);
            CubeOutput[i+2] = Vertex(Cube[i/3].pos.x, Cube[i/3].pos.y, Cube[i/3].pos.z);
        
        }
        else{
            CubeOutput[i] = Vertex(Cube[i/3].pos.x, Cube[i/3].pos.y, Cube[i/3].pos.z, color);
            CubeOutput[i+1] = Vertex(Cube[i/3].pos.x, Cube[i/3].pos.y, Cube[i/3].pos.z, color);
            CubeOutput[i+2] = Vertex(Cube[i/3].pos.x, Cube[i/3].pos.y, Cube[i/3].pos.z, color);
        }
     }
    
    this->Buffer = (Vertex*)calloc(NVERTICES_CUBOIDE*3, sizeof(Vertex));
    if (!this->Buffer) {
        cout << "Memory Allocation Failed";
        exit(1);
    }
    else {
        int i;
        for (i = 0; i < NVERTICES_CUBOIDE*3; i++)
        {
            *this->Buffer = CubeOutput[i];
            this->Buffer++;
        }
        this->Buffer -= i;
    }

    sizeBuffer = NVERTICES_CUBOIDE*3;
    id = 3;

}

void Models::createCubeIndices(int Indices2[NINDEX_CUBOID])
{
    this->Indices = Indices2;
    
    int IndicesCuboid[36] = {
                            //  4,7,6,
                            //  7,1,3,
                            //  5,0,1,
                            //  2,1,0,
                            //  6,3,2,
                            //  4,2,0,
                            //  4,5,7,
                            //  7,5,1,
                            //  5,4,0,
                            //  2,3,1,
                            //  6,7,3,
                            //  4,6,2,
                            // saida para 3 vertices com o mesmo valor de posi????o e normas distintas
                            0, 3, 9,
                            0, 9, 6,
                            1, 16, 4,
                            1, 13, 16,
                            2, 20, 14,
                            2, 8, 20,
                            18, 15, 12,
                            18, 21, 15,
                            7, 10, 22,
                            7, 22, 19,
                            5, 23, 11,
                            5, 17, 23,
    };

    for (int j = 0; j < NINDEX_CUBOID; j++)
    {
        *this->Indices = IndicesCuboid[j];

        printf("Indices[%d] = %d\n", j, *Indices);
        this->Indices++;
    }

    this->Indices -= NINDEX_CUBOID;

    sizeIndices = NINDEX_CUBOID;

}

void Models::createTableBuffer(Vertex Table[( NELEMENTS_TABLE * NVERTICES_CUBOIDE )]/*,  Vertex origin, float color[4]*/)
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
    originCenter.pos.x = 0; originCenter.pos.y = 0; originCenter.pos.z = 0;
    Top.Dimensions(topTable, originCenter);

    // restart the variables 
    length = 0.1,  heigth = 0.8,  depth = 0.1;
    Cuboide leg1Table(length, heigth, depth);
    Cuboide leg2Table(length, heigth, depth);
    Cuboide leg3Table(length, heigth, depth);
    Cuboide leg4Table(length, heigth, depth);

    origin.pos.x = topTable[0].pos.x - length/2;  // 0.05 - 0.1/2
    origin.pos.y = topTable[0].pos.y - heigth/2;  // 0.05 - 1.0/2 = 0
    origin.pos.z = topTable[0].pos.z - depth /2;   // 0.5 - 0.1/2
    leg1Table.Dimensions(legsTable[0], origin);

    origin.pos.x = topTable[2].pos.x + length/2; 
    origin.pos.y = topTable[2].pos.y - heigth/2;
    origin.pos.z = topTable[2].pos.z - depth/2;
    leg2Table.Dimensions(legsTable[1], origin);
    
    origin.pos.x = topTable[4].pos.x - length/2; 
    origin.pos.y = topTable[4].pos.y - heigth/2;
    origin.pos.z = topTable[4].pos.z + depth/2;
    leg3Table.Dimensions(legsTable[2], origin);
    
    origin.pos.x = topTable[6].pos.x + length/2; 
    origin.pos.y = topTable[6].pos.y - heigth/2;
    origin.pos.z = topTable[6].pos.z + depth/2;
    leg4Table.Dimensions(legsTable[3], origin);
    
    for (int i = 0; i < NVERTICES; i++) {
        if(color[0] == -1)
            Table[i] = Vertex(topTable[i].pos.x, topTable[i].pos.y, topTable[i].pos.z);
        else
            Table[i] = Vertex(topTable[i].pos.x, topTable[i].pos.y, topTable[i].pos.z, color);
        for (int j = 0; j < NLEGS; j++) {
            Table[i + (j + 1) * NVERTICES] = Vertex(legsTable[j][i].pos.x, legsTable[j][i].pos.y, legsTable[j][i].pos.z, color);
        }
        // printf("Table[%d] => x = %f y = %f e z = %f\t", i, Table[i].pos.x, Table[i].pos.y, Table[i].pos.z);
        // printf("Table[%d] => x = %f y = %f e z = %f\n", i+8, Table[i+8].pos.x, Table[i+8].pos.y, Table[i+8].pos.z);
    }

    // Buffer = NULL;

    this->Buffer = (Vertex *)calloc(NELEMENTS_TABLE * NVERTICES_CUBOIDE , sizeof(Vertex));
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
    id = 1;
    sizeBuffer = NELEMENTS_TABLE * NVERTICES_CUBOIDE;

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
      
    sizeIndices = NELEMENTS_TABLE * NINDEX_CUBOID;
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
    //     icosahedroVector[i] = Vertex(vdata[i].pos.x, vdata[i].pos.y, vdata[i].pos.z);
    // }
    // Vector3f redPure = Vector3f(1.0, 0.0, 0.0);
    // Vector3f greenPure = Vector3f(0.0, 1.0, 0.0);
    // Vector3f bluePure = Vector3f(0.0, 0.0, 1.0);
    // Vector3f whitePure = Vector3f(1.0, 1.0, 1.0);

    // int tri[3] = { 4,8,1 };
    // for(int i = 0; i < NVERTICES ; i++){    
    //     if(i == tri[0]){
    //         icosahedroVector[i] = Vertex(vdata[i].pos.x, vdata[i].pos.y, vdata[i].pos.z, redPure);
    //     }
    //     else if (i == tri[1])
    //         icosahedroVector[i] = Vertex(vdata[i].pos.x, vdata[i].pos.y, vdata[i].pos.z, greenPure);

    //     else if (i == tri[2])
    //         icosahedroVector[i] = Vertex(vdata[i].pos.x, vdata[i].pos.y, vdata[i].pos.z, bluePure);

    //     else
    //         icosahedroVector[i] = Vertex(vdata[i].pos.x, vdata[i].pos.y, vdata[i].pos.z, whitePure);

    // }

    this->Buffer = (Vertex *)calloc(NVERTICES_ICO, sizeof(Vertex));
    if (!this->Buffer) {
        cout << "Memory Allocation Failed";
        exit(1);
    }
    else {
        int i;
        for (i = 0; i < NVERTICES_ICO; i++)
        {
            if(color[0] == -1)
                *this->Buffer = Vertex(vdata[i].x, vdata[i].y , vdata[i].z );
            else
                *this->Buffer = Vertex(vdata[i].x, vdata[i].y , vdata[i].z, color );
            this->Buffer++;
        }
        this->Buffer -= i;
    }

    sizeBuffer = NVERTICES_ICO;
    id = 2;

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

    sizeIndices = 62;

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

void Models::setNormalVectorPoints(){
   
   Vector3f k1, k2, k3; 

   Vector3f n(0.0f, 0.0f, 0.0f);

   for(int k = 0; k < sizeIndices; k+=3){
        k1 = this->Buffer[this->Indices[k  ]].pos;
        k2 = this->Buffer[this->Indices[k+1]].pos;
        k3 = this->Buffer[this->Indices[k+2]].pos;
        n = computeFaceNormal(k1, k2, k3);

        this->Buffer[this->Indices[k  ]].normal = n;
        this->Buffer[this->Indices[k+1]].normal = n;
        this->Buffer[this->Indices[k+2]].normal = n;
   }
   
}    

Vector3f Models::computeFaceNormal(Vector3f v1, Vector3f v2, Vector3f v3)
{
    const float EPSILON = 0.000001f;

    // default return value (0, 0, 0)
    Vector3f n(0.0f, 0.0f, 0.0f);

    // find 2 edge vectors: v1-v2, v1-v3
    float ex1 = v2.x - v1.x;
    float ey1 = v2.y - v1.y;
    float ez1 = v2.z - v1.z;
    float ex2 = v3.x - v1.x;
    float ey2 = v3.y - v1.y;
    float ez2 = v3.z - v1.z;

    // cross product: e1 x e2
    float nx, ny, nz;
    nx = ey1 * ez2 - ez1 * ey2;
    ny = ez1 * ex2 - ex1 * ez2;
    nz = ex1 * ey2 - ey1 * ex2;

    // normalize only if the length is > 0
    float length = sqrtf(nx * nx + ny * ny + nz * nz);
    
    if(length > EPSILON)
    {
        // normalize
        float lengthInv = 1.0f / length;
        n.x = nx * lengthInv;
        n.y = ny * lengthInv;
        n.z = nz * lengthInv;
    }

    return n;
}

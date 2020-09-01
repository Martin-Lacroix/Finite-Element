#include <Eigen/Dense>
#include <iostream>
#include <direct.h>
#include "solver.h"
#include <fstream>
#include <vector>
#include "mesh.h"

using namespace std;
using namespace Eigen;

struct Param{

    vector<vector<double>> nXY;
    vector<vector<int>> eId;
    vector<vector<int>> fId;
    vector<int> nIdx;
    vector<int> nIdy;
};

Param meshParam(int type,int size,double xyMax){

    int k = 0;
    Param param;
    vector<int> idx;
    int node = size+1;
    vector<int> nIdx(node);
    vector<int> nIdy(node);
    vector<vector<int>> eId;
    vector<vector<int>> fId(size,vector<int>(2));
    vector<vector<double>> nXY(node*node,vector<double>(2));
    for(int i=size; i<node*size-1; i+=node){idx.push_back(i);}

    // Node coordinates

    for(int i=0; i<node; i++){
        for(int j=0; j<node; j++){
            nXY[i*node+j] = {j*xyMax/size,i*xyMax/size};
        }
    }

    // Element indices

    for(int i=0; i<node*size-1; i++){if(i!=idx[k]){

        if(type==4){eId.push_back({i,i+1,i+node+1,i+node});}
        if(type==3){eId.push_back({i,i+1,i+node});eId.push_back({i+1,i+node+1,i+node});}}
        else{k++;}
    }

    // Boundary face indices

    for(int i=0; i<node; i++){nIdy[i] = i;}
    for(int i=0; i<node; i++){nIdx[i] = i*node;}
    for(int i=0; i<size; i++){fId[i] = {(i+1)*node-1,(i+2)*node-1};}

    param.nXY = nXY;
    param.eId = eId;
    param.fId = fId;
    param.nIdx = nIdx;
    param.nIdy = nIdy;
    return param;
}

// Solves a static linear elsaticity equation ∇·σ(u) = 0

int main(){

    int type = 4;
    int size = 50;
    double xyMax = 4;

    Param param = meshParam(type,size,xyMax);
    vector<Vector2d> bcNeu(param.fId.size());
    for(int i=0; i<param.fId.size(); i++){bcNeu[i] = Vector2d {2,0};}

    Data data;
    data.E = 2;
    data.v = 1.0/3;
    data.bcNeu = bcNeu;
    data.fId = param.fId;
    data.nIdx = param.nIdx;
    data.nIdy = param.nIdy;
    data.bcDirX = vector<double> (param.nIdx.size(),0);
    data.bcDirY = vector<double> (param.nIdy.size(),0);

    // Mesh and solver

    Mesh mesh(param.nXY,param.eId);
    cout << "\nMesh: done" << endl;
    vector<VectorXd> u = elasticity(mesh,data);
    cout << "Solver: done" << endl;

    // Writes the file

    mkdir("../output");
    ofstream solution("../output/solution.txt");
    ofstream nXY("../output/nXY.txt");

    for (int i=0; i<param.nXY.size(); i++){
        nXY << param.nXY[i][0] << "," << param.nXY[i][1] << "\n";
    }

    for (int i=0; i<u.size(); i++){
        for (int j=0; j<u[0].size()-1; j++){solution << u[i][j] << ",";}
        solution << u[i][u[0].size()-1] << "\n";
    }

    cout << "Writing: done\n" << endl;
    return 0;
}

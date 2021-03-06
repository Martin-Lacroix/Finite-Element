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
    vector<int> nId;
};

Param meshParam(int type,int size,double xyMax){

    int k = 0;
    Param param;
    vector<int> idx;
    int node = size+1;
    vector<vector<int>> eId;
    vector<int> nId(4*size);
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

    for(int i=0; i<size; i++){
        
        nId[i] = i;
        nId[node+2*i] = (i+1)*node;
        nId[size+2*i] = (i+1)*node-1;
        nId[3*size+i] = node*size+i+1;
    }

    param.nXY = nXY;
    param.eId = eId;
    param.nId = nId;
    return param;
}

// Gaussian initial solution

VectorXd gaussian(vector<vector<double>> nXY,double xyMax){
    
    double mu = xyMax/2;
    int nNbr = nXY.size();
    VectorXd u(nXY.size());

    for(int i=0; i<nNbr; i++){u(i) = exp(-pow((nXY[i][0]-mu),2)/2-pow((nXY[i][1]-mu),2)/2);}
    u /= u.maxCoeff();
    return u;
}

// Solves the unsteady diffusion equation du(t,x,y)/dt - kΔu(t,xy) = 0

int main(){

    int type = 4;
    int size = 50;
    double xyMax = 10;

    Param param = meshParam(type,size,xyMax);
    VectorXd u0 = gaussian(param.nXY,xyMax);
    vector<double> bc(param.nId.size(),0);

    Data data;
    data.k = 1;
    data.u0 = u0;
    data.tMax = 1;
    data.bcDir = bc;
    data.dt = 0.001;
    data.nId = param.nId;

    // Mesh and solver

    Mesh mesh(param.nXY,param.eId);
    cout << "\nMesh: done" << endl;

    vector<VectorXd> u = diffusion(mesh,data);
    cout << "Solver: done" << endl;

    // Writes the file

    mkdir("output");
    ofstream solution("output/solution.txt");
    ofstream nXY("output/nXY.txt");

    for (int i=0; i<mesh.nXY.size(); i++){
        nXY << mesh.nXY[i][0] << "," << mesh.nXY[i][1] << "\n";
    }

    for (int i=0; i<u.size(); i++){
        for (int j=0; j<u[0].size()-1; j++){solution << u[i][j] << ",";}
        solution << u[i][u[0].size()-1] << "\n";
    }

    cout << "Writing: done\n" << endl;
    return 0;
}

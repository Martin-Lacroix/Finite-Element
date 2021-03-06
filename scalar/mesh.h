#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>

#ifndef MESH_H
#define MESH_H

using namespace std;
using namespace Eigen;
typedef SparseMatrix<double> SM;

class Elem{

    public:

    Elem(vector<vector<double>> nXY,int idx);

    int type;
    int gPts;
    int index;

    VectorXd detJ;
    MatrixXd dxN;
    MatrixXd dyN;
    MatrixXd xy;
    MatrixXd N;
    VectorXd w;
};

class Face{

    public:

    Face(vector<vector<double>> nXY,vector<int> fId,int idx);

    int index;
    double detJ;

    MatrixXd N;
    VectorXd w;
    Vector2d norm;
    vector<int> fId;
};

class Mesh{

    public:

    Mesh(vector<vector<double>> nXY,vector<vector<int>> eId);
    VectorXd neumannBC2(vector<Face> &fList,vector<VectorXd> F);
    VectorXd neumannBC1(vector<Face> &fList,vector<double> bc);
    VectorXd vector1D(function<VectorXd(MatrixXd)> fun);
    vector<Face> setFace(vector<vector<int>> fId);
    SM dirichletBC(SM A,vector<int> nId);
    SM matrix2D(string name);

    vector<Elem> eList;
    vector<vector<int>> eId;
    vector<vector<double>> nXY;

    int nNbr;
    int eNbr;
};

#endif
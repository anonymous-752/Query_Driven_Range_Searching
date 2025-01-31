#include <bits/stdc++.h>
#include <ctime>
#include "point.h"
using namespace std;

class Query{
public:
    Query(){}
    ~Query(){}

    virtual int stabs(int n,vector<point*>& pointset,int *leaves){
        return 0;//query \cup pointset = \emptyset
        return 1;//query \cup pointset = pointset
        return 2;//query stabs pointset
    }

    virtual void inside(int n, vector<point*>& pointset, vector<int>& ans){
        return;//make ans pointer to an array with the points that are inside the query
    }

    virtual bool inside(point *p){
        return false;
    }

    virtual void query2vector(vector<double>& Q){
        return;//return the query for input to the neural network
    }

};

class BallQuery: public Query{
public:
    point *center;
    double rad;

    BallQuery(int d, double *cent,double r){
        center = new point(d,cent);
        rad=r;
    }
    BallQuery(int d){
        center = new point(d);
        static random_device rd;
        static mt19937 gen(rd());
        normal_distribution<> dist(0.0, 1.0);
        for (int i=0;i<d;i++) {
            center->x[i] = dist(gen);
        }
        rad=abs(dist(gen))*4;
    }
    BallQuery(int d, int xdownlimit,int xuplimit, int rdownlimit,int ruplimit){
        rad=rand()%(ruplimit-rdownlimit)+rdownlimit;
        center= new point(d);
        for(int i=0;i<d;i++){
            center->x[i]=rand()%(xuplimit-xdownlimit)+xdownlimit;
        }
    }
    ~BallQuery(){
        delete center;
    }
    bool inside(point *p){
        if(dist(center,p)<=rad)return 1;
        return 0;
    }
    int stabs(int l,int r,vector<point*>& pointset,int *leaves){
        int insi=0,outsi=0;
        for(int i=l;i<=r;i++){
            if(inside(pointset[leaves[i]]))
                insi+=1;
            else outsi+=1;

            if(insi*outsi>0)break;
        }
        if(insi==0)
            return 0;//query \cup pointset = \emptyset
        else if(outsi==0)
            return 1;//query \cup pointset = pointset
        else
            return 2;//query stabs pointset
    }

    void inside(int n, vector<point*>& pointset, vector<int>& ans){
        ans.clear();
        for(int i=0;i<n;i++){
            if(inside(pointset[i]))
                ans.push_back(i);
        }
        return;
    }

    void query2vector(vector<double>& Q){
        Q.resize(center->d+1);
        //RowVector Q(center->d+1);
        for(int i=0;i<center->d;i++){
            Q[i]=center->x[i];
        }
        Q[center->d]=rad;
        return;
    }

};

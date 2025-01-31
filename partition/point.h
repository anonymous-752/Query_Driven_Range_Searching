#include <bits/stdc++.h>

using namespace std;

class point{
public:
    int d;
    double *x;
    point(int vd){
        d=vd;
        x= new double[d];
    };
    point(int vd, double *vx){
        d=vd;
        x=new double[d];
        for(int i=0;i<d;i++)
            x[i]=vx[i];
    };
    point(const point* a){
        d=a->d;
        x=new double[d];
        for(int i=0;i<d;i++)
            x[i]=a->x[i];
    };
    point(const vector<double>& a){
        d=a.size();
        x=new double[d];
        for(int i=0;i<d;i++)
            x[i]=a[i];
    };
    ~point(){
        delete [] x;
    };
    void print(ostream &out=cout){
        out<<"point: ";
        for(int i=0;i<d;i++){
            out<<x[i]<<" ";
        }
        out<<endl;
    };

    friend void print(const point* a,ostream &out=cout){
        out<<"point: ";
        for(int i=0;i<a->d;i++){
            out<<a->x[i]<<" ";
        }
        out<<endl;
    };

    friend double dist(const point* a, const point* b){
        double ans=0;
        const int d1=a->d,d2=b->d;
        if(d1!=d2)return -1;
        for(int i=0;i<d1;i++)ans+=(b->x[i]-a->x[i])*(b->x[i]-a->x[i]);
        ans=sqrt(ans);
        return ans;
    }
};

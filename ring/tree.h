#include "separator.h"

using namespace std;

class tree{
public:
    int d,np,mass,nl,nr,c,check,inside,nq;
    vector<int> rinco,used,rindex,pointindex,queryindex;
    vector<point*> points,subset,remsub,queries;
    bool isleaf;
    double r,tbalance,balance;
    separator *sep;
    tree *lchild,*rchild;

    tree(int vd,double vr){
        d=vd;
        c=pow(2,d);//doubling dimension
        r=vr;
        np=0;
        nq=0;
        mass=0;
        inside=0;
        balance=0.0;
        tbalance=0.0;
        nl=0;
        nr=0;
        bool isleaf=false;
        sep= nullptr;
        lchild=nullptr;
        rchild=nullptr;

    };

    void build_from_file(istream &in){
        string temps;
        in>>temps;
        if(temps[0]=='L'){//LEAF
            isleaf=true;
            in>>temps;//Up
            return;
        }
        if(temps[0]=='c'){//center
            double vdim[d];
            for(int i=0;i<d;i++)in>>vdim[i];
            in>>temps;//R:
            double vrs;
            in>>vrs;
            point *vc=new point(d,vdim);
            sep=new separator(d,vrs,r,vc);
            delete vc;
            in>>temps;//Left
            lchild=new tree(d,r);
            lchild->build_from_file(in);
            in>>temps;//Right
            rchild=new tree(d,r);
            rchild->build_from_file(in);
            in>>temps;//Up
        }
    }

    tree(string filename){
        ifstream fin(filename);
        char tempc;
        fin>>tempc>>d;
        fin>>tempc>>r;
        c=pow(2,d);//doubling dimension
        np=0;
        nq=0;
        mass=0;
        inside=0;
        balance=0.0;
        tbalance=0.0;
        nl=0;
        nr=0;
        bool isleaf=0;

        build_from_file(fin);
    }
    ~tree(){
        if(lchild!=nullptr)delete lchild;
        if(rchild!=nullptr)delete rchild;
        if(sep!=nullptr)delete sep;

        //delete [] points;

        for(int i=0;i<np;i++){
            delete points[i];
        }
        for(int i=0;i<nq;i++){
            delete queries[i];
        }
        points.clear();
        queries.clear();

    };
    void add(const point* a,int vpi){//ToDo update mass and balance
        np++;
        //mass++;
        points.push_back(new point(a));
        pointindex.push_back(vpi);

        if(sep!=nullptr){//add point to all tree if it is already built
            const point *cen=sep->center;
            const double rs=sep->rs;
            double td=dist(cen,a);

            if(td>=rs && td<=rs+2*r || double_equals(td,rs) ||double_equals(td,rs+2*r))mass++;
            if(td< rs+r || double_equals(td,rs+r)){
                inside++;
                lchild->add(a,vpi);
            }
            else rchild->add(a,vpi);
        }
    };
    void add_query(const point* a,int vqi){//ToDo update mass and balance
        //a->print();
        nq++;
        mass++;
        queries.push_back(new point(a));
        queryindex.push_back(vqi);
    };

    int checkmassbalance(const separator* s){
        const point *cen=s->center;
        const double rs=s->rs;
        int ans=0,le=0,ri=0;
        for(int i=0;i<np;i++){
            double td=dist(cen,points[i]);

            if(td<rs+r || double_equals(td,rs+r))le++;
            else ri++;
        }
        for(int i=0;i<nq;i++){
            double td=dist(cen,queries[i]);
            if(td>=rs && td<=rs+2*r || double_equals(td,rs) ||double_equals(td,rs+2*r))ans++;
        }
        tbalance=double(min(ri,le))/np;
        if (double(min(ri,le))/np<1.0/(100.0))return (nq+1);
        else return ans;
    };

    void tryall(int i,int *inco){
        if(i==np){
            if(subset.size()!=d+1)return;
            separator *possep=new separator(d,subset,inco,r);
            int temass=checkmassbalance(possep);
            if(temass<mass || temass==mass && tbalance>balance){//found better separator
                if(sep!=nullptr)delete sep;
                sep=possep;
                mass=temass;
                balance=tbalance;
            }
            else delete possep;
            return;
        }
        else if(np-i<d+1-subset.size() || subset.size()>d+1)return;
        tryall(i+1,inco);
        subset.push_back(points[i]);
        for(int j=0;j<3;j++){
            inco[subset.size()-1]=j;
            tryall(i+1,inco);
        }
        subset.pop_back();
    };

    void findsepbf(){
        check=0;
        if(subset.size()!=0)subset.clear();
        int incenout[d+1];
        tryall(0,incenout);
    };

    bool trythis(int *inco,int *index){
        bool ans=false;
        /*for(int i=0;i<15;i++){
            subset[i]->print();
        }*/
        //cout<<endl;
        separator *possep=new separator(d,subset,inco,r);
        int temass=checkmassbalance(possep);
        if(temass<mass || temass==mass && tbalance>balance){//found better separator
            if(sep!=nullptr)delete sep;
            sep=possep;
            mass=temass;
            balance=tbalance;
            ans=true;
            if(remsub.size()!=d+1)remsub.resize(d+1);
            if(rinco.size()!=d+1)rinco.resize(d+1);
            if(rindex.size()!=d+1)rindex.resize(d+1);
            for(int i=0;i<=d;i++){
                rinco[i]=inco[i];
                rindex[i]=index[i];
                remsub[i]=subset[i];
                //subset[i]->print();
            }
            //cout<<endl;
        }
        else delete possep;
        return ans;
    };

    bool trylocal(int *inco,int *index){
        bool ans=0;
        for(int i=0;i<=d;i++){//change ith point
            for(int j=0;j<np+nq;j++){//new point in place of index[i]
                bool difpoint=true;
                for(int k=0;k<=d;k++){//check if new point is different to all others
                    if(k==i)continue;
                    if(index[k]==j){
                        difpoint=false;
                        break;
                    }
                }
                if(!difpoint)continue;
                point *temp=subset[i];
                int tindex=index[i];
                if(j<np)subset[i]=points[j];
                else subset[i]=queries[j-np];
                index[i]=j;
                bool formeee=0;
                //cout<<i<<" "<<j<<endl;
                //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                for(int k=0;k<3;k++){
                    inco[i]=k;
                    //formeee=formeee||trythis(inco,index);
                    ans=ans||trythis(inco,index);
                }
                index[i]=tindex;
                subset[i]=temp;
            }
        }
        return ans;
    };

    int randnum(){//random number between 0-np-1
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dis(0, np+nq - 1);
        return dis(gen);
    }

    int randomindex(){
        bool au;
        int ans;
        do{
            ans=randnum();
            au=false;
            for(int i=0;i<used.size();i++){
                if(used[i]==ans){
                    au=true;
                    break;
                }
            }
        }while(au);
        used.push_back(ans);
        return ans;
    }

    void findseploc(){
        double vm, rss;
        if(subset.size()!=0)subset.clear();
        int incenout[d+1],index[d+1];

        int maxit=trunc(log2(np));
        //cout<<"Total number of iterations: "<<maxit<<endl;
        check=0;
        while(check<maxit){
            //cout<<"iteration: "<<check<<endl;
        //ToDo initialize 'randomly' the subset
            if(used.size()!=0)used.clear();
            if(subset.size()!=0)subset.clear();
            for(int i=0;i<=d;i++){
                int a=randomindex();
                if(a<np)subset.push_back(points[a]);
                else subset.push_back(queries[a-np]);
                index[i]=a;
                incenout[i]=0;
                //cout<<a<<" ";
            }
            //cout<<"Initialized"<<endl;
            while(trylocal(incenout,index)&& check<maxit){
                //cout<<"in while"<<endl;
                check++;
                for(int i=0;i<=d;i++){
                    subset[i]=remsub[i];
                    incenout[i]=rinco[i];
                    index[i]=rindex[i];
                }
            }
            check++;
            //cout<<"Iterations Done: "<<check<<endl;
        }
    }

    void splitdataset(){
        const point *cen=sep->center;
        const double rs=sep->rs;
        for(int i=0;i<np;i++){
            double td=dist(cen,points[i]);

            if(td< rs+r || double_equals(td,rs+r))lchild->add(points[i],pointindex[i]);
            else rchild->add(points[i],pointindex[i]);
        }
        for(int i=0;i<nq;i++){
            double td=dist(cen,queries[i]);

            if(td< rs+r || double_equals(td,rs+r))lchild->add_query(queries[i],queryindex[i]);
            else rchild->add_query(queries[i],queryindex[i]);
        }
    }
    void build_tree(){
	cout<<"new node "<<np<<" "<<nq<<endl;
    isleaf=false;
	if(np<=d+1){
            isleaf=true;
            return;
        }
        findseploc();
        if(mass>=nq-d){
            isleaf=true;
            delete sep;
            return;
        }
	cout<<mass<<" "<<balance<<endl;
        lchild=new tree(d,r);
        rchild=new tree(d,r);
        splitdataset();
        lchild->build_tree();
        rchild->build_tree();
        return;
    };

    void printrec(ostream &out=cout){//pre-order print
        if(isleaf){
            out<<"LEAF"<<endl;
            return;
        }
        sep->print(out);
        out<<"Left"<<endl;
        lchild->printrec(out);
        out<<"Up"<<endl;
        out<<"Right"<<endl;
        rchild->printrec(out);
        out<<"Up"<<endl;
        return;
    }
    void print(ostream &out=cout){
        out<<"d "<< d<<endl;
        out<<"r "<<fixed<<setprecision(20)<< r<<endl;
        printrec(out);
    }
    void printtofile(string filename){
        ofstream fout (filename);
        fout<<"d "<< d<<endl;
        fout<<"r "<<fixed<<setprecision(20)<< r<<endl;
        printrec(fout);
        fout.close();
        return;
    }
    int query(const point* a,double e=0){
        if(isleaf){
            int ans=0;
            for(int i=0;i<np;i++){
                if(dist(a,points[i])<=(1+e)*r)
                    ans++;
            }
            return ans;
        }
        const point *cen=sep->center;
        const double rs=sep->rs;
        double td=dist(cen,a);

        if(td>=rs && td<=rs+2*r || double_equals(td,rs) ||double_equals(td,rs+2*r)){
            int q1=lchild->query(a,e);
            int q2=rchild->query(a,e);
            return q1+q2;
        }
        if(td< rs){
            return lchild->query(a,e);
        }
        else return rchild->query(a,e);
    }
};

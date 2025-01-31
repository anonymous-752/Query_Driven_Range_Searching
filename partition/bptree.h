#include<bits/stdc++.h>
#include<Eigen/Dense>
#include "query.h"
#include "nn.h"
typedef Eigen::RowVectorXd RowVector;
using namespace std;

class treenode{
public:
    int l,r;
    int leafindex;
    treenode *lchild,*rchild;
    NeuralNetwork *nn;

    treenode(int s,int li){
        l=s;
        r=s;
        leafindex=li;
        lchild=nullptr;
        rchild=nullptr;
        nn=nullptr;
    }
    treenode(int vl,int vr, treenode *lc, treenode *rc){
        if(vl==vr || vl>vr)cout<<"mistake!!"<<endl;
        l=vl;
        r=vr;
        lchild=lc;
        rchild=rc;
        nn=nullptr;
    }

    ~treenode(){
        cout<<"deleting node"<<endl;
        if(lchild!=nullptr)delete lchild;
        cout<<"right"<<endl;
        if(rchild!=nullptr)delete rchild;
        cout<<"node deleted"<<endl;
    }

    void printrec(ostream &out=cout){//pre-order print
        if(l==r){
            out<<"LEAF "<<leafindex<<endl;
            return;
        }

        out<<"Left"<<endl;
        lchild->printrec(out);
        out<<"Up"<<endl;
        out<<"Right"<<endl;
        rchild->printrec(out);
        out<<"Up"<<endl;

        if(nn==nullptr)out<<"NoNeural"<<endl;
        else{
            try {
                out << "YesNeural" << endl;
                nn->print(out);
            } catch (const std::exception &e) {
                out << "Exception while printing NeuralNetwork: " << e.what() << endl;
            }
        }
        return;
    }

    friend int treequery(const treenode *curr,BallQuery *q,RowVector rowq,const vector<point*>& points,const int *leaves){
        //int d=points[0]->d;
        if(curr->l==curr->r){
            if (q->inside(points[curr->leafindex])){
                return 1;
            }
            return 0;
        }
        if(curr->l>=curr->r-20 || curr->nn==nullptr){
            int ans=0;
            for(int i=curr->l;i<=curr->r;i++){
                if (q->inside(points[leaves[i]])){
                    ans+=1;
                }
            }
            return ans;
        }
        NeuralNetwork *tnn=curr->nn;
        int stabstatus=tnn->testsingle(rowq);
        if (stabstatus==0){
            return 0;
        }
        else if(stabstatus==1){
            return (curr->r-curr->l+1);
        }
        else{
            int foundl= treequery(curr->lchild, q,rowq,points,leaves);
            int foundr= treequery(curr->rchild, q,rowq,points,leaves);
            return foundl+foundr;
        }
    }
    friend void treequeryfull(const treenode *curr,BallQuery *q,RowVector rowq,const vector<point*>& points,const int *leaves,vector<int>& ans){
        if(curr->l==curr->r){
            if (q->inside(points[curr->leafindex])){
                ans.push_back(curr->leafindex);
            }
            return;
        }
        if(curr->l>=curr->r-20 || curr->nn==nullptr){
            for(int i=curr->l;i<=curr->r;i++){
                if (q->inside(points[leaves[i]])){
                    ans.push_back(curr->leafindex);
                }
            }
            return;
        }
        NeuralNetwork *tnn=curr->nn;
        int stabstatus=tnn->testsingle(rowq);
        if (stabstatus==0){
            return;
        }
        else if(stabstatus==1){
            for(int i=curr->l;i<=curr->r;i++){
                if (q->inside(points[leaves[i]])){
                    ans.push_back(curr->leafindex);
                }
            }
            return;
        }
        else{
            treequeryfull(curr->lchild, q,rowq,points,leaves,ans);
            treequeryfull(curr->rchild, q,rowq,points,leaves,ans);
            return;
        }
    }

};

int linecounter=2;
bool found=false;
class BinPartTree{

public:
    int n;
    int *leaves;
    treenode *root;
    vector<point*> points;

    BinPartTree(){
        n=0;
    }

    BinPartTree(int vn, const int *vl){
        n=vn;
        leaves=new int[n];
        for(int i=0;i<n;i++)
            leaves[i]=vl[i];
    }
    BinPartTree(const vector<int> &vl){
        n=vl.size();
        leaves=new int[n];
        for(int i=0;i<n;i++)
            leaves[i]=vl[i];
    }
    treenode* build_from_file(istream &in,int l,int r){
        string str;
        treenode *ans;
        if(l==r){
            in>>str>>str;//LEAF #leaf
	    linecounter++;
            ans=new treenode(l,leaves[l]);
            return ans;
        }
        int mid=(l+r)/2;
        in>>str;//Left
	linecounter++;
        treenode *tr=build_from_file(in,l,mid);
        in>>str;//Up
	linecounter++;
        in>>str;//Right
	linecounter++;
        treenode *tl=build_from_file(in,mid+1,r);
        in>>str;//Up
	linecounter++;
        ans=new treenode(l,r,tr,tl);
        in>>str;//YesNeural or NoNeural
	linecounter++;
	if(!found && str[0]!='Y' && str[0]!='N'){
	    cout<<linecounter<<endl;
	    found=true;
	}
        if(str[0]=='Y'){
	    linecounter+=50;
            ans->nn=new NeuralNetwork(in);
        }
	else {ans->nn=nullptr;}

	return ans;
    }

    void buildbpt(string filename){
        ifstream fin(filename);
        string str;
        fin>>str>>n;
        leaves=new int[n];
        fin>>str>>str>>str;
        for(int i=0;i<n;i++){
            fin>>leaves[i];
        }
        fin>>str;
        root=build_from_file(fin,0,n-1);
    }

    treenode* build(int l, int r){
        treenode *ans;
        if(l==r){
            ans=new treenode(l,leaves[l]);
            return ans;
        }
        int mid=(l+r)/2;
        treenode *tr=build(l,mid);
        treenode *tl=build(mid+1,r);
        ans=new treenode(l,r,tr,tl);
        return ans;
    }

    void updateleaves(vector<int>& order){
        if(leaves!=nullptr)delete [] leaves;
        leaves=new int[order.size()];

        for(unsigned int i=0;i<order.size();i++)leaves[i]=order[i];
    }

    void buildbpt(){
        if(leaves==nullptr){
            leaves=new int[n];
            for(int i=0;i<n;i++)leaves[i]=i;
        }
        root=build(0,n-1);
    }

    ~BinPartTree(){
        delete [] leaves;
        cout<<"deleted leaves"<<endl;
        if(root!=nullptr)delete root;
    }

    void printbpt(ostream &out=cout){
        root->printrec(out);
    }

    int rangequery(BallQuery* q,RowVector rowq){
        return treequery(root,q,rowq,points,leaves);
    }

    void rangequeryfull(BallQuery* q, RowVector rowq, vector<int>& ans){
	return treequeryfull(root,q,rowq,points,leaves,ans);
    }

    void traininnernn(int l,int r,NeuralNetwork*& nn){
        int d=points[0]->d;
        double lr=0.001;
        if(nn==nullptr)
            nn=new NeuralNetwork({d+1,16,8,3},lr);
        RowVector rowq(d+1),rowsq(3);

        double ste=0.0,oldtte=0.0;
        long int countersamples=0;
        int countstable=0,countdiff=0;
        double diff=0.01;

        while(1){
            countersamples+=1;
            BallQuery *q=new BallQuery(d);
            vector<double> Q;
            q->query2vector(Q);
            for(int i=0;i<d;i++)rowq[i]=Q[i];
            double mindp=30000,maxdp=-1;
            for(int j=l;j<=r;j++){
                double td=dist(q->center,points[leaves[j]]);
                if(td>maxdp)maxdp=td;
                if(td<mindp)mindp=td;
            }

            maxdp*=1000;
            mindp*=1000;
            int maxdpi=(int)(trunc(maxdp));
            int mindpi=(int)(trunc(mindp));
	    int maxxx=2*maxdpi;

            for(int j=0;j<10;j++){

                int t=rand()%20;
                if(t<5){
                    rowq(Q.size()-1)=double(double(rand()%mindpi)/1000.0);
                    rowsq(0)=1;
		    rowsq(1)=0;
		    rowsq(2)=0;
		    //rowsq<<1,0,0;
                }
                else if(t<10){
                    rowq(Q.size()-1)=double(double(rand()%(maxxx-maxdpi)+maxdpi)/1000.0);
                    rowsq(0)=0;
		    rowsq(1)=1;
		    rowsq(2)=0;
                    //rowsq<<0,1,0;
                }
                else{
                    rowq(Q.size()-1)=double(double(rand()%(maxdpi-mindpi)+mindpi)/1000.0);
                    rowsq(0)=0;
		    rowsq(1)=0;
		    rowsq(2)=1;
                    //rowsq<<0,0,1;
                }


                if(countersamples==1000){
                    ste+=nn->trainsingle(rowq,rowsq,false);
                }
                else
                    ste+=nn->trainsingle(rowq,rowsq,false);
            }
	    delete q;
            if(countersamples==1000){
                double totte=ste/(countersamples*10);

                if(abs(oldtte-totte)<diff){
                    countstable++;
                    if(countstable==10){
                        if( lr<=0.0000015){
                            break;
                        }
                        countdiff=0;
                        diff/=10;
                        lr/=10;
                        cout<<"Change of learning rate to: "<<lr<<"    and checkdiff to: "<<diff<<endl;
                        nn->updateLR(lr);
                        countstable=0;
                    }
                }
                else{
                    countdiff++;
                    countstable=0;
                    if(countdiff==10){
                        diff*=10;
                        countdiff=0;
                        cout<<"Change of checkdiff to: "<<diff<<endl;
                    }
                }
                oldtte=totte;
                ste=0.0;
                countersamples=0;
            }
        }
        ofstream fout("testnn.txt");
        nn->print(fout);
        fout.close();
        return;
    }

    void traininnernns(treenode*& tn){
        cout<<tn->l<<" "<<tn->r<<endl;
        if(tn->l>=tn->r-19)return;
        traininnernn(tn->l,tn->r,tn->nn);
        traininnernns(tn->lchild);
        traininnernns(tn->rchild);
    }

    void train(){
        traininnernns(root);
    }

    void readmnist() {
        cout<<"beginning"<<endl;
        points.clear();
        ifstream file("mnist_train.csv");
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << "mnist_train.csv" << endl;
            return;
        }

        string line;
        cout<<"starting to read data"<<endl;
        int tcou=0;
        bool skipfirstline=1;
        while (getline(file, line)) {

            if(skipfirstline){
                skipfirstline=0;
                continue;
            }
            //if(tcou==500)break;
            tcou++;
            stringstream ss(line);
            vector<double> parsed_vec;
            string word;

            bool skiplabel=1;
            while (getline(ss, word, ',')) {
                if(skiplabel){
                    skiplabel=0;
                    continue;
                }
                // Trim whitespace and convert to ldouble.
                try {
                    parsed_vec.push_back(stold(word));
                } catch (const invalid_argument& e) {
                    //cerr << "Error: Invalid number format in line: " << line << endl;
                    continue;
                }
            }
            points.push_back(new point(parsed_vec));
            n++;
        }
        cout<<"data read"<<endl;
    }

    void readmnistprojection() {
        cout<<"beginning"<<endl;
        points.clear();
        ifstream file("MnistProjection.txt");
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << "MnistProjection.txt" << endl;
            return;
        }

        string line;
        cout<<"starting to read data"<<endl;
        int tcou=0;
        bool skipfirstline=0;
        while (getline(file, line)) {

            if(skipfirstline){
                skipfirstline=0;
                continue;
            }
            //if(tcou%1000==0)cout<<"read "<<tcou<<" lines"<<endl;
            //if(tcou==5000)break;
            tcou++;
            stringstream ss(line);
            vector<double> parsed_vec;
            string word;

            bool skiplabel=0;
            while (getline(ss, word, ' ')) {
                if(skiplabel){
                    skiplabel=0;
                    continue;
                }
                // Trim whitespace and convert to ldouble.
                try {
                    parsed_vec.push_back(stold(word));
                } catch (const invalid_argument& e) {
                    //cerr << "Error: Invalid number format in line: " << line << endl;
                    continue;
                }
            }
            points.push_back(new point(parsed_vec));
            n++;
        }
        cout<<"data read"<<endl;
    }

    void normalize_points(){

        int d=points[0]->d;

        point *mean=new point(d);
        point *stddev=new point(d);
        for (int i=0;i<d;i++){
            mean->x[i]=0;
            stddev->x[i]=0;
        }
        for (int j = 0; j<n;j++) {
            for (int i = 0; i < d; ++i) {
                mean->x[i] += points[j]->x[i];
            }
        }
        for (int i = 0; i < d; ++i) {
            mean->x[i] /= n;
        }
        for (int j = 0; j<n;j++) {
            for (int i = 0; i < d; ++i) {
                stddev->x[i] += pow(points[j]->x[i] - mean->x[i], 2);
            }
        }
        for (int i = 0; i < d; ++i) {
            stddev->x[i] = sqrt(stddev->x[i] / n);
        }

        for (int j=0;j<n;j++) {
            for (int i = 0; i < d; i++) {
                if ((stddev->x[i]) != 0) {
                    points[j]->x[i] = ((points[j]->x[i]) - (mean->x[i])) / (stddev->x[i]);
                }
            }

        }
        delete mean;
        delete stddev;
        cout<<"Points Normalized."<<endl;
    }

    void print(ostream &out=cout){
        out<<"N: "<<n<<endl;
        out<<"Order of Leaves: ";
        for(int i=0;i<n;i++){
            out<<leaves[i];
            if(i!=n-1)out<<" ";
        }
        out<<endl;
        out<<"TREE:"<<endl;
        root->printrec(out);
    }
};

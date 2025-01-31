#include<bits/stdc++.h>
#include<chrono>
#include<Eigen/Dense>
#include "query.h"
#include "nn.h"
typedef Eigen::RowVectorXd RowVector;
using namespace std;

int leaves[60001];
vector<point*> points;

double trainnn(int n, int fl, int sl, vector<point*>& points, NeuralNetwork*& nn){
    int d=points[0]->d;
    double lr=0.001;
    nn=new NeuralNetwork({d+1,fl,sl,3},lr);
    RowVector rowq(d+1),rowsq(3);

    //return -1;
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
        for(int j=0;j<n;j++){
            double td=dist(q->center,points[j]);
            if(td>maxdp)maxdp=td;
            if(td<mindp)mindp=td;
        }

        maxdp*=1000;
        mindp*=1000;
        int maxdpi=(int)(trunc(maxdp));
        int mindpi=(int)(trunc(mindp));
        const int stabs=q->stabs(0,n-1,points,leaves);

        for(int j=0;j<10;j++){

            int t=rand()%20;
            if(t<5){
                rowq(Q.size()-1)=double(double(rand()%mindpi)/1000.0);
                rowsq<<1,0,0;
            }
            else if(t<10){
                rowq(Q.size()-1)=double(double(rand()%(30000-maxdpi)+maxdpi)/1000.0);
                rowsq<<0,1,0;
            }
            else{
                rowq(Q.size()-1)=double(double(rand()%(maxdpi-mindpi)+mindpi)/1000.0);
                rowsq<<0,0,1;
            }


            if(countersamples==1000){
                //q->center->print();
                ste+=nn->trainsingle(rowq,rowsq,false);
                //cout<<"helloooo"<<endl;
                //nn->print();
            }
            else
                ste+=nn->trainsingle(rowq,rowsq,false);
        }

        if(countersamples==1000){
            double totte=ste/(countersamples*10);
            cout<<"Training Error: "<<totte<<endl;

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
        //break;
        //rowq(Q.size()-1)=Q[Q.size()-1]/20000;//4699;//normalization
        //if(stabs==0)
        //else if(stabs==1)rowsq<<0,1,0;
        //else rowsq<<0,0,1;
    }
    double totte=ste/(countersamples*10);
    return totte;
}

void testnn(int n,NeuralNetwork*& nn, ostream &out=cout){
    int o0=0,o1=0,o2=0;
    int c0=0,c1=0,c2=0;
    int w01=0,w02=0,w10=0,w12=0,w20=0,w21=0;
    int d=points[0]->d;

    double totnano=0.0;
    RowVector rowq(d+1);

    for(int i=0;i<1000;i++){
        BallQuery *q=new BallQuery(d);//,-4000,4000,1252,21172);//BallQuery(d,0,256,300,5000);
        vector<double> Q;
        q->query2vector(Q);
        for(int i=0;i<d;i++)rowq[i]=Q[i];
        //cout<<rowq<<endl;
        double mindp=30000,maxdp=-1;
        for(int j=0;j<n;j++){
            double td=dist(q->center,points[j]);
            if(td>maxdp)maxdp=td;
            if(td<mindp)mindp=td;
        }

        maxdp*=1000;
        mindp*=1000;
        int maxdpi=(int)(trunc(maxdp));
        int mindpi=(int)(trunc(mindp));
        //const int stabs=q->stabs(0,n-1,points,leaves);
        //for(int j=0;j<Q.size()-1;j++)rowq(j)=(Q[j]+4000)/8000;//255;//normalization

        for(int j=0;j<10;j++){

            int t=rand()%20;
            if(t<5){
                o0++;
                rowq(Q.size()-1)=double(double(rand()%mindpi)/1000.0);
		chrono::steady_clock::time_point begin=chrono::steady_clock::now();
		int pred=nn->testsingle(rowq);
		chrono::steady_clock::time_point end=chrono::steady_clock::now();
                if(pred==0)c0++;
                else if(pred==1)w01++;
                else w02++;
		totnano+=chrono::duration_cast<chrono::nanoseconds>(end-begin).count();
            }
            else if(t<10){
                o1++;
                rowq(Q.size()-1)=double(double(rand()%(30000-maxdpi)+maxdpi)/1000.0);
		chrono::steady_clock::time_point begin=chrono::steady_clock::now();
                int pred=nn->testsingle(rowq);
		chrono::steady_clock::time_point end=chrono::steady_clock::now();
                if(pred==0)w10++;
                else if(pred==1)c1++;
                else w12++;
		totnano+=chrono::duration_cast<chrono::nanoseconds>(end-begin).count();
            }
            else{
                o2++;
                rowq(Q.size()-1)=double(double(rand()%(maxdpi-mindpi)+mindpi)/1000.0);
		chrono::steady_clock::time_point begin=chrono::steady_clock::now();
                int pred=nn->testsingle(rowq);
		chrono::steady_clock::time_point end=chrono::steady_clock::now();
                if(pred==0)w20++;
                else if(pred==1)w21++;
                else c2++;
		totnano+=chrono::duration_cast<chrono::nanoseconds>(end-begin).count();
            }
        }
    }
    totnano/=10000.0;

    out<<"Testing:"<<endl;
    out<<"Queries of 3 types(0,1,2): "<<o0<<" "<<o1<<" "<<o2<<endl;
    out<<"Type 0 queries pred labels(0,1,2): "<<c0<<" "<<w01<<" "<<w02<<endl;
    out<<"Type 1 queries pred labels(0,1,2): "<<w10<<" "<<c1<<" "<<w12<<endl;
    out<<"Type 2 queries pred labels(0,1,2): "<<w20<<" "<<w21<<" "<<c2<<endl;
    out<<"Prediction Time: "<<totnano<<"[ns]"<<endl;
    out<<"-----------------------------"<<endl;
}

int main(){
    int n=0;
    for(int i=0;i<60000;i++)leaves[i]=i;
    ifstream file("MnistProjection.txt");
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << "MnistProjection.txt" << endl;
        return -1;
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

    int d=points[0]->d;
    //points[59999]->print();
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
    cout<<"Points Normalized."<<endl;

    int sizes[4]={5000,10000,30000,60000};
    int fls[4]={16,25,-1,100};
    int sls[4]={8,10,-1,20};

    ofstream fout("results.txt");

    for(int top=0;top<4;top++){
        for(int siz=0;siz<4;siz++){
            //if(top==0 && siz<3)continue;
            int tfl=fls[top],tsl=sls[top];
            if(tfl==-1){
                tsl=int(trunc(log2(sizes[siz])));
                tfl=2*tsl;
            }
            cout<<"Now at topology "<<top<<" and size "<<siz<<endl;
            NeuralNetwork *nn;
            fout<<"Hiddel Layers: "<<tfl<<" "<<tsl<<endl;
            fout<<"Pointset Size: "<<sizes[siz]<<endl;
            fout<<"Training:"<<endl;

            chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            double res=trainnn(sizes[siz],tfl,tsl,points,nn);
            chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

            fout<<"Final MSE: "<<res<<endl;
            fout<<"Time to converge: "<<std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]"<<endl;
            testnn(sizes[siz],nn,fout);
        }
    }
}

#include "tree.h"

using namespace std;

int toint(char *st){
    int ans=0,i=0;
    while('0'<=st[i] && '9'>=st[i]){
        ans*=10;
        ans+=st[i]-'0';
        i++;
    }
    return ans;
}

int main(int argc,char * argv[]){
    //srand(time(NULL));


    vector<point*> dataset;
    ifstream fin("MnistProjection.txt");
    for(int i=0;i<60000;i++){
        double vdim[15];
        for(int j=0;j<15;j++){
            int t;
            fin>>t;
            vdim[j]=(double)(t);
        }
        point *a=new point(15,vdim);
        dataset.push_back(a);
    }

    fin.close();

    //normalize
    int d=dataset[0]->d;
    int n=60000;
    point *mean=new point(d);
    point *stddev=new point(d);
    for (int i=0;i<d;i++){
        mean->x[i]=0;
        stddev->x[i]=0;
    }
    for (int j = 0; j<n;j++) {
        for (int i = 0; i < d; ++i) {
            mean->x[i] += dataset[j]->x[i];
        }
    }
    for (int i = 0; i < d; ++i) {
        mean->x[i] /= n;
    }
    for (int j = 0; j<n;j++) {
        for (int i = 0; i < d; ++i) {
            stddev->x[i] += pow(dataset[j]->x[i] - mean->x[i], 2);
        }
    }
    for (int i = 0; i < d; ++i) {
        stddev->x[i] = sqrt(stddev->x[i] / n);
    }

    for (int j=0;j<n;j++) {
        for (int i = 0; i < d; i++) {
            if ((stddev->x[i]) != 0) {
                dataset[j]->x[i] = ((dataset[j]->x[i]) - (mean->x[i])) / (stddev->x[i]);
            }
        }
	//if(j<10)dataset[j]->print();

    }
    delete mean;
    delete stddev;
    cout<<"Points read and normalized"<<endl;

    int ccc=0;
    vector<double> timess;
    double tdmi=10.0,tdma=-1,meannn=0.0;
    for(int i=0;i<5000;i++){
	for(int j=i+1;j<5000;j++){
	    ccc++;
	    double td=dist(dataset[i],dataset[j]);
	    int ttt=rand()%3;
	    if(ttt==0)timess.push_back(td);
	    meannn+=td;
	    if(td>tdma)tdma=td;
	    if(td<tdmi)tdmi=td;
	}
    }
    meannn/=double(ccc);
    sort(timess.begin(),timess.end());
    cout<<timess.size()<<" 20 quantile: "<<timess[timess.size()/5]<<endl;
    cout<<timess[0]<<" "<<timess[timess.size()-1]<<endl;
    cout<<tdma<<endl<<tdmi<<endl<<meannn<<endl;
    //###

    vector<point*> queries;
    ifstream fin2("queries.txt");
    string temp;
    for(int i=0;i<120000;i++){
        double vdim[15];
        //fin2>>temp;//point:
        //cout<<temp<<" ";
        for(int j=0;j<15;j++){
            double t;
            fin2>>t;
            vdim[j]=t;
            //cout<<t<<" ";
        }
	//if(i%2==1 && i<60000)continue;
        point *a=new point(15,vdim);
        //a->print();
        queries.push_back(a);
    }
    //int n=60000;
    mean=new point(d);
    stddev=new point(d);
    for (int i=0;i<d;i++){
        mean->x[i]=0;
        stddev->x[i]=0;
    }
    for (int j = 0; j<120000;j++) {
        for (int i = 0; i < d; ++i) {
            mean->x[i] += queries[j]->x[i];
        }
    }
    for (int i = 0; i < d; ++i) {
        mean->x[i] /= 120000;
    }
    for (int j = 0; j<120000;j++) {
        for (int i = 0; i < d; ++i) {
            stddev->x[i] += pow(queries[j]->x[i] - mean->x[i], 2);
        }
    }
    for (int i = 0; i < d; ++i) {
        stddev->x[i] = sqrt(stddev->x[i] / 120000);
    }

    for (int j=0;j<120000;j++) {
        for (int i = 0; i < d; i++) {
            if ((stddev->x[i]) != 0) {
                queries[j]->x[i] = ((queries[j]->x[i]) - (mean->x[i])) / (stddev->x[i]);
            }
        }
	//if(j<10)dataset[j]->print();

    }
    delete mean;
    delete stddev;

    //tree *root=new tree(filename);
    cout<<"Queries read and normalized"<<endl;

    string intfile="newtree_";
    int sti=0;
    while('0'<=argv[1][sti] && '9'>=argv[1][sti]){
        intfile+=argv[1][sti];
        //outfile+=argv[1][sti];
        sti++;
    }
    intfile+=".txt";

    tree *root=new tree(intfile);
    cout<<"tree built"<<endl;

    int samplesize=toint(argv[1]);

    //string resfilename="tree_";
    string outfile="querytimesnew_";
    sti=0;
    while('0'<=argv[1][sti] && '9'>=argv[1][sti]){
        //resfilename+=argv[1][sti];
        outfile+=argv[1][sti];
        sti++;
    }
    //resfilename+=".txt";
    outfile+=".txt";
    //root->printtofile(resfilename);
    for(int i=0;i<60000;i++){
        root->add(dataset[i],i);
    }

    ofstream fout(outfile);

    long double tottimenano=0;
    //ifstream fin3("queries_test.txt");
    for(int i=0;i<120000;i++){
        int pred;
        chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        pred=root->query(queries[i],0);
        chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        tottimenano+=double(chrono::duration_cast<std::chrono::microseconds> (end - begin).count())/double(1.0);
    }
    tottimenano/=120000.0;

    fout<<"Mean query time per answer: "<<tottimenano<<"[us]"<<endl;
}

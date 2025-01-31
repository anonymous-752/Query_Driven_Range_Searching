#include "minspantree.h"
#include "bptree.h"
#include <ctime>
using namespace std;

//unsigned short edges[60000][60000];

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
    srand(time(0));
    MinSpanTree *mst;
    BinPartTree *bpt;

    bpt=new BinPartTree();
    bpt->readmnistprojection();

    bpt->normalize_points();
    //bpt->points[0]->print();

    int n=bpt->n;

    int querysamples=toint(argv[1]);
    cout<<"Number of samples: "<<querysamples<<endl;
    int d=bpt->points[0]->d;

    auto edges= new unsigned short[60000][60000];
    for(int i=0;i<60000;i++){
	for(int j=i+1;j<60000;j++){
	    edges[i][j]=0;
	}
    }
    cout<<"array initialized"<<endl;
    int emptyq=0,allq=0,has1000=0,has30000=0,hasmore=0;
    for(int i=0;i<querysamples;i++){
        BallQuery *q=new BallQuery(d);
        vector<int> ins;
        q->inside(n,bpt->points,ins);

        vector<int> outs;

	if(ins.size()==0)emptyq++;
	else if(ins.size()<=1000)has1000++;
	else if(ins.size()<=30000)has30000++;
	else if(ins.size()<60000)hasmore++;
	else if(ins.size()==60000)allq++;

        int j=0,c=0;
        while(j<n){
            while(c<ins.size() && ins[c]==j){
                c++;
                j++;
            }
            if(j>=n)break;
            outs.push_back(j);
            j++;
        }

        //cout<<q->rad<<" in: "<<ins.size()<<" out: "<<outs.size()<<endl;
        for(int j=0;j<ins.size();j++){
            for(int y=0;y<outs.size();y++){
                edges[ins[j]][outs[y]]+=1;
                edges[outs[y]][ins[j]]+=1;
            }
        }
        delete q;
	//if(i%100==0)cout<<i<<endl;
    }
    string filenameq="stabing_info";
    filenameq+=argv[1][0];
    filenameq+=".txt";
    ofstream foutqstab(filenameq);
    foutqstab<<"Number of Queries: "<<querysamples<<endl;
    foutqstab<<"Empty: "<<emptyq<<endl;
    foutqstab<<"<=1000: "<<has1000<<endl;
    foutqstab<<"<=30000: "<<has30000<<endl;
    foutqstab<<"<60000: "<<hasmore<<endl;
    foutqstab<<"All in: "<<allq<<endl;
    foutqstab.close();

    mst=new MinSpanTree(n);
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            mst->addedge(edges[i][j],i,j);
        }
	//if(i%60==0)cout<<"i at "<<i<<endl;
    }
    delete []edges;
    cout<<"mst ready"<<endl;
    mst->computemst();
    mst->computesp();
    //for(int i=0;i<n;i++){
    //    cout<<mst->sp[i]<< " ";
    //}
    //cout<<endl;

    bpt->updateleaves(mst->sp);
    //delete []edges;
    delete mst;

    bpt->buildbpt();//"testbpt.txt");
    cout<<"before training"<<endl;
    bpt->train();

    string filename="bptree_sample";
    filename+=argv[1][0];
    filename+=".txt";
    ofstream fout1(filename);
    bpt->print(fout1);

    //NeuralNetwork n({ 2, 3, 2 });
    //data in_dat, out_dat;
    //genData("testdata");
    //ReadCSV("mnist_train.csv", in_dat,1);
    //cout<<in_dat.size()<<endl;
    //ReadCSV("testdata-out", out_dat);
    //n.train(in_dat, out_dat, 20);
}

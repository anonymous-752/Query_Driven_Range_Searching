#include<bits/stdc++.h>
#include<chrono>
#include<Eigen/Dense>
#include "bptree.h"
typedef Eigen::RowVectorXd RowVector;
using namespace std;

int leaves[60001];
vector<point*> points;

void testtree(int n, BinPartTree* bpt, ostream &out=cout){
    out<<"Results: RealValue QueryAnswer"<<endl;
    int d=bpt->points[0]->d;
    RowVector rowq(d+1);
    long double microseconds=0.0;
    int testsamp=100000;
    //cout<<"heree"<<endl;
    for(int i=0;i<testsamp;i++){
	if(i%10000==0)cout<<i<<endl;
        BallQuery *q=new BallQuery(d);//,-4000,4000,1252,21172);//BallQuery(d,0,256,300,5000);
        vector<double> Q;
        q->query2vector(Q);
        for(int i=0;i<=d;i++)rowq(i)=Q[i];
        //cout<<rowq<<endl;
	//cout<<"sending query"<<endl;
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        const int pred=bpt->rangequery(q,rowq);
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
        //for(int j=0;j<Q.size()-1;j++)rowq(j)=(Q[j]+4000)/8000;//255;//normalization
	microseconds+=chrono::duration_cast<chrono::microseconds>(end-begin).count();
        int realans=0;
	//cout<<"here2"<<endl;
        for(int j=0;j<n;j++){
            double td=dist(q->center,bpt->points[bpt->leaves[j]]);
	    //cout<<j<<endl;
            if(td<=q->rad)realans++;
        }
	out<<realans<<"            "<<pred<<endl;
	delete q;
    }
    microseconds=microseconds/double(testsamp);
    out<<"Mean query time: "<<microseconds<<"[ms]"<<endl;
}

void testtreefull(int n, BinPartTree* bpt, ostream &out=cout){
    out<<"Results: RealValue QueryAnswer"<<endl;
    int d=bpt->points[0]->d;
    RowVector rowq(d+1);
    long double microseconds=0.0;
    int testsamp=100000;
    //cout<<"heree"<<endl;
    for(int i=0;i<testsamp;i++){
	if(i%10000==0)cout<<i<<endl;
        BallQuery *q=new BallQuery(d);//,-4000,4000,1252,21172);//BallQuery(d,0,256,300,5000);
        vector<double> Q;
        q->query2vector(Q);
        for(int i=0;i<=d;i++)rowq(i)=Q[i];
        //cout<<rowq<<endl;
	//cout<<"sending query"<<endl;
	vector<int> ans;
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        bpt->rangequeryfull(q,rowq,ans);
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
        //for(int j=0;j<Q.size()-1;j++)rowq(j)=(Q[j]+4000)/8000;//255;//normalization
	microseconds+=double(chrono::duration_cast<chrono::microseconds>(end-begin).count())/double(ans.size()+1.0);
        int realans=0;
	//cout<<"here2"<<endl;
        for(int j=0;j<n;j++){
            double td=dist(q->center,bpt->points[bpt->leaves[j]]);
	    //cout<<j<<endl;
            if(td<=q->rad)realans++;
        }
	out<<realans<<"            "<<ans.size()<<endl;
	delete q;
    }
    microseconds=microseconds/double(testsamp);
    out<<"Mean query time: "<<microseconds<<"[ms]"<<endl;
}


void testtreefullall(int n, BinPartTree* bpt0, ostream &out0, BinPartTree* bpt5, ostream &out5, BinPartTree* bpt1, ostream &out1, BinPartTree* bpt3, ostream &out3, BinPartTree* bpt6, ostream &out6){
    out0<<"Results: RealValue QueryAnswer"<<endl;
    out5<<"Results: RealValue QueryAnswer"<<endl;
    out1<<"Results: RealValue QueryAnswer"<<endl;
    out3<<"Results: RealValue QueryAnswer"<<endl;
    out6<<"Results: RealValue QueryAnswer"<<endl;
    int d=bpt0->points[0]->d;
    RowVector rowq(d+1);
    long double microseconds0=0.0;
    long double microseconds5=0.0;
    long double microseconds1=0.0;
    long double microseconds3=0.0;
    long double microseconds6=0.0;
    int testsamp=100000;
    //cout<<"heree"<<endl;
    for(int i=0;i<testsamp;i++){
	if(i%10000==0)cout<<i<<endl;
        BallQuery *q=new BallQuery(d);//,-4000,4000,1252,21172);//BallQuery(d,0,256,300,5000);
        vector<double> Q;
        q->query2vector(Q);
        for(int i=0;i<=d;i++)rowq(i)=Q[i];
        //cout<<rowq<<endl;
	//cout<<"sending query"<<endl;
	vector<int> ans;
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        bpt0->rangequeryfull(q,rowq,ans);
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
        microseconds0+=double(chrono::duration_cast<chrono::microseconds>(end-begin).count())/double(ans.size()+1.0);
        int ans0=ans.size();
	//cout<<ans0<<endl;
	ans.clear();
	begin = chrono::steady_clock::now();
        bpt5->rangequeryfull(q,rowq,ans);
	end = chrono::steady_clock::now();
        microseconds5+=double(chrono::duration_cast<chrono::microseconds>(end-begin).count())/double(ans.size()+1.0);
	int ans5=ans.size();
	//cout<<ans5<<endl;
	ans.clear();
	begin = chrono::steady_clock::now();
        bpt1->rangequeryfull(q,rowq,ans);
	end = chrono::steady_clock::now();
        microseconds1+=double(chrono::duration_cast<chrono::microseconds>(end-begin).count())/double(ans.size()+1.0);
	int ans1=ans.size();
	//cout<<ans1<<endl;
	ans.clear();
	begin = chrono::steady_clock::now();
        bpt3->rangequeryfull(q,rowq,ans);
	end = chrono::steady_clock::now();
        microseconds3+=double(chrono::duration_cast<chrono::microseconds>(end-begin).count())/double(ans.size()+1.0);
	int ans3=ans.size();
	//cout<<ans3<<endl;
	ans.clear();
	begin = chrono::steady_clock::now();
        bpt6->rangequeryfull(q,rowq,ans);
	end = chrono::steady_clock::now();
	microseconds6+=double(chrono::duration_cast<chrono::microseconds>(end-begin).count())/double(ans.size()+1.0);
	int ans6=ans.size();
	//cout<<ans6<<endl;
	int realans=0;
	//cout<<"here2"<<endl;
        for(int j=0;j<n;j++){
	    //cout<<"printing query"<<endl;
	    //q->center->print();
            double td=dist(q->center,bpt0->points[j]);
	    //cout<<j<<endl;
            if(td<=q->rad)realans++;
        }
	out0<<realans<<"            "<<ans0<<endl;
	out5<<realans<<"            "<<ans5<<endl;
	out1<<realans<<"            "<<ans1<<endl;
	out3<<realans<<"            "<<ans3<<endl;
	out6<<realans<<"            "<<ans6<<endl;
	delete q;
    }
    microseconds0=microseconds0/double(testsamp);
    microseconds5=microseconds5/double(testsamp);
    microseconds1=microseconds1/double(testsamp);
    microseconds3=microseconds3/double(testsamp);
    microseconds6=microseconds6/double(testsamp);
    out0<<"Mean query time: "<<microseconds0<<"[us]"<<endl;
    out5<<"Mean query time: "<<microseconds5<<"[us]"<<endl;
    out1<<"Mean query time: "<<microseconds1<<"[us]"<<endl;
    out3<<"Mean query time: "<<microseconds3<<"[us]"<<endl;
    out6<<"Mean query time: "<<microseconds6<<"[us]"<<endl;
}

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
    
    BinPartTree *bpt0=new BinPartTree();
    bpt0->readmnistprojection();
    bpt0->normalize_points();
    string infilename0="bptree_sample0.txt";
    string filename0="bptree_testresultsfullsame0.txt";
    cout<<"building tree 0..."<<endl;
    bpt0->buildbpt(infilename0);
    cout<<"tree built"<<endl;
    ofstream fout0(filename0);
    
    BinPartTree *bpt5=new BinPartTree();
    bpt5->readmnistprojection();
    bpt5->normalize_points();
    string infilename5="bptree_sample5.txt";
    string filename5="bptree_testresultsfullsame5.txt";
    cout<<"building tree 5000..."<<endl;
    bpt5->buildbpt(infilename5);
    cout<<"tree built"<<endl;
    ofstream fout5(filename5);

    BinPartTree *bpt1=new BinPartTree();
    bpt1->readmnistprojection();
    bpt1->normalize_points();
    string infilename1="bptree_sample1.txt";
    string filename1="bptree_testresultsfullsame1.txt";
    cout<<"building tree 10000..."<<endl;
    bpt1->buildbpt(infilename1);
    cout<<"tree built"<<endl;
    ofstream fout1(filename1);

    BinPartTree *bpt3=new BinPartTree();
    bpt3->readmnistprojection();
    bpt3->normalize_points();
    string infilename3="bptree_sample3.txt";
    string filename3="bptree_testresultsfullsame3.txt";
    cout<<"building tree 30000..."<<endl;
    bpt3->buildbpt(infilename3);
    cout<<"tree built"<<endl;
    ofstream fout3(filename3);

    BinPartTree *bpt6=new BinPartTree();
    bpt6->readmnistprojection();
    bpt6->normalize_points();
    string infilename6="bptree_sample6.txt";
    string filename6="bptree_testresultsfullsame6.txt";
    cout<<"building tree 60000..."<<endl;
    bpt6->buildbpt(infilename6);
    cout<<"tree built"<<endl;
    ofstream fout6(filename6);

    testtreefullall(60000,bpt0,fout0,bpt5,fout5,bpt1,fout1,bpt3,fout3,bpt6,fout6);
}

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char * argv[]){
    string filename="bptree_testresultsfullsame";
    filename+=argv[1][0];
    filename+=".txt";
    ifstream fin(filename);
    string t;
    fin>>t>>t>>t;
    long long int found=0,cor=0;
    for(int i=0;i<100000;i++){
	    int a,b;
	    fin>>a>>b;
	    found+=b;
	    cor+=a;
    }
    double ans=(double)((long double)(found)/(long double)(cor));
    cout<<ans<<endl;
}

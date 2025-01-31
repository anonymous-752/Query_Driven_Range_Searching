#include <bits/stdc++.h>

using namespace std;

typedef pair<int,int> ii;
typedef pair<unsigned short int, ii> ldii;
typedef pair<unsigned short, int> ldi;
typedef vector<ldi> vii;

class MinSpanTree{
public:
    int n,e;
    int value;
    vector<vii> tree;
    vector<ldii> edges;
    vector<int> sp;

    MinSpanTree(int vn): n(vn),e(0),value(0){}

    MinSpanTree(int vn,int ve,const ldii *vedge){
        n=vn;
        e=ve;
        value=0;
        for(int i=0;i<e;i++){
            ldii tedg=vedge[i];
            edges.push_back(ldii(tedg.first,ii(tedg.second.first,tedg.second.second)));
        }
    }


    void addedge(unsigned short int vd, int vu,int vv){
        edges.push_back(ldii(vd,ii(vu,vv)));
        e++;
    }

    void computemst(){
        sort(edges.begin(),edges.end());

        int addededges=0;

        uf= new int[n];
        for(int i=0;i<n;i++)uf[i]=i;
        tree.resize(n);

        for(int i=0;i<e;i++){
            if(addededges==n-1)break;
            ldii tedge=edges[i];
            if(findf(tedge.second.first)==findf(tedge.second.second))continue; //circle

            addededges++;

            value+=tedge.first;
            tree[tedge.second.first].push_back(ldi(tedge.first,tedge.second.second));
            tree[tedge.second.second].push_back(ldi(tedge.first,tedge.second.first));

            unionf(tedge.second.first,tedge.second.second);
        }

        delete [] uf;
    }

    ~MinSpanTree(){
        tree.clear();
        edges.clear();
        sp.clear();
    }

    void computesp(){
        int st=dfs(0,0,-1).first;
        dfs(st,-1);
    }

private:

    int *uf;

    int findf(int a){
        if(uf[a]==a)return a;
        uf[a]=findf(uf[a]);
        return uf[a];
    }
    void unionf(int a,int b){
        uf[findf(a)]=findf(b);
        return;
    }
    ii dfs(int st,int dist,int par){
        int ansdi=dist, ansu=st;
        for(int i=0;i<tree[st].size();i++){
            if(tree[st][i].second == par)continue;
            ii tans=dfs(tree[st][i].second,dist+1,st);
            if(tans.second>ansdi){
                ansdi=tans.second;
                ansu=tans.first;
            }
        }
        return ii(ansu,ansdi);
    }
    void dfs(int st, int par){
        sp.push_back(st);
        for(int i=0;i<tree[st].size();i++){
            if(tree[st][i].second==par)continue;
            dfs(tree[st][i].second,st);
        }
        return;
    }
};

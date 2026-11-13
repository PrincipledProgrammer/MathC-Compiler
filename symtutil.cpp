#include "symtutil.h"
#include <bits/stdc++.h>
using namespace std;

char *names[MAXN];
int types[MAXN]; // 0 int , 1 rtl , 2 flt , 3 cpx , 4x arr of type x
int curr_idx = 0;


bool search(char* id){
    for (int i = 0; i < curr_idx;i++){
        if(!strcmp(names[i],id))
            return true;
    }
    return false;
}

int fetch(char* id){
    for (int i = 0; i < curr_idx; i++)
    {
        if (!strcmp(names[i], id))
            return i;
    }

    return -1; // marks that it is called without search being called
}

void add(char* id,int type){
    if(search(id)){
        int idx = fetch(id);
        types[idx] = type;
    }
    else{
        names[curr_idx] = strdup(id);
        types[curr_idx++] = type;
    }
}

int fetch_type(char * id){
    if(search(id))return types[fetch(id)];
    else
        return -1;
}

char *fetch_name(int idx) { return names[idx]; }
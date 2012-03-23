#include <iostream>
#include<vector>
#include "stencil.h"
// Unit test for stencil.cpp

using namespace std;

void ce(Stencil* s){
    cout << s->c << " " << s->e << endl;
    return;
}

int main(){
    vector<double> x;
    vector<double> y;
    Stencil* s;
    
    for(int i = 1; i<=25; i++){
        x.push_back(i);
        y.push_back(i*2);
    }
    
    s = new Stencil(25,5);
    s -> assign(&x);
    
    cout << "Test 1: should see '1 2', '5 1' and '2 3'" << endl;
    ce(s);
    for(int i = 0; i < 4; i++)
        s++;
    ce(s);
    for(int i = 0; i <= 20; i++)
        s++;
    ce(s);

    cout << "Test 2: should see '2 4'" << endl;
    s->assign(&y);
    ce(s);
    
    cout << "Test 3: should see 2925" << endl;
    s->assign(&x);
    int total = 0;
    for(int i = 0; i <= 24; i++,s++)
        total += s->ne + s->n + s->nw + s->e + s->c + s->w + s->se + s->s + s->sw;
    cout << total << endl;
     
    return 0;
}


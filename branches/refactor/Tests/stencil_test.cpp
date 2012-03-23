#include <iostream>
#include<vector>
#include "stencil.h"
// Unit test for stencil.cpp

using namespace std;

int main(){
    vector<int> x;
    Stencil* old[2];
    
    for(int i = 1; i<=25; i++)
        x.push_back(i);
    
    old[1] = new Stencil(25,5);
    old[2] = new Stencil(25,5);
    
    old[1]->assign(&x);
    old[2]->assign(&x);
    
    cout << "Test 1: should see '1 2', '5 1' and '1 2'" << endl;
    cout << old[1]->c << " " << old[1]->e << endl;
    for(int i = 0; i < 4; i++)
        old[1]++;
    cout << old[1]->c << " " << old[1]->e << endl;
    for(int i = 0; i < 20; i++)
        old[1]++;
    cout << old[1]->c << " " << old[1]->e << endl;

    cout << "Test 2: should see '1 2' followed by '1 2'" << endl;
    cout << old[2]->c << " " << old[2] -> e << endl;
    for(int i = 0; i <= 24; i++)
        old[2]++;
    *old[1] = *old[2];
    cout << old[1]->c << " " << old[1]->e << endl;

    cout << "Test 3: should see 2925" << endl;
    old[2] -> assign(&x);
    int total = 0;
    for(int i = 0; i <= 24; i++)
        total += old[2]->ne + old[2]->n + old[2]->nw + old[2]->e + old[2]->c + old[2]->w + old[2]->se + old[2]->s + old[2]->sw;
    cout << total << endl;
     
    return 0;
}


#include <iostream>
#include<string>
#include "configf.h"

using namespace std;

int main(){
    string output;
    Configf* inputf = new Configf("find_test.conf");
    output = inputf -> Find("Couple 7*nu");
    cout << output << endl;
    output = inputf -> Find("Couple 7");
    cout << output << endl;
    
    return 0;
}

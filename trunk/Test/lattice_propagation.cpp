#include<iomanip>
using std::setw;
#include<vector>
using std::vector;
#include<iostream>
using std::cout;
using std::endl;
#include"../stencil.h"

int main(void)
{
	const int nodes = 25; const int longside = 5;
	vector<double> m(nodes,0); m[12] = 1;
	Stencil stencil(nodes,longside); stencil.assign(&m);
	for( int t=0; t<10; t++ ) {
		vector<double> temp(nodes,0);
		for( int i=0; i<nodes; i++, stencil++ )
			temp[i] += stencil.n() +stencil.s() +stencil.e() +stencil.w()
				+stencil.nw() +stencil.sw() +stencil.ne() +stencil.se();
		for( int i=0; i<nodes; i++ )
			m[i] += temp[i];

		for( int j=0; j<longside; j++ ) {
			for( int i=0; i<longside; i++ )
				cout<<"|"<<setw(2)<<m[i+j*longside];
			cout<<"|"<<endl;
		}
		cout<<endl;
	}
}

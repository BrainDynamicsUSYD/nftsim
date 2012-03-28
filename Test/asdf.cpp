#include<iostream>
using std::cout;
using std::endl;
#include<vector>
using std::vector;
#include<cmath>

int main(void)
{
	const int nodes = 25; const int side = sqrt(nodes);
	vector<int> grid(nodes,0);
	if( side%2 == 0 ) // even
	{
		for( int t=0; t<=side/2; t++ ) {
			for( int i=side/2-t; i<side/2+t; i++ )
				for( int j=side/2-t; j<side/2+t; j++ )
					grid[i+j*side] = 1;
			for( int j=0; j<side; j++ ) {
				for( int i=0; i<side; i++ )
					cout<<"|"<<grid[i+j*side];
				cout<<"|"<<endl;
			}
			cout<<endl;
		}
	} else // odd
	{
		for( int t=0; t<=side/2+1; t++ ) {
			for( int i=side/2-t+1; i<side/2+t; i++ )
				for( int j=side/2-t+1; j<side/2+t; j++ )
					grid[i+j*side] = 1;
			for( int j=0; j<side; j++ ) {
				for( int i=0; i<side; i++ )
					cout<<"|"<<grid[i+j*side];
				cout<<"|"<<endl;
			}
			cout<<endl;
		}
	}
	return 0;
}

#include<iostream>
using std::cout;
#include<vector>
using std::vector;

int main(void) {
	const int max = 25;
	for( int i=0; i<max-1; i+= 2)
		cout<<i<<" "<<i+1<<std::endl;
	cout<<max-1<<std::endl;
	return 0;
}

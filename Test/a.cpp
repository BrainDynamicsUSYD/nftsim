#include<cstdlib>
#include<cmath>
#include<cstdio>

int main(void)
{
	int nodes = 2500; int width = 20;
	bool node[nodes];
	for( int i=0; i<nodes; i++ )
		node[i] = false;
	for(int i=int(sqrt(nodes))/2-width; i<int(sqrt(nodes))/2+width; i++)
		for(int j=int(sqrt(nodes))/2-width; j<int(sqrt(nodes))/2+width; j++)
			node[int(i+j*sqrt(nodes))] = true;
	for( int i=0; i<sqrt(nodes); i++ ) {
		for( int j=0; j<sqrt(nodes); j++ )
			if( node[int(i+j*sqrt(nodes))] ) printf("8");
			else printf(".");
		printf("\n");
	}
	return 0;
}

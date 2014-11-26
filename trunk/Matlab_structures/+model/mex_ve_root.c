#include "mex.h"
#include "math.h"
#include "stdio.h"
/*
 * mex_ve_root.c - example found in API guide
 *
 * clone of ve_root.m except about twice as fast
 */


double sigm(double a, double* b, double* c, double* d){
	double f;
	f=*d/(1+exp(-((a-*b)/(*c))));
	return f;
}

void ve_root(double* y, double* ve, double* nus, double* theta, double* sigma, double* qmax, mwSize nentries)
{
  mwSize i;
  double q;
  /*Decrement nus, then they can be indexed starting from 1
    that is, nus[1] refers to nuee same as Matlab even though
    it is the first element in the array - so normally would be nus[0]*/
   --nus; 
  for (i=0; i<=nentries-1; ++i,++ve,++y) {
  	q = sigm(*ve,theta,sigma,qmax);
	*y = sigm( nus[4] * q + nus[5] * sigm( nus[7] * q + (nus[8] / nus[3]) * (*ve-(nus[1]+nus[2])*q),theta,sigma,qmax)+nus[6],theta,sigma,qmax)+((nus[1]+nus[2])*q-*ve)/(nus[3]);
  }
}

/* the gateway function */
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
/* What is my calling syntax?
ve_root(x,p.theta,p.sigma,p.qmax,p.nus) so 5 arguments */
  
  double *x,*nus,*theta,*sigma,*qmax,*y;

  mwSize mrows,ncols,nentries;
  
  /*  check for proper number of arguments */
  /* NOTE: You do not need an else statement when using mexErrMsgTxt
     within an if statement, because it will never get to the else
     statement if mexErrMsgTxt is executed. (mexErrMsgTxt breaks you out of
     the MEX-file) */
  if(nrhs!=5) 
    mexErrMsgTxt("Five inputs required.");
  if(nlhs>1) 
    mexErrMsgTxt("One output required.");
  
  /*  get the inputs */
  x = mxGetPr(prhs[0]);
  nus = mxGetPr(prhs[1]);
  theta = mxGetPr(prhs[2]);
  sigma = mxGetPr(prhs[3]);
  qmax = mxGetPr(prhs[4]);

  
  /*  get the dimensions of the matrix input x */
  mrows = mxGetM(prhs[0]);
  ncols = mxGetN(prhs[0]);
  if(mrows>ncols)
  	nentries = mrows;
  else
  	nentries = ncols;
  
  /*  set the output pointer to the output matrix */
  plhs[0] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  
  /*  create a C pointer to a copy of the output matrix */
  y = mxGetPr(plhs[0]);
  
  /*  call the C subroutine */
  ve_root(y,x,nus,theta,sigma,qmax,nentries);
}


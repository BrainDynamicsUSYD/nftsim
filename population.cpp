#include<iostream>
using std::endl;
#include"population.h"

void Population::init( Configf& configf )
{
  if( qresponse ) { // neural population
    double Qinit; configf.param("Q",Qinit);
    if( !qhistory.size() )
      qhistory.push_back( vector<double>(nodes,Qinit) );
    else
      for( size_t i=0; i<qhistory.size(); i++ )
        qhistory[i].resize(nodes,Qinit);
    configf.param( "Firing", *qresponse );
  }
  else { // stimulus population
    //for( int i=0; i<3; i++ )
    if( !qhistory.size() )
      qhistory.push_back( vector<double>(nodes,0) );
    else
      for( size_t i=0; i<qhistory.size(); i++ )
        qhistory[i].resize(nodes,0);
    timeseries = new Timeseries(nodes,deltat,index);
    configf.param( "Stimulus", *timeseries );
  }
  settled = true;
}

void Population::restart( Restartf& restartf )
{
}

void Population::dump( Dumpf& dumpf ) const
{
}

Population::Population( int nodes, double deltat, int index )
    : NF(nodes,deltat,index),
	qkey(0), qresponse(0), timeseries(0), settled(false)
{
}

Population::~Population()
{
  if(qresponse) delete qresponse;
  if(timeseries) delete timeseries;
}

void Population::step(void)
{
  // move pointer to keyring
  qkey = (qkey+1) % qhistory.size();
  if( qresponse ) { // neural population
    qresponse->step();
    qresponse->fire( qhistory[qkey] );
  }
  else { // stimulus population
    for( int i=0; i<nodes; i++ )
      qhistory[qkey][i] = 0; // reset Q for stimulus
    timeseries->step();
    timeseries->fire( qhistory[qkey] );
  }
  /*if(index==1){
	  for( int i=0; i<qhistory.size(); i++ ){
		  std::cout.width(8);
		  std::cout<<qhistory[i][0]<<" ";
	  }
	  std::cout<<endl;
  }*/
}

const vector<double>& Population::Q( const Tau& tau) const
{
  if( tau.m.size() == 1 ){ // homogeneous tau
	  /*if(index==0) {
		  std::cout.width(7); std::cout<<"Tau: "<<tau.m[0]<<"\t";
		  for( int i=qkey-3; i<qkey+3; i++ )
			if(i>0 && uint(i)<qhistory.size())
			  std::cout<<qhistory[i][0]<<"\t";
			else
				std::cout<<"       "<<"\t";
		  std::cout<<"\tIndex:\t"<<(qkey-tau.m[0]+qhistory.size())%qhistory.size()<<"/"<<qhistory.size()<<",\tQ: "<<qhistory[(qkey-tau.m[0]+qhistory.size())%qhistory.size()][0]<<std::endl;
	  }*/
    return qhistory[(qkey-tau.m[0]+qhistory.size())%qhistory.size()];}
  else { // tau.m.size() == nodes, inhomogeneous tau
    static vector<double> temp(nodes);
    for( int i=0; i<nodes; i++ )
      temp[i] = qhistory
        [ tau.m[i]<=qkey ? qhistory.size()+qkey-tau.m[i] : qkey-tau.m[i] ][i];
    return temp;
  }
}

double Population::Qinit( Configf& configf ) const
{
  if( qresponse ) {
    string buffer = configf.find( label("Population ",index+1)+"*Q:");
    return atof(buffer.c_str());
  }
  else
    return 0;
}

const vector<double>& Population::V(void) const
{
  if( qresponse )
    return qresponse->V();
  else {
    std::cerr<<"Trying to access V of a stimulus population."<<endl;
    exit(EXIT_FAILURE);
  }
}

void Population::add2Dendrite( int index,
    const Propag& prepropag, const Couple& precouple )
{
  if( settled ) {
    std::cerr<<"Population is already settled, no more dendrites can be added!"
        <<endl;
    exit(EXIT_FAILURE);
  }

  if( !qresponse )
    qresponse = new QResponse(nodes,deltat,index);
  qresponse->add2Dendrite( index, prepropag, precouple );
}

void Population::growHistory( const Tau& tau )
{
  if( settled ) {
    std::cerr<<"Population is already settled, cannot resize firing history!"
        <<endl;
    exit(EXIT_FAILURE);
  }

  if( size_t(tau.max) > qhistory.size() )
    qhistory.resize( tau.max+1 );
}

vector<Output*> Population::output(void) const
{
  Output* temp = new Output( label("Pop.",index+1)+".V", V() );
  return vector<Output*>(1,temp);
}

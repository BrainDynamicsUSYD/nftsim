#include"cascade.h"

void Cascade::CascadeDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y == { binding, H, input, nu, x, y }
  CaDE::rhs(y,dydt); // CaDE is a 2-state model with rates _x(Ca) and _y(Ca)
  // if this is not the last cascade, this is not responsible for LTP/LTD
  if( temporary )
    dydt[3] -= y[5]*max; // this term extend domain from [0,max] to [-max,max]
}

void Cascade::CascadeDE::init( Configf& configf )
{
  configf.param("nu_max",max);
  configf.param("Dth",dth);
  configf.param("Pth",pth);
  configf.param("xyth",th);
  configf.param("x",ltp);
  configf.param("y",ltd);
}

double Cascade::CascadeDE::_x( double Ca ) const
{
  // Ca is overloaded and is actually input
  return th+ltp*sig(Ca-pth,5);
  //return (Ca>0)?ltp*Ca:th;
}

double Cascade::CascadeDE::_y( double Ca ) const
{
  // Ca is overloaded and is actually input
  return th+ltd*sig(dth-Ca,5);
  //return (Ca<0)?-ltd*Ca:th;
}

Cascade::Cascade( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop, Cascade* nextcascade )
    : CaDP(nodes,deltat,index,glu,prepropag,postpop), nextcascade(nextcascade)
{
  delete de; delete rk4;
  de = new CascadeDE(nodes,deltat,nextcascade);
  rk4 = new RK4(*de);
}

Cascade::~Cascade(void)
{
}

void Cascade::input( vector<double>& input, double max )
{
  for( int i=0; i<nodes; i++ )
    (*de)[2][i] = input[i]/max;
}

void Cascade::initnu( double initnu )
{
  de->pos = pos = (initnu>0)?1:-1;
  if( nextcascade )
    nextcascade->initnu(initnu);
  else
    for( int i=0; i<nodes; i++ )
      (*de)[3][i] = initnu;
}

void Cascade::step(void)
{
  CaDP::step();
  if( nextcascade ) {
    nextcascade->input( (*de)[3], de->max );
    nextcascade->step();
    for( int i=0; i<nodes; i++ )
      n[i] = (*de)[3][i] +nextcascade->nu()[i];
  }
  else
    for( int i=0; i<nodes; i++ )
      n[i] = (*de)[3][i];
}

void Cascade::output( Output& output ) const
{
  output("nu",(*de)[3]);
}

const vector<double>& Cascade::nu(void) const
{
  return n;
}

void CaCascade::CaCascadeDE::rhs( const vector<double>& y, vector<double>& dydt )
{
  // y == { binding, H, Ca, nu, x, y }
  CaDE::rhs(y,dydt);   // as before
  dydt[3] -= y[5]*max; // this term extend domain from [0,max] to [-max,max]
}

void CaCascade::init( Configf& configf )
{
  int cascades=1; configf.param("Inter-cascades",cascades);
  cascade.resize(cascades+1);
  cascade[cascades] = new Cascade(nodes,deltat,index,glu,prepropag,postpop,0);
  for( int i=cascades-1; i>=0; i-- )
    cascade[i] = new Cascade(nodes,deltat,index,glu,prepropag,postpop,cascade[i+1]);

  CaDP::init(configf);

  for( unsigned int i=0; i<cascade.size(); i++ )
    configf>>*cascade[i];
  cascade[0]->initnu( (*de)[3][0] );
  for( int i=0; i<nodes; i++ )
    (*de)[3][i] = 0;
}

CaCascade::CaCascade( int nodes, double deltat, int index, const vector<double>& glu,
          const Propag& prepropag, const Population& postpop )
      : CaDP(nodes,deltat,index,glu,prepropag,postpop)
{
  delete de;  de  = new CaCascadeDE(nodes,deltat);
  delete rk4; rk4 = new RK4(*de);
}

CaCascade::~CaCascade(void)
{
  for( unsigned int i=0; i<cascade.size(); i++ )
    delete cascade[i];
}

void CaCascade::step(void)
{
  CaDP::step();
  cascade[0]->input( (*de)[3], de->max );
  cascade[0]->step();

  for( int i=0; i<nodes; i++ ) {
    n[i] = (*de)[3][i] +cascade[0]->nu()[i];
    if( pos*n[i]<0 )
      n[i] = 0;
  }
}

void CaCascade::output( Output& output ) const
{
  output("Couple",index+1,"Ca",(*de)[2]);
  output("Couple",index+1,"nu",n);
  output( label("Cascade.",index+1), 0, "nu",(*de)[3] );
  for( unsigned int i=0; i<cascade.size(); i++ ) {
    output.prefix( label("Cascade.",index+1), i+1 );
    cascade[i]->output(output);
  }
}

const vector<double>& CaCascade::nu(void) const
{
  return n;
}

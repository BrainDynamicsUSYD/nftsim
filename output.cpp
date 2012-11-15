#include"output.h"
using std::endl;
#include"configf.h"

vector<int> Outlet::node; // vector of nodes to output
Dumpf Outlet::dumpf; // file to dump

Outlet::Outlet( const string& name, const vector<double>& field )
    : name(name), field(field)
{
}

const string& Outlet::fieldname(void) const
{
  return name;
}

void Outlet::step(void) const
{
  for( size_t i=0; i<node.size(); i++ )
    dumpf<<space<<field[node[i]]<<space<<space;
  dumpf<<septor;
}

Output::Output( const string& key )
    : key(key)
{
}

void Output::operator() ( const string& name, const vector<double>& field )
{
  if( key=="" || key==name )
    outputs.push_back( new Outlet( m_prefix+name, field ) );
}

void Output::prefix( const string& object, int index )
{
  m_prefix = label(object+".",index) +".";
}

void Output::operator() ( const string& object, int index,
          const string& name, const vector<double>& field )
{
  prefix(object,index);
  (*this)(name,field);
}

bool Output::empty(void) const
{
  return outputs.empty();
}

Output::operator vector<Outlet*>(void) const
//vector<Output*> Output::operator() (void) const;
{
  return outputs;
}

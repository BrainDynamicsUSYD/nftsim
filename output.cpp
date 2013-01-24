#include"output.h"
using std::endl;
#include"configf.h"

vector<int> Outlet::node; // vector of nodes to output
Dumpf Outlet::dumpf; // file to dump

Outlet::Outlet( const string& name, const vector<double>& field, bool single_output )
    : name(name), field(field), single_output(single_output)
{
}

const string& Outlet::fieldname(void) const
{
  return name;
}

void Outlet::step(void) const
{
  if( single_output )
    dumpf<<space<<field[0]<<space<<space<<septor;
  else {
    for( size_t i=0; i<node.size(); i++ )
      dumpf<<space<<field[node[i]];
    dumpf<<space<<space<<septor;
  }
}

void Outlet::writeName(void) const
{
  if( single_output )
    dumpf<<space<<space<<setw<<name;
  else {
    for( size_t i=0; i<Outlet::node.size(); i++ )
      dumpf<<space<<space<<setw<<name;
  }
  dumpf<<space<<space<<septor;
}

void Outlet::writeNode(void) const
{
  if( single_output )
    dumpf<<space<<space<<setw<<1;
  else {
    for( size_t i=0; i<Outlet::node.size(); i++ )
      dumpf<<space<<space<<setw<<Outlet::node[i]+1;
  }
  dumpf<<space<<space<<septor;
}


Output::Output( const string& key )
    : key(key)
{
}

void Output::operator() ( const string& name, const vector<double>& field, bool single_output )
{
  if( key=="" || key==name )
    outputs.push_back( new Outlet( m_prefix+name, field, single_output ) );
}

void Output::prefix( const string& object, int index )
{
  m_prefix = label(object+".",index) +".";
}

void Output::operator() ( const string& object, int index,
          const string& name, const vector<double>& field, bool single_output )
{
  prefix(object,index);
  (*this)(name,field,single_output);
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

#include"output.h"
#include"configf.h"
#include"dumpf.h"

Outlet::Outlet( const string& name, const vector<double>& field, bool single_output )
    : name(name), field(field), single_output(single_output)
{
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
{
  return outputs;
}

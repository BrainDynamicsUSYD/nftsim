#include <utility>

#include"output.h"
#include"configf.h"
#include"dumpf.h"

Outlet::Outlet( string  name, const vector<double>& field, bool single_output )
  : name(std::move(name)), field(field), single_output(single_output) {
}

Output::Output( string  key )
  : key(std::move(key)) {
}

void Output::operator() ( const string& name, const vector<double>& field ) {
  if( key=="" || key==name ) {
    outputs.push_back( new Outlet( m_prefix+name, field, false ) );
  }
}

void Output::prefix( const string& object, int index ) {
  m_prefix = label(object+".",index) +".";
}

void Output::operator() ( const string& object, int index,
                          const string& name, const vector<double>& field ) {
  prefix(object,index);
  (*this)(name,field);
}

void Output::singleNode ( const string& name, const vector<double>& field ) {
  if( key=="" || key==name ) {
    outputs.push_back( new Outlet( m_prefix+name, field, true ) );
  }
}

void Output::singleNode (const string& object, int index, const string& name, const vector<double>& field ) {
  prefix(object, index);
  singleNode(name,field);
}

bool Output::empty() const {
  return outputs.empty();
}

Output::operator vector<Outlet*>() const {
  return outputs;
}

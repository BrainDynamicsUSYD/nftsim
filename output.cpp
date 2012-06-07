#include"output.h"
using std::endl;

vector<int> Output::node; // vector of nodes to output
Dumpf Output::dumpf; // file to dump

Output::Output( const string& name, const vector<double>& field )
    : name(name), field(field)
{
}

const string& Output::fieldname(void) const
{
  return name;
}

void Output::step(void) const
{
  for( size_t i=0; i<node.size(); i++ )
    dumpf<<space<<field[node[i]]<<space<<space;
  dumpf<<septor;
}

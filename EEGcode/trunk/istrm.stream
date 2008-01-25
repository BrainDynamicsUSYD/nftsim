/***************************************************************************
                          istrm.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "istrm.h"
#include <string>
using std::string;
#include <iostream>
#include <sstream>
using std::stringstream;
#include<cstring>

Istrm::Istrm(const char* filename):ifstream(filename,ios::in){
  pbuffer = new char[200];
}

Istrm::~Istrm(){
  delete [ ] pbuffer;
}
 
void Istrm::validate(const char* check, char delim){
  this->getline(pbuffer, 200, delim);
  char* p;
  if(pbuffer || check){
   p=pbuffer+std::strlen(pbuffer)-std::strlen(check);
   if(std::strcmp(p,check)){
     cerr << "Unable to find next input variable :'";
     cerr.write(check,std::strlen(check));
     cerr << "' "<< endl;
     cerr << "Please check eegcode.conf for correct sequence of parameters" << endl;
     exit(EXIT_FAILURE);
   }
  } else {
    cerr << "Validate function of Istrm has failed" << endl;
    exit(EXIT_FAILURE);
  }
}

int Istrm::choose(const char* ch, char delim){
  string choice;
  string list(ch);
  stringstream ss(stringstream::in | stringstream::out);
  int retval;

  if (32==this->peek()) this->ignore(1); //remove space character if there is one
  std::getline(*this, choice, delim);
  std::string::size_type indx;
  indx=list.find(choice)+choice.length()+1; // Find index to start of choice number
  ss << list.substr(indx); // Pass choice number to stringstream to convert to int
  ss >> retval; // extract choice number from string stream as int
  return retval;
}

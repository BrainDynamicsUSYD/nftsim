/***************************************************************************
                          istrm.cpp  -  description
                             -------------------
    copyright            : (C) 2005 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "istrm.h"

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
   p=pbuffer+strlen(pbuffer)-strlen(check);
   if(strcmp(p,check)){
     cerr << "Unable to find next input variable :'";
     cerr.write(check,strlen(check));
     cerr << "' "<< endl;
     cerr << "Please check eegcode.conf for correct sequence of parameters" << endl;
     exit(EXIT_FAILURE);
   }
  } else {
    cerr << "Validate function of Istrm has failed" << endl;
    exit(EXIT_FAILURE);
  }
}

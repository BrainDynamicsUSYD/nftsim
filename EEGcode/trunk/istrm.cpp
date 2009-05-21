/***************************************************************************
                          istrm.cpp  -  Extensions to input streams for parsing
			                the input files
                             -------------------
    copyright            : (C) 2009 by Peter Drysdale
    email                : peter@physics.usyd.edu.au
 ***************************************************************************/

#include "istrm.h"
#include <string>
#include <iostream>
#include <sstream>
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
     cerr << "Last read was :'";
     cerr.write(pbuffer,std::strlen(pbuffer));
     cerr << "' "<< endl;
     cerr << "Please check eegcode.conf for correct sequence of parameters" << endl;
     exit(EXIT_FAILURE);
   }
  } else {
    cerr << "Validate function of Istrm has failed" << endl;
    exit(EXIT_FAILURE);
  }
}

double Istrm::find(const char* check, char delim, int ordinal){
  double retval;
  std::streampos sp;
  char* p; // pointer to start of compare portion of pbuffer
  
  if(!check){
    cerr << "Find function of Istrm has failed no search string" << endl;
    exit(EXIT_FAILURE);   
  }
  sp = this->tellg(); // Save current position in input file
  this->seekg(std::ios::beg); // Move file pointer to start of input file
  while(ordinal>0){
    this->getline(pbuffer, 200, delim);
    if( this->eof() ){
      cerr << "Find function of Istrm has failed" << endl;
      exit(EXIT_FAILURE);
    }    
    if(!pbuffer){
      cerr << "Find function of Istrm has failed" << endl;
      exit(EXIT_FAILURE);
    }
    p=pbuffer+std::strlen(pbuffer)-std::strlen(check);
    if(std::strcmp(p,check)){
      ;
    } else {
      ordinal--;
    }
  }
  *this >> retval;
  this->seekg(sp); // Restore current position in input file
  return retval;
}

int Istrm::choose(const char* ch, char delim){
  std::string choice;
  std::string list(ch);
  std::stringstream ss(std::stringstream::in | std::stringstream::out);
  int retval;

  if (32==this->peek()) this->ignore(1); //remove space character if there is one
  std::getline(*this, choice, delim);
  std::string::size_type indx;
  indx=list.find(choice)+choice.length()+1; // Find index to start of choice number
  ss << list.substr(indx); // Pass choice number to stringstream to convert to int
  ss >> retval; // extract choice number from string stream as int
  return retval;
}

int Istrm::readtauab(double deltat){
  int optionnum;
  optionnum=this->choose("Tauab:1 Tauabt:2",58);
  int tauab;
  float tauabfloat;
  if(1==optionnum){
    *this >> tauabfloat;
  }
  if(2==optionnum){
    double tauabt;
    *this >> tauabt;
    tauabfloat=tauabt/deltat;
  }
  if( !((1==optionnum)||(2==optionnum)) ){
    cerr << "Last read looking for Tauab or Tauabt found neither" << endl;
    exit(EXIT_FAILURE);
  }
  tauab=int(tauabfloat);
  if(tauabfloat<1 && tauabfloat>0){
    cerr << "Last read Tauab: " << tauabfloat << endl;
    cerr << "Tauab must be greater than 1 as it is measured in" << endl;
    cerr << "time steps not a time measured in seconds" << endl;
    exit(EXIT_FAILURE);
  }
  return tauab;
 }

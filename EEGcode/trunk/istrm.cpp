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
#include<cstdlib>
using std::endl;

Istrm::Istrm(const char* filename):std::ifstream(filename,std::ios::in),
              pbuffer(0){
  pbuffer = new char[200];
}

Istrm::~Istrm(){
  delete [ ] pbuffer;
}

int Istrm::optional(const char* check,char delim){
  std::streampos sp;
  sp = this->tellg(); // Save current position in input file
  this->getline(pbuffer, 200, delim);
  char* p;
  p=pbuffer+std::strlen(pbuffer)-std::strlen(check);
  if(std::strlen(pbuffer)<std::strlen(check) || (std::strcmp(p,check)) ){
    this->seekg(sp); // Restore current position in input file
    return 0;
  } else {
    return 1;
  }
}
 
void Istrm::validate(const char* check,char delim){
  this->getline(pbuffer, 200, delim);
  char* p;
  if(pbuffer || check){
   p=pbuffer+std::strlen(pbuffer)-std::strlen(check);
   if(std::strlen(pbuffer)<std::strlen(check) || std::strcmp(p,check)){
     std::cerr << "Unable to find next input variable :'";
     std::cerr.write(check,std::strlen(check));
     std::cerr << "' "<< endl;
     std::cerr << "Last read was :'";
     std::cerr.write(pbuffer,std::strlen(pbuffer));
     std::cerr << "' "<< endl;
     std::cerr << "Please check neurofield.conf for correct sequence of parameters" << endl;
     exit(EXIT_FAILURE);
   }
  } else {
    std::cerr << "Validate function of Istrm has failed" << endl;
    exit(EXIT_FAILURE);
  }
}

double Istrm::find(const char* check, char delim, int ordinal){
  double retval;
  std::streampos sp;
  char* p; // pointer to start of compare portion of pbuffer
  
  if(!check){
    std::cerr << "Find function of Istrm has failed no search string" << endl;
    exit(EXIT_FAILURE);   
  }
  sp = this->tellg(); // Save current position in input file
  this->seekg(std::ios::beg); // Move file pointer to start of input file
  while(ordinal>0){
    this->getline(pbuffer, 200, delim);
    if( this->eof() ){
      std::cerr << "Find function of Istrm has failed" << endl;
      exit(EXIT_FAILURE);
    }    
    if(!pbuffer){
      std::cerr << "Find function of Istrm has failed" << endl;
      exit(EXIT_FAILURE);
    }
    p=pbuffer+std::strlen(pbuffer)-std::strlen(check);
    if(std::strlen(pbuffer)<std::strlen(check) || std::strcmp(p,check)){
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

#include "Register.h"

using std::string;

Register::Register(std::string qWord, std::string lWord, std::string byte){
  _qWord = qWord;
  _lWord = lWord;
  _byte = byte;
}

const string &Register::name (unsigned nbytes) const{
  switch(nbytes){
    case 8:
      return _byte;
    break;
    case 32:
      return _lWord;
    break;
    case 64:
      return _qWord;
    break;
    default:
      return _qWord;
    break;
  }
}

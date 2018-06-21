/*
 * File:	Label.cpp
 *
 * Description:	This file contains the function definitions
 *		          for Label. Label increments a counter to provide unique
 *		          label names.
 *
 *
 *
 *
 */

# include <iostream>
# include "Label.h"

using std::ostream;

unsigned Label::_counter = 0;

Label::Label() {
  _number = _counter++;
}

unsigned Label::number() const {
  return _number;
}

ostream &operator <<(ostream &ostr, const Label &label) {
  return ostr << ".L" << label.number();
}

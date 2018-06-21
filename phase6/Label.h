/*
 * File:	Label.h
 *
 * Description:	This file contains the class definition Label.
 *		          Label increments a counter to provide unique
 *		          label names.
 *
 *
 *
 *
 */

# ifndef LABEL_H
# define LABEL_H

class Label {
  static unsigned _counter;
  unsigned _number;

public:
  Label();
  unsigned number() const;
};

std::ostream &operator <<(std::ostream &ostr, const Label &label);

# endif /* LABEL_H */

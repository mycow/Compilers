# ifndef REGISTER_H
# define REGISTER_H

# include <string>

class Register{
  std::string _qWord, _lWord, _byte;
public:
  Register(std::string qWord, std::string lWord, std::string byte);
  const std::string &name (unsigned nbytes) const;
};

# endif /* REGISTER_H */

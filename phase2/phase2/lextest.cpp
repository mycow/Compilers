# include <cctype>
# include <string>
# include <iostream>
# include "tokens.h"
# include "lexer.h"

using namespace std;

int main()
{
    int token;
    string label, buffer;

    while ((token = lexan(buffer)) != DONE) {
	if (AUTO <= token && token <= WHILE)
	    label = "keyword";

	else if (token == STRING)
	    label = "string";

	else if (token == ID)
	    label = "identifier";

	else if (token == NUM)
	    label = tolower(buffer[buffer.size() - 1]) == 'l' ? "long" : "int";

	else
	    label = "operator";

	cout << label << ":" << buffer << endl;
    }
}
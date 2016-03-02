#ifndef PARSER_H_
#define PARSER_H_

#include <string>

#include "syntax.h"
#include "error.h"

class Parser
{
private:
	std::string* kRaw;

public:
	Parser(std::string* raw);
	~Parser();
	void Parse();
};

#endif // PARSER_H_
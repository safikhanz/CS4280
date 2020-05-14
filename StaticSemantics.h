#ifndef STATICSEMANTICS_H
#define STATICSEMANTICS_H

#include "Parser.h"
#include <vector>
#include <functional>

// typedef struct Globals {
// 	std::vector<std::string> Variables;
// } Globals;

class StaticSemantics
{

	std::vector<std::string> Variables;
	// Node* root;

	void Insert(std::string);
	bool Verify(std::string);
	bool Traverse(Node *);

public:
	StaticSemantics(Parser p) {}
	bool doStatSem(Node *);
};

#endif
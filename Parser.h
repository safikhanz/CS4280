#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include <vector>

typedef struct Node
{
	std::string Label;
	std::vector<Token::Token> Tokens;
	std::vector<Node *> Leaves;
	Node(const std::string &lbl) : Label(lbl) {}
	Node() : Label("") {}
} Node;

class Parser
{

	Scanner scanner;
	Token::Token tk;

	Node *program();
	Node *block();
	Node *vars();
	Node *expr();
	Node *N();
	Node *A();
	Node *M();
	Node *R();
	Node *stats();
	Node *mStat();
	Node *stat();

	Node *in();
	Node *out();
	Node *if_f();
	Node *Loop();
	Node *Assign();
	Node *label();
	Node *goto_f();
	Node *RO();
	void error(std::string);
	void GetToken();
	void PrintToken();
	Node *NewNode(const std::string &);

public:
	Node *root;
	Parser() : root(0) {}
	Parser(std::FILE *fp) : scanner(fp) {}
	void GetFile(std::FILE *fp) { this->scanner.GetFile(fp); }
	bool Parse();
	void PreOrder(Node *, const std::string &);
	void Print(const std::string &);
};

#endif

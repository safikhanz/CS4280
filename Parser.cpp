#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <sstream>
#include "Parser.h"

const bool DEBUG = 0;

void Parser::error(std::string msg)
{
	std::printf("ERROR %s\n\tToken: ", msg.c_str());
	PrintToken();
	return;
}

void PrintNodeTokens(Node *node,
					 int lvl, const std::string &fileName)
{

	if (DEBUG)
		std::printf("%lu", node->Tokens.size());

	std::stringstream out;
	out << node->Label;

	if (node->Tokens.size() > 0)
	{
		out << " Tokens: ";
		for (unsigned int iter = 0; iter < node->Tokens.size() - 1; iter++)
		{
			out << node->Tokens[iter].tokenName << ", ";
		}

		out << node->Tokens[node->Tokens.size() - 1].tokenName;
	}

	for (int iter = 0; iter < lvl; ++iter)
		std::printf("	");

	std::printf("%s\n", out.str().c_str());
	std::fstream file;
	file.open(fileName, std::fstream::app);

	for (int iter = 0; iter < lvl; ++iter)
		file << "\t";

	file << out.str() << "\n";
	file.close();
}

void Parser::PreOrder(Node *node, const std::string &file)
{
	std::string fileName = file;
	std::function<void(Node *, int)> print = [&](Node *node, int lvl) {
		if (!node)
		{
			return;
		}
		PrintNodeTokens(node, lvl, fileName);
		for (unsigned int iter = 0; iter < node->Leaves.size(); iter++)
		{
			print(node->Leaves[iter], lvl + 1);
		}
	};
	print(node, 0);
}

void Parser::Print(const std::string &file)
{
	this->PreOrder(this->root, file);
}

//<vars>  ->  empty | declare Identifier :=  Integer  ;  <vars>
Node *Parser::vars()
{
	if (DEBUG)
		std::printf("-->vars\n");
	Node *token = this->NewNode("<vars>");

	if (this->tk.id == Token::DECLR_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::IDENT_tk)
		{
			//token = this->NewNode("<vars>");
			token->Tokens.push_back(this->tk);
			this->GetToken();

			if (this->tk.id == Token::colonequals_tk)
			{
				token->Tokens.push_back(this->tk);
				this->GetToken();

				if (this->tk.id == Token::NUM_tk)
				{
					token->Tokens.push_back(this->tk);
					this->GetToken();
					if (this->tk.id == Token::semicolon_tk)

						token->Tokens.push_back(this->tk);
					this->GetToken();

					Node *retrn = this->vars();

					if (retrn != nullptr)
					{
						token->Leaves.push_back(retrn);
						if (DEBUG)
						{
							std::printf("Vars-->\n");
						}
						return token;
					}
					else if (DEBUG)
					{
						std::printf("Vars: returned ε\n");
					}
				}
				else
					error("vars: Expected NUM_Tk");
			}
			else
				error("Vars: Expected colonequals_tk");
		}
		else
			error("Vars: Expected IDENT_Tk");
	}
	return token;
}

//<in> -> in  Identifier
Node *Parser::in()
{
	if (DEBUG)
		std::printf("-->In\n");
	Node *token = this->NewNode("<in>");

	if (this->tk.id == Token::IN_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::IDENT_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if (DEBUG)
			{
				std::printf("In-->\n");
			}
			return token;
		}
		else
			error("In: Expected IDENT_tk");
	}
	else
		error("In: Expected IN_tk");
	return nullptr;
}

//<R> -> ( <expr> ) | Identifier | Integer
Node *Parser::R()
{
	if (DEBUG)
		std::printf("-->R\n");

	Node *token = NewNode("<R>");

	// <R> -> ( <expr> )
	if (this->tk.id == Token::leftPar_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();
		Node *leaf = this->expr();

		if (leaf != nullptr)
		{

			token->Leaves.push_back(leaf);

			if (this->tk.id == Token::rightPar_tk)
			{
				token->Tokens.push_back(this->tk);
				this->GetToken();
				if (DEBUG)
				{
					std::printf("R-->\n");
				}
				return token;
			}
			else
			{
				error("R: Expected )");
			}
		}
		else
		{
			error("R: Expr returned nullptr");
		}
		return nullptr;
	}

	if (this->tk.id == Token::IDENT_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();
		if (DEBUG)
		{
			std::printf("R-->\n");
		}
		return token;
	}

	if (this->tk.id == Token::NUM_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();
		if (DEBUG)
		{
			std::printf("R-->\n");
		}
		return token;
	}

	error("R: Expected (, identifier, integer");
	return nullptr;
}

//<M>  -> * <M> |  <R>
Node *Parser::M()
{
	if (DEBUG)
		std::printf("-->M\n");

	Node *token = NewNode("<M>");

	if (this->tk.id == Token::multiply_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();
		Node *leaf = this->M();
		if (leaf != nullptr)
		{
			token->Leaves.push_back(leaf);
			if (DEBUG)
				printf("M-->\n");
			return token;
		}
		error("M: returned nullptr");
		return nullptr;
	}

	if (this->tk.id == Token::leftPar_tk ||
		this->tk.id == Token::IDENT_tk ||
		this->tk.id == Token::NUM_tk)
	{

		Node *leaf = this->R();
		if (leaf != nullptr)
		{
			token->Leaves.push_back(leaf);
			if (DEBUG)
				printf("M-->\n");
			return token;
		}
		error("M: returned nullptr");
		return nullptr;
	}

	error("M: Expected ( or *");
	return nullptr;
}

//<N> -> <A> / <N> | <A> * <N> | <A>
Node *Parser::N()
{
	if (DEBUG)
		std::printf("-->N\n");

	Node *token = this->NewNode("<N>");

	Node *leaf = this->A();

	if (this->tk.id == Token::slash_tk)
	{

		token->Tokens.push_back(this->tk);
		this->GetToken();
		leaf = this->N();

		if (leaf != nullptr)
		{
			token->Leaves.push_back(leaf);
		}
		else
		{
			error("N: Returned nullptr");
			return nullptr;
		}
	}
	else if (this->tk.id == Token::multiply_tk)
	{

		token->Tokens.push_back(this->tk);
		this->GetToken();
		leaf = this->N();

		if (leaf != nullptr)
		{
			token->Leaves.push_back(leaf);
		}
		else
		{
			error("N: Returned nullptr");
			return nullptr;
		}
	}

	if (leaf != nullptr)
	{
		if (DEBUG)
		{
			std::printf("N-->\n");
		}
		token->Leaves.push_back(leaf);
		return token;
	}

	error("N: returning nullptr");
	return nullptr;
}

// <A> ->  <M> + <A> | <M>
Node *Parser::A()
{
	if (DEBUG)
		std::printf("-->A\n");

	Node *token = this->NewNode("<A>");

	// <A> -> <N>
	if (this->tk.id == Token::multiply_tk ||
		this->tk.id == Token::leftPar_tk ||
		this->tk.id == Token::IDENT_tk ||
		this->tk.id == Token::NUM_tk)
	{
		Node *leaf = this->M();

		// <A> -> <M> + <A>
		if (this->tk.id == Token::plus_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			leaf = this->A();

			if (leaf != nullptr)
			{
				if (DEBUG)
				{
					std::printf("A-->\n");
				}
				token->Leaves.push_back(leaf);
				return token;
			}
		}

		if (leaf != nullptr)
		{
			if (DEBUG)
			{
				std::printf("A-->\n");
			}
			token->Leaves.push_back(leaf);
			return token;
		}
		// if(DEBUG){ std::printf("A-->\n"); }
		// return token;
		error("A: Something");
		return nullptr;
	}
	else
		error("A: Expected * ( identifier integer");
	return nullptr;
}

/*
<expr> -> <N> - <expr>  | <N>
<N>    -> <A> / <N> | <A> * <N> | <A>
<A>    -> <M> + <A> | <M>
<M>    -> * <M> |  <R>
<R>    -> ( <expr> ) | Identifier | Integer

*/
Node *Parser::expr()
{
	if (DEBUG)
		std::printf("-->Expr\n");

	Node *token = this->NewNode("<expr>");
	Node *leaf = this->N();
	if (leaf == nullptr)
		return nullptr;
	else
		token->Leaves.push_back(leaf);

	if (this->tk.id == Token::minus_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();
		leaf = this->expr();

		if (leaf != nullptr)
		{
			if (DEBUG)
			{
				std::printf("Expr-->\n");
			}
			token->Leaves.push_back(leaf);
			return token;
		}
		else
			error("Expr: Expr() returned nullptr");
	}

	if (DEBUG)
	{
		std::printf("Expr-->\n");
	}
	return token;
}

//<out> ->   out <expr>
Node *Parser::out()
{
	if (DEBUG)
		std::printf("-->out\n");
	Node *token = NewNode("<out>");
	//Node* leaf;

	if (this->tk.id == Token::OUT_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();
		Node *leaf = this->expr();
		if (leaf != nullptr)
		{
			token->Leaves.push_back(leaf);
			return token;
		}
		else
			error("out: Expr returned nullptr");
	}
	else
		error("out: Expected printTk");
	return nullptr;
}

//<RO> ->   < | <  <  (two tokens >)  | >
//        | >  > (two tokens) |  == (one token ==)
//		  | <  >    (two tokens)
Node *Parser::RO()
{
	if (DEBUG)
		std::printf("-->RO\n");
	// <
	if (this->tk.id == Token::lessThan_tk)
	{
		Node *token = this->NewNode("<RO>");
		token->Tokens.push_back(this->tk);
		this->GetToken();

		// < >
		if (this->tk.id == Token::greaterThan_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if (DEBUG)
			{
				std::printf("RO-->\n");
			}
			return token;
		}
		else
		{
			if (DEBUG)
			{
				std::printf("RO-->\n");
			}
			return token;
		}
	}

	// <  after
	if (this->tk.id == Token::lessThan_tk)
	{
		Node *token = this->NewNode("<RO>");
		token->Tokens.push_back(this->tk);
		this->GetToken();

		// < <
		if (this->tk.id == Token::lessThan_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if (DEBUG)
			{
				std::printf("RO-->\n");
			}
			return token;
		}
		else
		{
			if (DEBUG)
			{
				std::printf("RO-->\n");
			}
			return token;
		}

		// < >
		if (this->tk.id == Token::greaterThan_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if (DEBUG)
			{
				std::printf("RO-->\n");
			}
			return token;
		}
		else
		{
			if (DEBUG)
			{
				std::printf("RO-->\n");
			}
			return token;
		}
	}

	// >
	if (this->tk.id == Token::greaterThan_tk)
	{
		Node *token = this->NewNode("<RO>");
		token->Tokens.push_back(this->tk);
		this->GetToken();
		// > >
		if (this->tk.id == Token::greaterThan_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if (DEBUG)
			{
				std::printf("RO-->\n");
			}
			return token;
		}
		else
		{
			if (DEBUG)
			{
				std::printf("RO-->\n");
			}
			return token;
		}
	}

	error("RO: Expected < > =");
	return nullptr;
}

//<if>  -> iffy [ <expr> <RO> <expr> ]then <stat>
Node *Parser::if_f()
{
	if (DEBUG)
		std::printf("-->If\n");
	Node *token = this->NewNode("<if>");
	Node *leaf;

	if (this->tk.id == Token::IFFY_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::leftBracket_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			leaf = this->expr();

			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				leaf = this->RO();

				if (leaf != nullptr)
				{
					token->Leaves.push_back(leaf);
					leaf = this->expr();

					if (leaf != nullptr)
					{
						token->Leaves.push_back(leaf);

						if (this->tk.id == Token::rightBracket_tk)
						{
							token->Tokens.push_back(this->tk);
							if (this->tk.id == Token::THEN_tk)
							{
								token->Tokens.push_back(this->tk);
								this->GetToken();
								leaf = this->stat();

								if (leaf != nullptr)
								{
									token->Leaves.push_back(leaf);
									if (DEBUG)
									{
										std::printf("If-->\n");
									}
									return token;
								}
								else
									error("If: Stat() returned nullptr");
							}
							else
							{
								error("Iffy: Expected then");
							}
						}
						else
							error("If: Expected ]");
					}
					else
						error("If: Expr() returned nullptr");
				}
				else
					error("If: RO() returned nullptr");
			}
			else
				error("If: Expr() returned nullptr");
		}
		else
			error("If: Expected [");
	}
	else
		error("If: Expected iffy");
	return nullptr;
}

//<loop> ->  loop [ <expr> <RO> <expr> ]  <stat>
Node *Parser::Loop()
{
	if (DEBUG)
		std::printf("-->Loop\n");
	Node *token = this->NewNode("<loop>");
	Node *leaf;

	if (this->tk.id == Token::LOOP_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::leftBracket_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			leaf = this->expr();

			if (leaf != nullptr)
			{
				//token = this->NewNode("<loop>");
				token->Leaves.push_back(leaf);
				leaf = this->RO();

				if (leaf != nullptr)
				{
					token->Leaves.push_back(leaf);
					leaf = this->expr();

					if (leaf != nullptr)
					{
						token->Leaves.push_back(leaf);

						if (this->tk.id == Token::rightBracket_tk)
						{
							token->Tokens.push_back(this->tk);
							this->GetToken();
							leaf = this->stat();

							if (leaf != nullptr)
							{
								token->Leaves.push_back(leaf);
								if (DEBUG)
									std::printf("Loop-->");
								return token;
							}
							else
								error("Loop: Stat() returned nullptr");
						}
						else
							error("Loop: Expected ]");
					}
					else
						error("Loop: Expr() returned nullptr");
				}
				else
					error("Loop: RO() returned nullptr");
			}
			else
				error("Loop: Expr() returned nullptr");
		}
		else
			error("Loop: Expected [");
	}
	else
		error("Loop: Expected loop");
	return nullptr;
}

//<assign> ->  Identifier  := <expr>
Node *Parser::Assign()
{
	if (DEBUG)
		std::printf("-->Assign\n");

	Node *token = this->NewNode("<assign>");
	if (this->tk.id == Token::IDENT_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::colonequals_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			Node *leaf = this->expr();
			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				if (DEBUG)
				{
					std::printf("Assign-->");
				}
				return token;
			}
			else
			{
				error("Assign: Expr() returned nullptr;");
			}
		}
		else
		{
			error("Assign: Expected :=");
		}
	}
	else
	{
		error("Assign: Expected Identifier");
	}

	error("Assign: Returning nullptr");
	return nullptr;
}
// <label>         ->       label Identifier
Node *Parser::label()
{
	if (DEBUG)
		std::printf("-->label\n");
	Node *token = this->NewNode("<label>");

	if (this->tk.id == Token::LBL_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::IDENT_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if (DEBUG)
			{
				std::printf("label-->\n");
			}
			return token;
		}
		else
			error("In: Expected IDENT_tk");
	}
	else
		error("In: Expected LBL_tk");
	return nullptr;
}
//<goto>         ->        goto Identifier
Node *Parser::goto_f()
{
	if (DEBUG)
		std::printf("-->goto\n");
	Node *token = this->NewNode("<goto>");

	if (this->tk.id == Token::IN_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::IDENT_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if (DEBUG)
			{
				std::printf("goto-->\n");
			}
			return token;
		}
		else
			error("In: Expected IDENT_tk");
	}
	else
		error("In: Expected goto_tk");
	return nullptr;
}

//<stat>   -> <in>;| <out>;| <block> | <if>;| <loop>;| <assign>;
//                 |<goto>;| <label>;
Node *Parser::stat()
{
	if (DEBUG)
		std::printf("-->Stat\n");
	Node *token = this->NewNode("<stat>");

	if (this->tk.id == Token::IN_tk)
	{

		//this->GetToken();
		Node *leaf = this->in();
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::semicolon_tk)
		{

			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				if (DEBUG)
				{
					std::printf("Stat-->\n");
				}
				return token;
			}
		}
		else
		{
			error("Stat: <in> expected ;");
		}
	}
	else if (this->tk.id == Token::OUT_tk)
	{

		Node *leaf = this->out();
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::semicolon_tk)
		{

			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				if (DEBUG)
				{
					std::printf("Stat-->\n");
				}
				return token;
			}
		}
		else
		{
			error("Stat: <out> expected ;");
		}
	}
	else if (this->tk.id == Token::VOID_tk)
	{

		Node *leaf = this->block();
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::semicolon_tk)
		{

			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				if (DEBUG)
				{
					std::printf("Stat-->\n");
				}
				return token;
			}
		}
		else
		{
			error("Stat: <block> expected ;");
		}
	}
	else if (this->tk.id == Token::IFFY_tk)
	{

		Node *leaf = this->if_f();
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::semicolon_tk)
		{

			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				if (DEBUG)
				{
					std::printf("Stat-->\n");
				}
				return token;
			}
		}
		else
		{
			error("Stat: <if> expected ;");
		}
	}
	else if (this->tk.id == Token::LOOP_tk)
	{

		Node *leaf = this->Loop();
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::semicolon_tk)
		{

			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				if (DEBUG)
				{
					std::printf("Stat-->\n");
				}
				return token;
			}
		}
		else
		{
			error("Stat: <loop> expected ;");
		}
	}
	else if (this->tk.id == Token::IDENT_tk)
	{

		Node *leaf = this->Assign();
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::semicolon_tk)
		{

			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				if (DEBUG)
				{
					std::printf("Stat-->\n");
				}
				return token;
			}
		}
		else
		{
			error("Stat: <assign> expected ;");
		}
	}
	else if (this->tk.id == Token::GOTO_tk)
	{

		Node *leaf = this->goto_f();
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::semicolon_tk)
		{

			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				if (DEBUG)
				{
					std::printf("Stat-->\n");
				}
				return token;
			}
		}
		else
		{
			error("Stat: <goto> expected ;");
		}
	}
	else if (this->tk.id == Token::LBL_tk)
	{

		Node *leaf = this->label();
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if (this->tk.id == Token::semicolon_tk)
		{

			if (leaf != nullptr)
			{
				token->Leaves.push_back(leaf);
				if (DEBUG)
				{
					std::printf("Stat-->\n");
				}
				return token;
			}
		}
		else
		{
			error("Stat: <label> expected ;");
		}
	}

	error("Stat: No valid Stat tokens");
	return nullptr;
}

// <mStat>  ->  empty |  <stat>  <mStat>
Node *Parser::mStat()
{
	if (DEBUG)
		std::printf("-->MSstat\n");
	Node *token = this->NewNode("<mStat>");

	// <mStat> -> <stat>
	if (this->tk.id == Token::IN_tk ||
		this->tk.id == Token::OUT_tk ||
		this->tk.id == Token::VOID_tk ||
		this->tk.id == Token::IFFY_tk ||
		this->tk.id == Token::LOOP_tk ||
		this->tk.id == Token::IDENT_tk ||
		this->tk.id == Token::GOTO_tk ||
		this->tk.id == Token::LBL_tk)
	{

		Node *leaf = this->stat();

		if (leaf == nullptr)
		{
			error("MStat: Stat returned nullptr");
			return nullptr;
		}
		else
		{
			token->Leaves.push_back(leaf);

			this->GetToken();
			leaf = this->mStat();

			if (leaf == nullptr)
			{
				if (DEBUG)
				{
					std::printf("MStat: MStat returned empty");
				}
				return token;
			}
			else
			{
				token->Leaves.push_back(leaf);
				return token;
			}
		}
	}

	// MStat can be empty, so no error
	return nullptr;
}

//<stats>   ->  <stat>  <mStat>
Node *Parser::stats()
{
	if (DEBUG)
		std::printf("-->Stats\n");

	Node *token = this->NewNode("<stats>");
	//Node* leaf;
	Node *leaf = this->stat();
	if (leaf != nullptr)
	{
		token->Leaves.push_back(leaf);
		this->GetToken();
		leaf = this->mStat();

		if (leaf == nullptr)
		{
			if (DEBUG)
			{
				std::printf("MStats returned ε\n");
			}
			return token;
		}
		else
		{
			token->Leaves.push_back(leaf);
			if (DEBUG)
			{
				std::printf("Stats-->\n");
			}
			return token;
		}
	}
	error("Stats: Caught nothing and returning null");
	return nullptr;
}

//<block>   ->  { <vars> <stats> }
Node *Parser::block()
{
	if (DEBUG)
		std::printf("-->Block\n");

	Node *token = this->NewNode("<block>");
	Node *leaf;

	if (this->tk.id == Token::leftBrace_tk)
	{
		token->Tokens.push_back(this->tk);
		this->GetToken();
		leaf = this->vars();

		if (leaf == nullptr)
		{
			if (DEBUG)
			{
				printf("Block: Vars() returned ε\n");
			}
		}
		else
			token->Leaves.push_back(leaf);

		if (this->tk.id == Token::rightBrace_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if (DEBUG)
			{
				printf("Block: Returning token\n");
			}
			return token;
		}
	}

	leaf = this->stats();
	if (leaf != nullptr)
	{
		token->Leaves.push_back(leaf);

		if (this->tk.id == Token::rightBrace_tk)
		{
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if (DEBUG)
			{
				printf("Block: Returning token\n");
			}
			return token;
		}
	}
	return nullptr;
}

// <program>->	<vars> <block>
Node *Parser::program()
{
	if (DEBUG)
		std::printf("-->program\n");

	// <vars>
	Node *token = this->NewNode("<program>");
	if (this->tk.id == Token::DECLR_tk)
		
	Node *retrn = this->vars();

		if (retrn == nullptr)
		{
			if (DEBUG)
			{
				printf("Vars returned ε.\n");
			}
		}
		else
			token->Leaves.push_back(retrn);
	} /*
	else if(this->tk.id == Token::leftBrace_tk)
	{
		Node *retrn = this->vars();

		if (retrn == nullptr)
		{
			if (DEBUG)
			{
				printf("Vars returned ε.\n");
			}
		}
		else
			token->Leaves.push_back(retrn);
	}
*/
	// <block>
	if (this->tk.id == Token::leftBrace_tk)
	{
		Node *retrn = nullptr;
		retrn = this->block();

		if (retrn == nullptr)
			error("program: returned nullptr");
		else
		{
			token->Leaves.push_back(retrn);
			if (DEBUG)
			{
				printf("program-->\n");
			}
			return token;
		}
	}

	error("program: Expected var or block");
	return nullptr;
}

void Parser::GetToken()
{
	this->tk = this->scanner.getToken();
	if (DEBUG)
		PrintToken();
}

void Parser::PrintToken()
{
	printf("{ %s , "
		   "%s"
		   ", %2d }\n",
		   Token::tkList[tk.id].c_str(),
		   tk.tokenName.c_str(),
		   tk.line);
}

Node *Parser::NewNode(const std::string &lbl)
{
	Node *token = nullptr;
	token = new Node(lbl);
	if (!token)
	{
		error("NewNode: Error creating node");
		return nullptr;
	}
	return token;
}

bool Parser::Parse()
{
	if (DEBUG)
		std::printf("Parse: ");
	this->GetToken();
	this->root = this->program();
	if (this->tk.id == Token::EOF_tk)
		return true;
	return false;
}

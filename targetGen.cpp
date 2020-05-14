#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "targetGen.h"
#include "Parser.h"

std::vector<Token::Token> vars;
//std::vector<Token::Token> R0s;
std::vector<std::string> temps;

static std::string asmOut;
//static int tempHelper = 0;
static int label = 0;
static const bool DEBUG = 0;

int varSize()
{
	return (int)vars.size() - 1;
}

bool tokenSearch(Node *node, std::string input)
{
	for (int i = 0; i < ((int)node->Tokens.size() - (int)1); ++i)
	{
		if (node->Tokens[i].tokenName == input)
			return true;
	}
	return false;
}

int findVar(const std::string &input)
{
	for (int i = 0; i < (int)vars.size(); ++i)
	{
		if (vars[i].tokenName == input)
			return (i);
	}
	return -1;
}

void printToFile(const char *code, ...)
{
	FILE *file = NULL;
	file = fopen(asmOut.c_str(), "a");
	if (!file)
	{
		perror("Could not open asm file\n");
		return;
	}
	/*
	 * https://vcpptips.wordpress.com/tag/three-dots-as-parameter/
	 * va_* is used for retreiveing args passed to function
	 *
	 */
	va_list args;
	va_start(args, code);
	vfprintf(file, code, args);
	fprintf(file, "\n");
	va_end(args);
	std::fclose(file);
}

bool Traverse(Node *node)
{
	const auto Label = [&](const std::string &r) { return node->Label == r; };
	Token::Token token;
	Token::Token tempToken;

	if (Label("<program>"))
	{

		if (DEBUG)
		{
			printf("\tProgram:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (node->Leaves.size() == 2)
		{
			if (!Traverse(node->Leaves[0]))
			{
				return false;
			}
			if (!Traverse(node->Leaves[1]))
			{
				return false;
			}
			return true;
		}
		if (node->Leaves.size() == 1)
		{
			if (!Traverse(node->Leaves[0]))
			{
				return false;
			}
			return true;
		}

		// Should not get to this case
		return false;
	}
	else if (Label("<block>"))
	{

		if (DEBUG)
		{
			printf("\tblock:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (!Traverse(node->Leaves[0]))
		{
			return false;
		}
		if (!Traverse(node->Leaves[1]))
		{
			return false;
		}
		return true;
	}
	else if (Label("<vars>"))
	{

		if (DEBUG)
		{
			printf("\tvars:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (node->Leaves.empty())
		{
			if (DEBUG)
			{
				printf("Vars: empty node\n");
			}
			return true;
		}
		else
		{
			if (DEBUG)
			{
				printf("Vars: non-empty node\n");
			}

			vars.push_back(node->Tokens.back());
			vars.back().tokenName = node->Tokens[1].tokenName;

			std::string text = node->Tokens[3].tokenName;

			int number = std::atoi(text.c_str());

			vars.back().line = number;

			printToFile("LOAD V%d", varSize());
			if (!Traverse(node->Leaves[0]))
			{
				return false;
			}
			return true;
			//}
		}
	}
	/*
	 * expr's + / - result always in temp
	 */

	else if (Label("<expr>"))
	{

		if (DEBUG)
		{
			printf("\texpr:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (node->Leaves.size() == 2)
		{

			//if ( tokenSearch(node, "-") ) {
			if (node->Tokens[0].tokenName == "-")
			{
				if (!Traverse(node->Leaves[0]))
				{
					return false;
				}

				int exprHelper = 0;

				if (temps.empty())
				{
					temps.push_back("0");
					exprHelper = 1;
				}
				else
				{
					temps.push_back("0");
					exprHelper = (int)temps.size();
				}
				printToFile("STORE T%i", exprHelper);
				if (!Traverse(node->Leaves[1]))
				{
					return false;
				}
				printToFile("STORE V%i", varSize());
				printToFile("LOAD T%i", exprHelper);
				//printToFile("LOAD V%i", (int)vars.size() );
				printToFile("SUB V%i", varSize());
				return true;
			}

			else
			{
				printf("ERROR: expr has multiple args without -.\n");
				return false;
			}
		}
		else
		{
			if (!Traverse(node->Leaves[0]))
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		return false;
	}
	else if (Label("<A>"))
	{

		if (DEBUG)
		{
			printf("\tA:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (node->Leaves.size() == 2)
		{
			if (tokenSearch(node, "/"))
			{
				if (!Traverse(node->Leaves[0]))
				{
					return false;
				}
				printToFile("STORE V%i", varSize());
				if (!Traverse(node->Leaves[1]))
				{
					return false;
				}
				printToFile("STORE V%i", varSize());
				printToFile("LOAD V%i", (varSize() - 1));
				printToFile("DIV V%i", varSize());
				return true;
			}
		}
		else
		{
			if (!Traverse(node->Leaves[0]))
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		return false;
	}
	else if (Label("<N>"))
	{

		if (DEBUG)
		{
			printf("\tN:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (node->Leaves.size() == 2)
		{
			if (node->Tokens[0].tokenName == "*")
			{
				Traverse(node->Leaves[0]);
				printToFile("STORE V%i", varSize());
				Traverse(node->Leaves[1]);
				printToFile("MULT V%i", varSize());
				return true;
			}
		}
		else
		{
			if (!Traverse(node->Leaves[0]))
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		return false;
	}
	else if (Label("<M>"))
	{

		if (DEBUG)
		{
			printf("\tM:Leaves: %i\n", (int)node->Leaves.size());
			//printf("\tM:Tokens: %i\n", (int)node->Tokens.size());
		}

		if (node->Tokens.size() == 1)
		{

			printToFile("MULT -%i", 1);
			printToFile("STORE V%i", varSize());

			if (!Traverse(node->Leaves[0]))
			{
				return false;
			}

			return true;
		}

		if (!Traverse(node->Leaves[0]))
		{
			return false;
		}
		return true;
	}
	else if (Label("<R>"))
	{

		if (DEBUG)
		{
			printf("\tR:Leaves: %i\n", (int)node->Leaves.size());
			printf("\tR:Tokens: %i\n", (int)node->Tokens.size());
		}

		if (node->Leaves.empty())
		{
			//if(DEBUG) {
			//printf("R: leaves.empty case\n");
			//}
			//vars.push_back(node->Tokens[0]);
			//printf("vars.size(): %i\n", (int)vars.size() );

			if (node->Tokens[0].id == Token::INT_tk)
			{
				//printf("R: integer case\n");
				printToFile("LOAD %s", node->Tokens[0].tokenName.c_str());
				return true;
			}
			else
			{
				//printf("R: var case\n");
				//printToFile("STORE V%i", (int)vars.size() );
				int rHelper = 0;
				rHelper = findVar(node->Tokens[0].tokenName.c_str());
				printToFile("LOAD V%i", rHelper);
				return true;
			}
		}

		else
		{
			if (!Traverse(node->Leaves[0]))
			{
				return false;
			}
			return true;
		}

		return false;
	}
	else if (Label("<stats>"))
	{

		if (DEBUG)
		{
			printf("\tstats:Leaves: %i\n", (int)node->Leaves.size());
		}

		for (int i = 0; i < (int)node->Leaves.size(); ++i)
		{
			if (!Traverse(node->Leaves[i]))
			{
				return false;
			}
		}

		// if(!Traverse(node->Leaves[0]) ) {
		// 	return false;
		// }
		// if(!Traverse(node->Leaves[1]) ) {
		// 	return false;
		// }
		return true;
	}
	else if (Label("<mStat>"))
	{

		if (DEBUG)
		{
			printf("\tmStat:Leaves: %i\n", (int)node->Leaves.size());
		}

		for (int i = 0; i < (int)node->Leaves.size(); ++i)
		{
			if (!Traverse(node->Leaves[i]))
			{
				return false;
			}
		}
		return true;
	}
	else if (Label("<stat>"))
	{

		if (DEBUG)
		{
			printf("\tstat:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (!Traverse(node->Leaves[0]))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else if (Label("<in>"))
	{

		if (DEBUG)
		{
			printf("\tIn:Leaves: %i\n", (int)node->Leaves.size());
		}

		// vars.push_back(node->Tokens[1]);
		// vars.back().line = 0;
		int place = findVar(node->Tokens[1].tokenName.c_str());
		//printf("Place: %i\n", place);
		printToFile("READ V%i", place);
		printToFile("LOAD V%i", place);
		printToFile("STORE V%i", place);
		return true;
	}
	else if (Label("<out>"))
	{

		if (DEBUG)
		{
			printf("\tOut:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (!Traverse(node->Leaves[0]))
		{
			return false;
		}

		temps.push_back("0");
		printToFile("STORE T%i", (int)temps.size());
		printToFile("WRITE T%i", (int)temps.size());
		return true;
	}
	else if (Label("<if>"))
	{

		if (DEBUG)
		{
			printf("\tIf:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (!Traverse(node->Leaves[0]))
		{
			return false;
		}
		//tempHelper++;
		//int expr1 = tempHelper;
		temps.push_back("0");
		printToFile("STORE T%i", (int)temps.size());

		if (!Traverse(node->Leaves[2]))
		{
			return false;
		}

		printToFile("SUB T%i", (int)temps.size());
		label++;
		int label1 = label;

		if (node->Leaves[1]->Tokens.size() > 1)
		{

			if (node->Leaves[1]->Tokens[1].tokenName == "<")
			{
				if (node->Leaves[1]->Tokens[0].tokenName == "=")
				{
					printToFile("BRNEG label%i", label1);
				}
				else if (node->Leaves[1]->Tokens[0].tokenName == ">")
				{
					printToFile("BRZERO label%i", label1);
				}
			}
			else if (node->Leaves[1]->Tokens[1].tokenName == ">")
			{
				if (node->Leaves[1]->Tokens[0].tokenName == "=")
				{
					printToFile("BRPOS label%i", label1);
				}
			}
		}
		else
		{

			if (node->Leaves[1]->Tokens[0].tokenName == "<")
			{
				printToFile("BRZNEG label%i", label1);
			}
			if (node->Leaves[1]->Tokens[0].tokenName == ">")
			{
				printToFile("BRZPOS label%i", label1);
			}
			if (node->Leaves[1]->Tokens[0].tokenName == "=")
			{
				printToFile("BRNEG label%i", label1);
				printToFile("BRPOS label%i", label1);
			}
		}

		if (!Traverse(node->Leaves[3]))
		{
			return false;
		}
		printToFile("label%i: NOOP", label1);
		return true;
	}
	else if (Label("<loop>"))
	{

		if (DEBUG)
		{
			printf("\tLoop:Leaves: %i\n", (int)node->Leaves.size());
		}

		label++;
		int label1 = label;
		printToFile("label%i: NOOP", label1);

		if (!Traverse(node->Leaves[0]))
		{
			return false;
		}
		//tempHelper++;
		//int expr1 = tempHelper;
		temps.push_back("0");
		printToFile("STORE T%i", (int)temps.size());

		if (!Traverse(node->Leaves[2]))
		{
			return false;
		}

		printToFile("SUB T%i", (int)temps.size());
		label++;
		int label2 = label;

		if (node->Leaves[1]->Tokens.size() == 2)
		{
			//if(DEBUG)
			//printf("loop:node->Leaves[1]->Tokens.size(): %i\n", (int)node->Leaves[1]->Tokens.size() );

			// if(node->Leaves[0]->Tokens[0].tokenName == "=") {
			// 	if(node->Leaves[0]->Tokens[1])
			// }

			if (node->Leaves[1]->Tokens[1].tokenName == "<")
			{
				if (node->Leaves[1]->Tokens[0].tokenName == "=")
				{
					printToFile("BRNEG label%i", label2);
				}
				else if (node->Leaves[1]->Tokens[0].tokenName == ">")
				{
					printToFile("BRZERO label%i", label2);
				}
			}
			else if (node->Leaves[1]->Tokens[1].tokenName == ">")
			{
				if (node->Leaves[1]->Tokens[0].tokenName == "=")
				{
					printToFile("BRPOS label%i", label2);
				}
			}
		}
		else
		{

			if (node->Leaves[1]->Tokens[0].tokenName == "<")
			{
				printToFile("BRZNEG label%i", label2);
			}
			if (node->Leaves[1]->Tokens[0].tokenName == ">")
			{
				printToFile("BRZPOS label%i", label2);
			}
		}

		if (!Traverse(node->Leaves[3]))
		{
			return false;
		}
		printToFile("BR label%i", label1);
		printToFile("label%i: NOOP", label2);
		return true;
	}
	else if (Label("<assign>"))
	{

		if (DEBUG)
		{
			printf("\tAssign:Leaves: %i\n", (int)node->Leaves.size());
		}

		if (node->Leaves.size() != 1)
		{
			perror("Assign has no leaves\n");
			return false;
		}
		if (!Traverse(node->Leaves[0]))
		{
			return false;
		}
		else
		{
			int assignHelper = findVar(node->Tokens[0].tokenName.c_str());
			if (assignHelper != -1)
			{
				printToFile("STORE V%i", assignHelper);
				return true;
			}
		}
	}
	else if (Label("<goto>"))
	{

		if (DEBUG)
		{
			printf("\tIn:Leaves: %i\n", (int)node->Leaves.size());
		}

		// vars.push_back(node->Tokens[1]);
		// vars.back().line = 0;
		int place = findVar(node->Tokens[1].tokenName.c_str());
		//printf("Place: %i\n", place);
		printToFile("BR V%i", place);

		return true;
	}
	else if (Label("<RO>"))
	{
		return true;
	}
	printf("ERROR: failed to find label\n");
	return false;
}

void Compile(std::string &filename, std::FILE *f)
{
	Parser p(f);
	if (!p.Parse())
	{
		perror("Parsing failed\n");
		return;
	}
	else
	{
		p.PreOrder(p.root, filename);
		FILE *file = NULL;
		file = fopen(filename.c_str(), "w");
		std::fclose(file);
		asmOut = filename;
		if (!Traverse(p.root))
		{
			perror("Compiling failed\n");
		}
		else
		{
			printToFile("STOP");
			printf("%s\n", filename.c_str());

			if (!vars.empty())
			{
				for (int i = 0; i <= (int)vars.size() - 1; i++)
				{
					//printToFile("V%i %i", (i+1) , vars[i].line);
					printToFile("V%i %i", (i), vars[i].line);
				}
			}

			if (DEBUG)
			{
				printf("temps.size(): %i\n", (int)temps.size());
			}

			if (!temps.empty())
			{
				for (int i = 0; i < (int)temps.size(); i++)
				{
					printToFile("T%i %s", (i + 1), temps[i].c_str());
				}
			}
		}
	}
}

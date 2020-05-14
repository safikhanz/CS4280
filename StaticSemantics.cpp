#include "StaticSemantics.h"

const bool DEBUG = 0;
bool ERROR = 0;

bool StaticSemantics::doStatSem(Node *root)
{
	if (Traverse(root))
		return true;

	if (DEBUG)
		std::printf("statSem returning false\n");
	return false;
}

void StaticSemantics::Insert(std::string var)
{
	if (DEBUG)
		std::printf("Inserting: %s\n", var.c_str());
	Variables.push_back(var);
}

bool StaticSemantics::Verify(std::string var)
{
	if (DEBUG)
		std::printf("Verifying: %s\n\t", var.c_str());

	//Could do a simple == here
	int compare;
	for (unsigned int i = 0; i < Variables.size(); i++)
	{
		compare = Variables[i].compare(var);
		if (compare == 0)
		{
			if (DEBUG)
				std::printf("%s has already been declared\n", var.c_str());
			return true;
		}
	}
	return false;
}

bool StaticSemantics::Traverse(Node *node)
{

	std::function<bool(Node *, int)> find = [&](Node *node, int lvl) {
		if (ERROR)
		{
			return false;
		} //Exit in ERROR

		//Vars --> If re delaring error, else insert
		if (node->Label == "<vars>")
		{
			if (node->Tokens.size() > 0)
			{
				for (unsigned int it = 0; it < node->Tokens.size() - 1; it++)
				{
					if (node->Tokens[it].id == Token::IDENT_tk)
					{
						if (Variables.size() > 0)
						{
							if (Verify(node->Tokens[it].tokenName))
							{
								std::printf("ERROR: %s has already been declared, exiting\n",
											node->Tokens[it].tokenName.c_str());
								ERROR = 1;
								return false;
							}
							else
							{
								Insert(node->Tokens[it].tokenName);
							}
						}
						else
						{
							Insert(node->Tokens[it].tokenName);
						}
					}
				}
			}
			else
			{
				if (DEBUG)
					std::printf("%s has no tokens\n", node->Label.c_str());
			}
		}

		//<in><assign><R> --> if identifier has not been declared, error
		else if (node->Label == "<in>" || node->Label == "<assign>" || node->Label == "<R>")
		{
			if (node->Tokens.size() > 0)
			{
				for (unsigned int it = 0; it < node->Tokens.size(); it++)
				{
					if (node->Tokens[it].id == Token::IDENT_tk)
					{
						if (Verify(node->Tokens[it].tokenName))
						{
						}
						else
						{
							std::printf("ERROR: %s has not been defined, exiting\n",
										node->Tokens[it].tokenName.c_str());
							ERROR = 1;
							return false;
						}
					}
				}
			}
		}

		//Traverse the tree
		for (unsigned int i = 0; i < node->Leaves.size(); i++)
		{
			find(node->Leaves[i], lvl + 1);
		}
		return true;
	};
	find(node, 0);
	return true;
}

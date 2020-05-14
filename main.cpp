
#include <fstream>
#include <string>
#include <iostream>
#include <cstring>
#include "targetGen.h"

using namespace std;

int main(int argc, char **argv)
{
	string fileName;
	char *input_file_name;
	if (argc > 1)
	{ //Get the name of the input file as a string.
		string file_name = argv[1];

		//See if the input file name contains the '.sp20' extension. If
		//find() returns npos, then no matches were found, and the file
		//name does not have the .sp20 extension. If find() does not return
		//npos, then the position of the first character of the first
		//match of .sp20 in the file name was returned, so the .sp20 file name
		//extension is present.
		size_t index = file_name.find(".sp20");
		string input_file_str = file_name;
		string output_file_str = file_name;

		//If the input file does not include the .sp20 extension, then
		//append the .sp20 extension to the file name.
		if (index == string::npos)
		{
			//file_name.append(".sp20");
			input_file_str.append(".sp20");
		
			output_file_str.append(".asm");
		cout<< input_file_str << endl;
		cout<<output_file_str <<endl;
		}
		else
		{
			output_file_str = file_name.substr(0, index);
			output_file_str.append(".asm");
		}

		FILE *file = fopen(input_file_str.c_str(), "r");
		if (!file)
		{
			perror("Could not open file");
			fclose(file);
		}
		cout << "resched";
		Compile(output_file_str, file);
		fclose(file);
	}
	else
	{
		string fileout = "out.asm";
		Compile(fileout, stdin);
	}
	return 0;
}

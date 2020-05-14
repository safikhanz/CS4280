#include "Scanner.h"

const bool DEBUG = 0;
const bool ERRMSG = 1;

char delimAndOpArr[] = {
    ':=',
    '<',
    '>',
    '==',
    ':',
    '+',
    '-',
    '*',
    '/',
    '%',
    '.',
    '(',
    ')',
    ',',
    '{',
    '}',
    ';',
    '[',
    ']'};

std::string Keywords[] = {
    "label",
    "goto",
    "loop",
    "void",
    "declare",
    "return",
    "in",
    "out",
    "program",
    "iffy",
    "then",
    "assign",
    "data"

};

int Scanner::filter()
{

        if (!this->file)
        {
                perror("ERROR: File pointer invalid\n");
                return -1;
        }
        return 1;
}

Token::Token Scanner::getToken()
{

        Token::Token token;
        char input;
        if (line == 0)
                line++;

        while (1)
        {

                input = fgetc(file);

                // Check for tabs, spaces, newline(increment line if so), carriage return.
                while (isspace(input))
                {
                        if (input == '\n')
                                line++;

                        if (DEBUG)
                                std::printf("%c character found\n", input);

                        input = fgetc(file);
                }

                /*
                delimAndOpArr has the same values in Id and IdNames in Token.h
                However, Id and IdNames start with the keywords,
                so I need to skip over the starting keyword values.
                This is why line: token.id = static_cast<Token::Id>(11+i); has (11+i)
                */
                for (int i = 0; i <= 17; i++)
                {
                        if (input == delimAndOpArr[i])
                        {
                                buffer += input;
                                token.line = line;
                                token.tokenName = buffer;
                                buffer.clear();
                                token.id = static_cast<Token::tkID>(11 + i);
                                return token;
                        }
                }

                // EOF check
                if (input == EOF)
                {
                        token.line = line;
                        token.tokenName = "EOF";
                        buffer.clear();
                        token.id = Token::tkID::EOF_tk;
                        return token;
                }

                // Process Letter
                if (isalpha(input))
                {

                        // Process any letters or digits
                        while (isalnum(input))
                        {
                                //while ( ( isalpha(input) || isdigit(input) ) && input != ('\n' || '\r' || ' ') ) {
                                buffer += input;
                                input = fgetc(file);
                        }

                        /*
                        Check against Keywords array for matches
                        Keywords array matches beginning of enum Id and IdNames
                        So I cast token.id to i and it will match the Id enum
                        */
                        for (int i = 0; i <= 10; i++)
                        {
                                if (buffer == Keywords[i])
                                {
                                        token.line = line;
                                        token.tokenName = buffer;
                                        buffer.clear();
                                        token.id = static_cast<Token::tkID>(i);
                                        return token;
                                }
                        }

                        // If there is a space/newline after the word, valid id/keyword. Otherwise, error
                        // It would probably be wise to include EOF condition as well
                        if (isspace(input))
                        {
                                token.line = line;
                                token.tokenName = buffer;
                                buffer.clear();
                                token.id = Token::tkID::IDENT_tk;
                                return token;
                        }

                        token.line = line;
                        token.tokenName = "ERROR";
                        buffer.clear();
                        token.id = Token::tkID::ERR_tk;
                        if (ERRMSG)
                                std::printf("ERROR: '%c' found while processing letter\n", input);
                        return token;
                }

                // Process Digit
                if (isdigit(input))
                {

                        while (1)
                        {

                                if (input == ' ')
                                {
                                        token.line = line;
                                        token.tokenName = buffer;
                                        buffer.clear();
                                        token.id = Token::tkID::NUM_tk;
                                        return token;
                                }

                                if (input == '\n')
                                {
                                        token.line = line;
                                        token.tokenName = buffer;
                                        buffer.clear();
                                        token.id = Token::tkID::NUM_tk;
                                        return token;
                                }

                                // Check for letters
                                if (isalpha(input))
                                {
                                        token.line = line;
                                        token.tokenName = buffer;
                                        token.id = Token::tkID::ERR_tk;
                                        if (ERRMSG)
                                                std::printf("ERROR: '%c' found while processing digit\n", input);
                                        return token;
                                }

                                // Check for delim and op
                                for (int i = 0; i <= 17; i++)
                                {
                                        if (input == delimAndOpArr[i])
                                        {
                                                token.line = line;
                                                token.tokenName = "ERROR";
                                                buffer.clear();
                                                token.id = Token::tkID::ERR_tk;
                                                if (ERRMSG)
                                                        printf("ERROR: '%c' found while processing digit\n", input);
                                                return token;
                                        }
                                }

                                // If the last token is a number with no space at the end, need to catch EOF
                                if (input == EOF)
                                {
                                        token.line = line;
                                        token.tokenName = buffer;
                                        buffer.clear();
                                        token.id = Token::tkID::NUM_tk;
                                        fputc(input, file); // Put EOF back into the file to be caught
                                        return token;
                                }

                                buffer += input;
                                input = fgetc(file);
                        }
                }

                // Check for comment
                if (input == '#')
                {

                        // Read until I get next '#'
                        while (input != '#')
                                input = fgetc(file);

                        if (DEBUG)
                                printf("%c", input);

                        // If I reach the end of file instead, return
                        if (input == EOF)
                        {
                                token.line = line;
                                token.tokenName = "EOF";
                                buffer.clear();
                                token.id = Token::tkID::EOF_tk;
                                return token;
                        }
                }
                if (DEBUG)
                        std::printf("\n");
        }
}

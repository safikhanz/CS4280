#ifndef SCANNER_H
#define SCANNER_H

#include "Token.h"
#include <cstdio>

class Scanner {
    int line;
    std::string buffer;
    std::FILE* file;
    int filter();
public:
    Scanner() {
        this->file = 0;
        this->line = 0;
    }
    Scanner(std::FILE * fp) {
        this->file = fp;
        this->line = 0;
    }
    void GetFile(std::FILE* fp) {
        this->file=fp;
    }
    Token::Token getToken();
};

#endif

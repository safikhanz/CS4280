#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace Token
{
        enum tkID
        {
                IDENT_tk,
                NUM_tk,
                KW_tk,
                LINE_No,
                LBL_tk,
                GOTO_tk,
                LOOP_tk,
                VOID_tk,
                DECLR_tk,
                RETRN_tk,
                IN_tk,
                OUT_tk,
                PROGM_tk,
                IFFY_tk,
                THEN_tk,
                ASSGN_tk,
                DATA_tk,
                EOF_tk,
                ERR_tk,
                INT_tk,
                colonequals_tk,
                lessThan_tk,
                greaterThan_tk,
                doubleEqual_tk,
                colon_tk,
                plus_tk,
                minus_tk,
                multiply_tk,
                slash_tk,
                modulo_tk,
                period_tk,
                leftPar_tk,
                rightPar_tk,
                comma_tk,
                leftBrace_tk,
                rightBrace_tk,
                semicolon_tk,
                leftBracket_tk,
                rightBracket_tk

        };

        const std::string tkList[] = {
            "IDENT_tk", "NUM_tk", "KW_tk", "LINE_No", "LBL_tk", "GOTO_tk", "LOOP_tk", "VOID_tk",
            "DECLR_tk", "RETRN_tk", "IN_tk", "OUT_tk", "PROGM_tk", "IFFY_tk", "THEN_tk", "ASSGN_tk", "DATA_tk",
            "EOF_tk", "ERR_tk", "INT_tk", "colonequals_tk", "lessThan_tk", "greaterThan_tk", "doubleEqual_tk",
            "colon_tk", "plus_tk", "minus_tk", "multiply_tk", "slash_tk", "modulo_tk", "period_tk", "leftPar_tk",
            "rightPar_tk", "comma_tk", "leftBrace_tk", "rightBrace_tk", "semicolon_tk", "leftBracket_tk", "rightBracket_tk"

        };

        typedef struct Token_T
        {
                tkID id;
                std::string tokenName;
                int line;
        } Token;

} // namespace Token
#endif

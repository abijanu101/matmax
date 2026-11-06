/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_BUILD_PARSE_H_INCLUDED
# define YY_YY_BUILD_PARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 21 "src/parse.y"

    /* goes into parse.h, so the lexer also knows what an ASTNode is */
    #include "ast.h"

#line 54 "build/parse.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    MATRIX = 258,                  /* MATRIX  */
    TYPES = 259,                   /* TYPES  */
    OPS = 260,                     /* OPS  */
    SUBOPS = 261,                  /* SUBOPS  */
    INFER = 262,                   /* INFER  */
    DEFAULT = 263,                 /* DEFAULT  */
    CUSTOMS = 264,                 /* CUSTOMS  */
    ALL = 265,                     /* ALL  */
    FILETYPE = 266,                /* FILETYPE  */
    INT = 267,                     /* INT  */
    UINT = 268,                    /* UINT  */
    DOUBLE = 269,                  /* DOUBLE  */
    CHAR = 270,                    /* CHAR  */
    STRING = 271,                  /* STRING  */
    FOR = 272,                     /* FOR  */
    INRANGE = 273,                 /* INRANGE  */
    IF = 274,                      /* IF  */
    RET = 275,                     /* RET  */
    INIT = 276,                    /* INIT  */
    ZEROES = 277,                  /* ZEROES  */
    ENUMERATE = 278,               /* ENUMERATE  */
    RANDOM = 279,                  /* RANDOM  */
    ASSIGN = 280,                  /* ASSIGN  */
    TRAN = 281,                    /* TRAN  */
    APPLY = 282,                   /* APPLY  */
    SUM = 283,                     /* SUM  */
    MIN = 284,                     /* MIN  */
    MAX = 285,                     /* MAX  */
    LEN = 286,                     /* LEN  */
    ROWS = 287,                    /* ROWS  */
    COLS = 288,                    /* COLS  */
    HSTACK = 289,                  /* HSTACK  */
    VSTACK = 290,                  /* VSTACK  */
    COL = 291,                     /* COL  */
    ROW = 292,                     /* ROW  */
    WRITE = 293,                   /* WRITE  */
    STDOUT = 294,                  /* STDOUT  */
    STDIN = 295,                   /* STDIN  */
    EQ = 296,                      /* EQ  */
    GT = 297,                      /* GT  */
    GTE = 298,                     /* GTE  */
    LT = 299,                      /* LT  */
    LTE = 300,                     /* LTE  */
    PLUS = 301,                    /* PLUS  */
    MINUS = 302,                   /* MINUS  */
    MULTIPLY = 303,                /* MULTIPLY  */
    DIVIDE = 304,                  /* DIVIDE  */
    MOD = 305,                     /* MOD  */
    COMMA = 306,                   /* COMMA  */
    COLON = 307,                   /* COLON  */
    SEMICOLON = 308,               /* SEMICOLON  */
    LBRACKET = 309,                /* LBRACKET  */
    RBRACKET = 310,                /* RBRACKET  */
    TO = 311,                      /* TO  */
    LPAREN = 312,                  /* LPAREN  */
    RPAREN = 313,                  /* RPAREN  */
    LBRACE = 314,                  /* LBRACE  */
    RBRACE = 315,                  /* RBRACE  */
    STRING_LITERAL = 316,          /* STRING_LITERAL  */
    CHAR_LITERAL = 317,            /* CHAR_LITERAL  */
    DOUBLE_LITERAL = 318,          /* DOUBLE_LITERAL  */
    INT_LITERAL = 319,             /* INT_LITERAL  */
    UINT_LITERAL = 320,            /* UINT_LITERAL  */
    IDENTIFIER = 321,              /* IDENTIFIER  */
    INVALID_TOKEN = 322            /* INVALID_TOKEN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 40 "src/parse.y"

    char *ascii_string;
    char ascii_char;
    int signed_integer;
    unsigned int unsigned_integer;
    double floating;

    struct ASTNode *node;   /* value of almost every non-terminal          */
    const char *op_text;    /* constant operator text like "+" or "==",
                               used by bin_op / cmp_op, never freed         */

#line 150 "build/parse.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_BUILD_PARSE_H_INCLUDED  */

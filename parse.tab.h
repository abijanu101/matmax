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

#ifndef YY_YY_PARSE_TAB_H_INCLUDED
# define YY_YY_PARSE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

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
    ALL = 264,                     /* ALL  */
    FILETYPE = 265,                /* FILETYPE  */
    INT = 266,                     /* INT  */
    UINT = 267,                    /* UINT  */
    DOUBLE = 268,                  /* DOUBLE  */
    CHAR = 269,                    /* CHAR  */
    STRING = 270,                  /* STRING  */
    FOR = 271,                     /* FOR  */
    INRANGE = 272,                 /* INRANGE  */
    IF = 273,                      /* IF  */
    RET = 274,                     /* RET  */
    INIT = 275,                    /* INIT  */
    ZEROES = 276,                  /* ZEROES  */
    ENUMERATE = 277,               /* ENUMERATE  */
    RANDOM = 278,                  /* RANDOM  */
    TRAN = 279,                    /* TRAN  */
    APPLY = 280,                   /* APPLY  */
    SUM = 281,                     /* SUM  */
    MIN = 282,                     /* MIN  */
    MAX = 283,                     /* MAX  */
    HSTACK = 284,                  /* HSTACK  */
    VSTACK = 285,                  /* VSTACK  */
    WRITE = 286,                   /* WRITE  */
    PLUS = 287,                    /* PLUS  */
    MINUS = 288,                   /* MINUS  */
    MULTIPLY = 289,                /* MULTIPLY  */
    COMMA = 290,                   /* COMMA  */
    SEMICOLON = 291,               /* SEMICOLON  */
    LBRACKET = 292,                /* LBRACKET  */
    RBRACKET = 293,                /* RBRACKET  */
    LPAREN = 294,                  /* LPAREN  */
    RPAREN = 295,                  /* RPAREN  */
    LBRACE = 296,                  /* LBRACE  */
    RBRACE = 297,                  /* RBRACE  */
    STRING_LITERAL = 298,          /* STRING_LITERAL  */
    CHAR_LITERAL = 299,            /* CHAR_LITERAL  */
    DOUBLE_LITERAL = 300,          /* DOUBLE_LITERAL  */
    INT_LITERAL = 301,             /* INT_LITERAL  */
    IDENTIFIER = 302               /* IDENTIFIER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSE_TAB_H_INCLUDED  */

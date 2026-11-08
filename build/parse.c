/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 26 "src/parse.y"

#include <stdio.h>
#include <stdlib.h>
#include "ast/parser/ast.h"

extern int yylex(void);
extern int yylineno;
void yyerror(const char *s);

ParserASTNode *ast_root = NULL;   /* the finished tree, read by main.c */

#line 83 "build/parse.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parse.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_MATRIX = 3,                     /* MATRIX  */
  YYSYMBOL_TYPES = 4,                      /* TYPES  */
  YYSYMBOL_OPS = 5,                        /* OPS  */
  YYSYMBOL_SUBOPS = 6,                     /* SUBOPS  */
  YYSYMBOL_INFER = 7,                      /* INFER  */
  YYSYMBOL_DEFAULT = 8,                    /* DEFAULT  */
  YYSYMBOL_CUSTOMS = 9,                    /* CUSTOMS  */
  YYSYMBOL_ALL = 10,                       /* ALL  */
  YYSYMBOL_FILETYPE = 11,                  /* FILETYPE  */
  YYSYMBOL_INT = 12,                       /* INT  */
  YYSYMBOL_UINT = 13,                      /* UINT  */
  YYSYMBOL_DOUBLE = 14,                    /* DOUBLE  */
  YYSYMBOL_CHAR = 15,                      /* CHAR  */
  YYSYMBOL_STRING = 16,                    /* STRING  */
  YYSYMBOL_FOR = 17,                       /* FOR  */
  YYSYMBOL_INRANGE = 18,                   /* INRANGE  */
  YYSYMBOL_IF = 19,                        /* IF  */
  YYSYMBOL_RET = 20,                       /* RET  */
  YYSYMBOL_INIT = 21,                      /* INIT  */
  YYSYMBOL_ZEROES = 22,                    /* ZEROES  */
  YYSYMBOL_ENUMERATE = 23,                 /* ENUMERATE  */
  YYSYMBOL_RANDOM = 24,                    /* RANDOM  */
  YYSYMBOL_ASSIGN = 25,                    /* ASSIGN  */
  YYSYMBOL_TRAN = 26,                      /* TRAN  */
  YYSYMBOL_APPLY = 27,                     /* APPLY  */
  YYSYMBOL_SUM = 28,                       /* SUM  */
  YYSYMBOL_MIN = 29,                       /* MIN  */
  YYSYMBOL_MAX = 30,                       /* MAX  */
  YYSYMBOL_LEN = 31,                       /* LEN  */
  YYSYMBOL_ROWS = 32,                      /* ROWS  */
  YYSYMBOL_COLS = 33,                      /* COLS  */
  YYSYMBOL_HSTACK = 34,                    /* HSTACK  */
  YYSYMBOL_VSTACK = 35,                    /* VSTACK  */
  YYSYMBOL_COL = 36,                       /* COL  */
  YYSYMBOL_ROW = 37,                       /* ROW  */
  YYSYMBOL_WRITE = 38,                     /* WRITE  */
  YYSYMBOL_STDOUT = 39,                    /* STDOUT  */
  YYSYMBOL_STDIN = 40,                     /* STDIN  */
  YYSYMBOL_EQ = 41,                        /* EQ  */
  YYSYMBOL_GT = 42,                        /* GT  */
  YYSYMBOL_GTE = 43,                       /* GTE  */
  YYSYMBOL_LT = 44,                        /* LT  */
  YYSYMBOL_LTE = 45,                       /* LTE  */
  YYSYMBOL_PLUS = 46,                      /* PLUS  */
  YYSYMBOL_MINUS = 47,                     /* MINUS  */
  YYSYMBOL_MULTIPLY = 48,                  /* MULTIPLY  */
  YYSYMBOL_DIVIDE = 49,                    /* DIVIDE  */
  YYSYMBOL_MOD = 50,                       /* MOD  */
  YYSYMBOL_COMMA = 51,                     /* COMMA  */
  YYSYMBOL_COLON = 52,                     /* COLON  */
  YYSYMBOL_SEMICOLON = 53,                 /* SEMICOLON  */
  YYSYMBOL_LBRACKET = 54,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 55,                  /* RBRACKET  */
  YYSYMBOL_TO = 56,                        /* TO  */
  YYSYMBOL_LPAREN = 57,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 58,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 59,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 60,                    /* RBRACE  */
  YYSYMBOL_STRING_LITERAL = 61,            /* STRING_LITERAL  */
  YYSYMBOL_CHAR_LITERAL = 62,              /* CHAR_LITERAL  */
  YYSYMBOL_DOUBLE_LITERAL = 63,            /* DOUBLE_LITERAL  */
  YYSYMBOL_INT_LITERAL = 64,               /* INT_LITERAL  */
  YYSYMBOL_UINT_LITERAL = 65,              /* UINT_LITERAL  */
  YYSYMBOL_IDENTIFIER = 66,                /* IDENTIFIER  */
  YYSYMBOL_INVALID_TOKEN = 67,             /* INVALID_TOKEN  */
  YYSYMBOL_YYACCEPT = 68,                  /* $accept  */
  YYSYMBOL_root = 69,                      /* root  */
  YYSYMBOL_program = 70,                   /* program  */
  YYSYMBOL_item_list = 71,                 /* item_list  */
  YYSYMBOL_item = 72,                      /* item  */
  YYSYMBOL_opt_colon = 73,                 /* opt_colon  */
  YYSYMBOL_matrix_decl = 74,               /* matrix_decl  */
  YYSYMBOL_types_clause = 75,              /* types_clause  */
  YYSYMBOL_type_list = 76,                 /* type_list  */
  YYSYMBOL_type_name = 77,                 /* type_name  */
  YYSYMBOL_ops_clause = 78,                /* ops_clause  */
  YYSYMBOL_op_list = 79,                   /* op_list  */
  YYSYMBOL_op_name = 80,                   /* op_name  */
  YYSYMBOL_subops_clause = 81,             /* subops_clause  */
  YYSYMBOL_subop_list = 82,                /* subop_list  */
  YYSYMBOL_subop = 83,                     /* subop  */
  YYSYMBOL_custom_modifier_opt = 84,       /* custom_modifier_opt  */
  YYSYMBOL_type_pat = 85,                  /* type_pat  */
  YYSYMBOL_bin_op = 86,                    /* bin_op  */
  YYSYMBOL_customs_clause_opt = 87,        /* customs_clause_opt  */
  YYSYMBOL_custom_list = 88,               /* custom_list  */
  YYSYMBOL_custom = 89,                    /* custom  */
  YYSYMBOL_custom_result = 90,             /* custom_result  */
  YYSYMBOL_param_list = 91,                /* param_list  */
  YYSYMBOL_statement = 92,                 /* statement  */
  YYSYMBOL_decl = 93,                      /* decl  */
  YYSYMBOL_decl_list = 94,                 /* decl_list  */
  YYSYMBOL_decl_item = 95,                 /* decl_item  */
  YYSYMBOL_assign_stmt = 96,               /* assign_stmt  */
  YYSYMBOL_lvalue = 97,                    /* lvalue  */
  YYSYMBOL_init_stmt = 98,                 /* init_stmt  */
  YYSYMBOL_init_value = 99,                /* init_value  */
  YYSYMBOL_apply_stmt = 100,               /* apply_stmt  */
  YYSYMBOL_write_stmt = 101,               /* write_stmt  */
  YYSYMBOL_block = 102,                    /* block  */
  YYSYMBOL_stmt_list = 103,                /* stmt_list  */
  YYSYMBOL_body = 104,                     /* body  */
  YYSYMBOL_if_stmt = 105,                  /* if_stmt  */
  YYSYMBOL_for_stmt = 106,                 /* for_stmt  */
  YYSYMBOL_ret_stmt = 107,                 /* ret_stmt  */
  YYSYMBOL_expr = 108,                     /* expr  */
  YYSYMBOL_cmp_expr = 109,                 /* cmp_expr  */
  YYSYMBOL_cmp_op = 110,                   /* cmp_op  */
  YYSYMBOL_add_expr = 111,                 /* add_expr  */
  YYSYMBOL_mul_expr = 112,                 /* mul_expr  */
  YYSYMBOL_unary = 113,                    /* unary  */
  YYSYMBOL_prefix_expr = 114,              /* prefix_expr  */
  YYSYMBOL_region_opt = 115,               /* region_opt  */
  YYSYMBOL_postfix = 116,                  /* postfix  */
  YYSYMBOL_index = 117,                    /* index  */
  YYSYMBOL_primary = 118,                  /* primary  */
  YYSYMBOL_expr_list = 119,                /* expr_list  */
  YYSYMBOL_arg_list_opt = 120              /* arg_list_opt  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  67
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   379

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  68
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  53
/* YYNRULES -- Number of rules.  */
#define YYNRULES  139
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  285

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   322


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   102,   102,   106,   107,   111,   112,   116,   117,   121,
     122,   137,   147,   151,   152,   156,   157,   158,   159,   160,
     161,   162,   166,   170,   171,   175,   176,   177,   178,   179,
     180,   184,   185,   189,   190,   197,   202,   207,   217,   218,
     223,   224,   225,   229,   230,   231,   232,   236,   237,   241,
     242,   248,   257,   269,   270,   274,   276,   285,   286,   287,
     288,   289,   290,   291,   292,   299,   303,   304,   308,   310,
     315,   316,   320,   322,   327,   332,   333,   334,   335,   341,
     353,   358,   362,   363,   369,   370,   374,   378,   383,   393,
     397,   398,   402,   403,   404,   405,   406,   410,   411,   412,
     416,   417,   418,   419,   423,   424,   425,   429,   430,   431,
     432,   433,   434,   435,   436,   438,   445,   446,   447,   448,
     452,   453,   458,   459,   463,   464,   465,   466,   467,   468,
     469,   470,   471,   472,   473,   475,   488,   489,   493,   494
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "MATRIX", "TYPES",
  "OPS", "SUBOPS", "INFER", "DEFAULT", "CUSTOMS", "ALL", "FILETYPE", "INT",
  "UINT", "DOUBLE", "CHAR", "STRING", "FOR", "INRANGE", "IF", "RET",
  "INIT", "ZEROES", "ENUMERATE", "RANDOM", "ASSIGN", "TRAN", "APPLY",
  "SUM", "MIN", "MAX", "LEN", "ROWS", "COLS", "HSTACK", "VSTACK", "COL",
  "ROW", "WRITE", "STDOUT", "STDIN", "EQ", "GT", "GTE", "LT", "LTE",
  "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "MOD", "COMMA", "COLON",
  "SEMICOLON", "LBRACKET", "RBRACKET", "TO", "LPAREN", "RPAREN", "LBRACE",
  "RBRACE", "STRING_LITERAL", "CHAR_LITERAL", "DOUBLE_LITERAL",
  "INT_LITERAL", "UINT_LITERAL", "IDENTIFIER", "INVALID_TOKEN", "$accept",
  "root", "program", "item_list", "item", "opt_colon", "matrix_decl",
  "types_clause", "type_list", "type_name", "ops_clause", "op_list",
  "op_name", "subops_clause", "subop_list", "subop", "custom_modifier_opt",
  "type_pat", "bin_op", "customs_clause_opt", "custom_list", "custom",
  "custom_result", "param_list", "statement", "decl", "decl_list",
  "decl_item", "assign_stmt", "lvalue", "init_stmt", "init_value",
  "apply_stmt", "write_stmt", "block", "stmt_list", "body", "if_stmt",
  "for_stmt", "ret_stmt", "expr", "cmp_expr", "cmp_op", "add_expr",
  "mul_expr", "unary", "prefix_expr", "region_opt", "postfix", "index",
  "primary", "expr_list", "arg_list_opt", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-253)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-22)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      14,   -63,  -253,  -253,  -253,  -253,  -253,  -253,   -24,   272,
     272,     3,    16,    21,   -46,    93,  -253,    14,  -253,  -253,
      38,  -253,  -253,  -253,   -10,  -253,  -253,  -253,  -253,  -253,
    -253,    48,    94,    66,   288,   288,   288,   288,    89,   111,
     124,   130,   131,  -253,  -253,   272,   272,   272,  -253,  -253,
    -253,  -253,  -253,   133,    96,  -253,   322,   104,  -253,  -253,
     145,  -253,   138,   227,     8,   272,   272,  -253,  -253,   125,
      69,  -253,   272,   178,   200,   272,   272,   145,    11,    11,
      11,   272,   272,   272,   272,   272,  -253,  -253,     4,   148,
     272,  -253,   181,  -253,  -253,  -253,  -253,  -253,   272,   272,
     272,   272,   272,   272,   272,  -253,    66,  -253,  -253,   162,
    -253,  -253,  -253,  -253,   157,   163,   166,   169,   272,    38,
    -253,   168,   272,    96,   217,    96,   172,  -253,  -253,  -253,
     167,   176,   180,   173,   188,   272,  -253,  -253,   190,   184,
    -253,  -253,  -253,  -253,   104,   104,    97,  -253,  -253,  -253,
     193,  -253,   238,  -253,   272,   272,  -253,  -253,  -253,   195,
     192,    96,   246,   181,   272,  -253,  -253,  -253,   272,   272,
    -253,  -253,   216,   272,   211,  -253,   209,  -253,    -2,   214,
      96,   256,  -253,   212,   213,   215,  -253,  -253,   220,   221,
    -253,  -253,     6,  -253,   313,    -4,    96,   219,  -253,  -253,
    -253,  -253,   203,    -2,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,    28,  -253,  -253,   160,   218,   224,   222,  -253,   313,
    -253,  -253,    -2,  -253,   123,  -253,   137,   229,  -253,   226,
    -253,    70,  -253,  -253,  -253,  -253,  -253,  -253,   160,   160,
     -15,  -253,  -253,  -253,   230,   151,  -253,  -253,    85,   103,
     231,   244,   245,   247,  -253,   257,   103,   116,   242,    -2,
     258,   261,   263,   264,  -253,   265,  -253,   248,   266,  -253,
    -253,   248,  -253,   -35,   248,   -12,   252,   226,    91,   226,
    -253,  -253,   226,  -253,  -253
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,    20,    15,    16,    17,    18,    19,     0,     0,
       0,     0,     0,     0,    72,     0,     2,     3,     6,     7,
       0,     8,    57,    58,     0,    59,    60,    61,    62,    63,
      64,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   130,   131,     0,     0,     0,   127,   128,
     126,   124,   125,   129,     9,    89,    90,    99,   103,   105,
     106,   120,     0,     0,   119,     0,     0,     1,     5,    68,
       0,    67,     0,     0,     0,     0,     0,   107,   119,   119,
     119,     0,     0,     0,     0,     0,   104,   137,     0,     0,
     139,    10,     0,    92,    93,    94,    95,    96,     0,     0,
       0,     0,     0,     0,     0,    88,    77,    75,    76,     0,
      78,   116,   117,   118,     0,     0,   122,     0,     0,     0,
      65,     0,     0,     9,     0,     9,     0,   108,   109,   110,
       0,     0,     0,     0,     0,     0,   133,   132,   138,     0,
      83,    84,    85,    86,    97,    98,    91,   100,   101,   102,
       0,    74,     0,    80,     0,     0,    69,    66,    70,     0,
       0,     9,     0,     0,     0,   111,   112,   113,     0,     0,
     136,   135,     0,     0,     0,   123,     0,    71,     0,     0,
       9,    47,    87,     0,     0,     0,    81,    82,     0,     0,
      73,    21,     0,    14,     0,     0,     9,     0,   134,   115,
     114,   121,     0,     0,    12,    29,    25,    26,    27,    28,
      30,     0,    24,    32,     0,     0,     0,     0,    13,     0,
      22,    40,     0,    41,     0,    34,     0,     0,    11,     0,
      23,     0,    31,    33,    43,    44,    45,    46,     0,     0,
       0,    50,    79,    42,     0,     0,    48,    49,     0,     0,
       0,     0,     0,     0,    53,     0,     0,     0,    38,     0,
       0,     0,     0,     0,    39,     0,    54,     0,     0,    35,
      36,     0,    56,     0,     0,     0,     0,     0,     0,     0,
      55,    51,     0,    37,    52
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -253,  -253,  -253,  -253,   303,  -118,  -253,  -253,    61,  -154,
    -253,  -253,   102,  -253,  -253,   100,  -253,   -98,  -253,  -253,
    -253,    90,    75,  -252,   -86,  -253,  -253,   206,  -253,  -253,
    -253,  -253,  -253,  -253,  -171,  -253,   177,  -253,  -253,  -253,
      -9,  -253,  -253,   232,    81,   -13,  -253,    78,   175,  -102,
    -253,   251,  -253
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    15,    16,    17,    18,    92,    19,   124,   192,    20,
     162,   211,   212,   181,   224,   225,   265,   226,   238,   197,
     240,   241,   255,   273,    21,    22,    70,    71,    23,    24,
      25,   109,    26,    27,   142,   172,   143,    28,    29,    30,
     116,    55,   100,    56,    57,    58,    59,   114,    60,   117,
      61,    88,   139
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      54,    62,   150,    31,   213,   160,   141,   163,    66,     2,
       3,     4,     5,     6,     7,    72,   276,     1,   111,   275,
     -21,   111,   278,   277,   193,     2,     3,     4,     5,     6,
       7,     8,    86,     9,    10,    11,    73,    87,    89,   276,
     246,    12,    32,   179,   112,   113,   279,   112,   113,   218,
     214,   239,    13,   176,   110,   135,   115,   203,   242,   136,
     223,   204,   195,   121,   191,   104,   125,   126,   193,    63,
     223,   188,   130,   131,   132,   133,   134,   141,   215,   219,
      14,    87,    64,   220,   223,   223,   187,    65,   147,   148,
     149,   223,   251,    67,   252,   254,     2,     3,     4,     5,
       6,     7,   254,   263,    69,   266,   281,    74,   283,   156,
     253,   284,    75,   159,     2,     3,     4,     5,     6,     7,
     119,   203,   120,    76,   262,   243,   170,     2,     3,     4,
       5,     6,     7,   221,     2,     3,     4,     5,     6,     7,
     244,   245,   276,    98,    99,   175,    81,   250,    91,   282,
     118,   191,   101,   102,   103,   183,   127,   128,   129,   184,
     185,   221,     2,     3,     4,     5,     6,     7,    82,   191,
     221,     2,     3,     4,     5,     6,     7,   222,   232,   144,
     145,    83,   191,   234,   235,   236,   237,    84,    85,   191,
      90,   105,     2,     3,     4,     5,     6,     7,     8,   104,
       9,    10,    11,   122,   123,   222,   137,   249,    12,    77,
      78,    79,    80,   152,   222,   151,   153,   191,   154,    13,
     155,   158,   161,   164,   168,   165,   191,     2,     3,     4,
       5,     6,     7,     8,   166,     9,    10,    11,   167,   169,
     140,   135,   171,    12,   173,   174,   178,    14,   177,   106,
     107,   108,   180,    34,    13,    35,    36,    37,    38,    39,
      40,    41,    42,   189,   190,   196,    43,    44,   194,   217,
     198,   199,   227,   200,    45,   201,   186,   228,   202,   216,
     229,    46,    14,   231,    47,   140,   248,   256,    48,    49,
      50,    51,    52,    53,    33,   239,   257,   258,    34,   259,
      35,    36,    37,    38,    39,    40,    41,    42,   264,   260,
      33,    43,    44,   268,   272,   267,   269,   270,   280,    45,
      68,   230,   271,   274,   233,   157,    46,    43,    44,    47,
     247,   261,   146,    48,    49,    50,    51,    52,    53,   205,
     182,   138,    46,     0,     0,    47,     0,     0,     0,    48,
      49,    50,    51,    52,    53,     0,     0,     0,     0,   206,
     207,   208,   209,    93,    94,    95,    96,    97,    98,    99,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   210
};

static const yytype_int16 yycheck[] =
{
       9,    10,   104,    66,     8,   123,    92,   125,    54,    11,
      12,    13,    14,    15,    16,    25,    51,     3,    10,   271,
      66,    10,   274,    58,   178,    11,    12,    13,    14,    15,
      16,    17,    45,    19,    20,    21,    46,    46,    47,    51,
      55,    27,    66,   161,    36,    37,    58,    36,    37,   203,
      54,    66,    38,   155,    63,    51,    65,    51,   229,    55,
     214,    55,   180,    72,    66,    54,    75,    76,   222,    66,
     224,   173,    81,    82,    83,    84,    85,   163,   196,    51,
      66,    90,    66,    55,   238,   239,   172,    66,   101,   102,
     103,   245,     7,     0,   248,   249,    11,    12,    13,    14,
      15,    16,   256,   257,    66,   259,   277,    59,   279,   118,
       7,   282,    18,   122,    11,    12,    13,    14,    15,    16,
      51,    51,    53,    57,     8,    55,   135,    11,    12,    13,
      14,    15,    16,    10,    11,    12,    13,    14,    15,    16,
     238,   239,    51,    46,    47,   154,    57,   245,    52,    58,
      25,    66,    48,    49,    50,   164,    78,    79,    80,   168,
     169,    10,    11,    12,    13,    14,    15,    16,    57,    66,
      10,    11,    12,    13,    14,    15,    16,    54,    55,    98,
      99,    57,    66,    46,    47,    48,    49,    57,    57,    66,
      57,    53,    11,    12,    13,    14,    15,    16,    17,    54,
      19,    20,    21,    25,     4,    54,    58,    56,    27,    34,
      35,    36,    37,    56,    54,    53,    53,    66,    52,    38,
      51,    53,     5,    51,    51,    58,    66,    11,    12,    13,
      14,    15,    16,    17,    58,    19,    20,    21,    58,    51,
      59,    51,    58,    27,    51,     7,    54,    66,    53,    22,
      23,    24,     6,    26,    38,    28,    29,    30,    31,    32,
      33,    34,    35,    52,    55,     9,    39,    40,    54,    66,
      58,    58,    54,    58,    47,    55,    60,    53,    57,    60,
      58,    54,    66,   222,    57,    59,    56,    56,    61,    62,
      63,    64,    65,    66,    22,    66,    52,    52,    26,    52,
      28,    29,    30,    31,    32,    33,    34,    35,    66,    52,
      22,    39,    40,    52,    66,    57,    53,    53,    66,    47,
      17,   219,    57,    57,   224,   119,    54,    39,    40,    57,
     240,   256,   100,    61,    62,    63,    64,    65,    66,    26,
     163,    90,    54,    -1,    -1,    57,    -1,    -1,    -1,    61,
      62,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    46,
      47,    48,    49,    41,    42,    43,    44,    45,    46,    47,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    11,    12,    13,    14,    15,    16,    17,    19,
      20,    21,    27,    38,    66,    69,    70,    71,    72,    74,
      77,    92,    93,    96,    97,    98,   100,   101,   105,   106,
     107,    66,    66,    22,    26,    28,    29,    30,    31,    32,
      33,    34,    35,    39,    40,    47,    54,    57,    61,    62,
      63,    64,    65,    66,   108,   109,   111,   112,   113,   114,
     116,   118,   108,    66,    66,    66,    54,     0,    72,    66,
      94,    95,    25,    46,    59,    18,    57,   116,   116,   116,
     116,    57,    57,    57,    57,    57,   113,   108,   119,   108,
      57,    52,    73,    41,    42,    43,    44,    45,    46,    47,
     110,    48,    49,    50,    54,    53,    22,    23,    24,    99,
     108,    10,    36,    37,   115,   108,   108,   117,    25,    51,
      53,   108,    25,     4,    75,   108,   108,   115,   115,   115,
     108,   108,   108,   108,   108,    51,    55,    58,   119,   120,
      59,    92,   102,   104,   112,   112,   111,   113,   113,   113,
     117,    53,    56,    53,    52,    51,   108,    95,    53,   108,
      73,     5,    78,    73,    51,    58,    58,    58,    51,    51,
     108,    58,   103,    51,     7,   108,   117,    53,    54,    73,
       6,    81,   104,   108,   108,   108,    60,    92,   117,    52,
      55,    66,    76,    77,    54,    73,     9,    87,    58,    58,
      58,    55,    57,    51,    55,    26,    46,    47,    48,    49,
      66,    79,    80,     8,    54,    73,    60,    66,    77,    51,
      55,    10,    54,    77,    82,    83,    85,    54,    53,    58,
      80,    76,    55,    83,    46,    47,    48,    49,    86,    66,
      88,    89,   102,    55,    85,    85,    55,    89,    56,    56,
      85,     7,    77,     7,    77,    90,    56,    52,    52,    52,
      52,    90,     8,    77,    66,    84,    77,    57,    52,    53,
      53,    57,    66,    91,    57,    91,    51,    58,    91,    58,
      66,   102,    58,   102,   102
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    68,    69,    70,    70,    71,    71,    72,    72,    73,
      73,    74,    75,    76,    76,    77,    77,    77,    77,    77,
      77,    77,    78,    79,    79,    80,    80,    80,    80,    80,
      80,    81,    81,    82,    82,    83,    83,    83,    84,    84,
      85,    85,    85,    86,    86,    86,    86,    87,    87,    88,
      88,    89,    89,    90,    90,    91,    91,    92,    92,    92,
      92,    92,    92,    92,    92,    93,    94,    94,    95,    95,
      96,    96,    97,    97,    98,    99,    99,    99,    99,   100,
     101,   102,   103,   103,   104,   104,   105,   106,   107,   108,
     109,   109,   110,   110,   110,   110,   110,   111,   111,   111,
     112,   112,   112,   112,   113,   113,   113,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   115,   115,   115,   115,
     116,   116,   117,   117,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   119,   119,   120,   120
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     0,     2,     1,     1,     1,     0,
       1,     9,     5,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     5,     3,     1,     1,     1,     1,     1,     1,
       1,     5,     3,     2,     1,     8,     8,    11,     0,     1,
       1,     1,     3,     1,     1,     1,     1,     0,     5,     2,
       1,     9,    10,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     3,
       4,     5,     1,     6,     4,     1,     1,     1,     1,    10,
       4,     3,     2,     0,     1,     1,     4,     6,     3,     1,
       1,     3,     1,     1,     1,     1,     1,     3,     3,     1,
       3,     3,     3,     1,     2,     1,     1,     2,     3,     3,
       3,     4,     4,     4,     6,     6,     1,     1,     1,     0,
       1,     6,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     6,     4,     3,     1,     1,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_STRING_LITERAL: /* STRING_LITERAL  */
#line 91 "src/parse.y"
            { free(((*yyvaluep).ascii_string)); }
#line 1415 "build/parse.c"
        break;

    case YYSYMBOL_IDENTIFIER: /* IDENTIFIER  */
#line 91 "src/parse.y"
            { free(((*yyvaluep).ascii_string)); }
#line 1421 "build/parse.c"
        break;

    case YYSYMBOL_program: /* program  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1427 "build/parse.c"
        break;

    case YYSYMBOL_item_list: /* item_list  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1433 "build/parse.c"
        break;

    case YYSYMBOL_item: /* item  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1439 "build/parse.c"
        break;

    case YYSYMBOL_matrix_decl: /* matrix_decl  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1445 "build/parse.c"
        break;

    case YYSYMBOL_types_clause: /* types_clause  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1451 "build/parse.c"
        break;

    case YYSYMBOL_type_list: /* type_list  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1457 "build/parse.c"
        break;

    case YYSYMBOL_type_name: /* type_name  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1463 "build/parse.c"
        break;

    case YYSYMBOL_ops_clause: /* ops_clause  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1469 "build/parse.c"
        break;

    case YYSYMBOL_op_list: /* op_list  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1475 "build/parse.c"
        break;

    case YYSYMBOL_op_name: /* op_name  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1481 "build/parse.c"
        break;

    case YYSYMBOL_subops_clause: /* subops_clause  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1487 "build/parse.c"
        break;

    case YYSYMBOL_subop_list: /* subop_list  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1493 "build/parse.c"
        break;

    case YYSYMBOL_subop: /* subop  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1499 "build/parse.c"
        break;

    case YYSYMBOL_custom_modifier_opt: /* custom_modifier_opt  */
#line 91 "src/parse.y"
            { free(((*yyvaluep).ascii_string)); }
#line 1505 "build/parse.c"
        break;

    case YYSYMBOL_type_pat: /* type_pat  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1511 "build/parse.c"
        break;

    case YYSYMBOL_customs_clause_opt: /* customs_clause_opt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1517 "build/parse.c"
        break;

    case YYSYMBOL_custom_list: /* custom_list  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1523 "build/parse.c"
        break;

    case YYSYMBOL_custom: /* custom  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1529 "build/parse.c"
        break;

    case YYSYMBOL_custom_result: /* custom_result  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1535 "build/parse.c"
        break;

    case YYSYMBOL_param_list: /* param_list  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1541 "build/parse.c"
        break;

    case YYSYMBOL_statement: /* statement  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1547 "build/parse.c"
        break;

    case YYSYMBOL_decl: /* decl  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1553 "build/parse.c"
        break;

    case YYSYMBOL_decl_list: /* decl_list  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1559 "build/parse.c"
        break;

    case YYSYMBOL_decl_item: /* decl_item  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1565 "build/parse.c"
        break;

    case YYSYMBOL_assign_stmt: /* assign_stmt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1571 "build/parse.c"
        break;

    case YYSYMBOL_lvalue: /* lvalue  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1577 "build/parse.c"
        break;

    case YYSYMBOL_init_stmt: /* init_stmt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1583 "build/parse.c"
        break;

    case YYSYMBOL_init_value: /* init_value  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1589 "build/parse.c"
        break;

    case YYSYMBOL_apply_stmt: /* apply_stmt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1595 "build/parse.c"
        break;

    case YYSYMBOL_write_stmt: /* write_stmt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1601 "build/parse.c"
        break;

    case YYSYMBOL_block: /* block  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1607 "build/parse.c"
        break;

    case YYSYMBOL_stmt_list: /* stmt_list  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1613 "build/parse.c"
        break;

    case YYSYMBOL_body: /* body  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1619 "build/parse.c"
        break;

    case YYSYMBOL_if_stmt: /* if_stmt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1625 "build/parse.c"
        break;

    case YYSYMBOL_for_stmt: /* for_stmt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1631 "build/parse.c"
        break;

    case YYSYMBOL_ret_stmt: /* ret_stmt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1637 "build/parse.c"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1643 "build/parse.c"
        break;

    case YYSYMBOL_cmp_expr: /* cmp_expr  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1649 "build/parse.c"
        break;

    case YYSYMBOL_add_expr: /* add_expr  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1655 "build/parse.c"
        break;

    case YYSYMBOL_mul_expr: /* mul_expr  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1661 "build/parse.c"
        break;

    case YYSYMBOL_unary: /* unary  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1667 "build/parse.c"
        break;

    case YYSYMBOL_prefix_expr: /* prefix_expr  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1673 "build/parse.c"
        break;

    case YYSYMBOL_region_opt: /* region_opt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1679 "build/parse.c"
        break;

    case YYSYMBOL_postfix: /* postfix  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1685 "build/parse.c"
        break;

    case YYSYMBOL_index: /* index  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1691 "build/parse.c"
        break;

    case YYSYMBOL_primary: /* primary  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1697 "build/parse.c"
        break;

    case YYSYMBOL_expr_list: /* expr_list  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1703 "build/parse.c"
        break;

    case YYSYMBOL_arg_list_opt: /* arg_list_opt  */
#line 90 "src/parse.y"
            { ast_free(((*yyvaluep).node)); }
#line 1709 "build/parse.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* root: program  */
#line 102 "src/parse.y"
                                    { ast_root = (yyvsp[0].node); }
#line 1982 "build/parse.c"
    break;

  case 3: /* program: item_list  */
#line 106 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 1988 "build/parse.c"
    break;

  case 4: /* program: %empty  */
#line 107 "src/parse.y"
                                    { (yyval.node) = ast_new(N_PROGRAM, NULL); }
#line 1994 "build/parse.c"
    break;

  case 5: /* item_list: item_list item  */
#line 111 "src/parse.y"
                                    { (yyval.node) = ast_add((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2000 "build/parse.c"
    break;

  case 6: /* item_list: item  */
#line 112 "src/parse.y"
                                    { (yyval.node) = ast_make(N_PROGRAM, NULL, 1, (yyvsp[0].node)); }
#line 2006 "build/parse.c"
    break;

  case 7: /* item: matrix_decl  */
#line 116 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2012 "build/parse.c"
    break;

  case 8: /* item: statement  */
#line 117 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2018 "build/parse.c"
    break;

  case 11: /* matrix_decl: MATRIX IDENTIFIER LBRACE types_clause ops_clause subops_clause customs_clause_opt RBRACE SEMICOLON  */
#line 140 "src/parse.y"
        {
            (yyval.node) = ast_make(N_MATRIX_DEF, (yyvsp[-7].ascii_string), 4, (yyvsp[-5].node), (yyvsp[-4].node), (yyvsp[-3].node), (yyvsp[-2].node));
            free((yyvsp[-7].ascii_string));
        }
#line 2027 "build/parse.c"
    break;

  case 12: /* types_clause: TYPES opt_colon LBRACKET type_list RBRACKET  */
#line 147 "src/parse.y"
                                                    { (yyval.node) = (yyvsp[-1].node); }
#line 2033 "build/parse.c"
    break;

  case 13: /* type_list: type_list COMMA type_name  */
#line 151 "src/parse.y"
                                    { (yyval.node) = ast_add((yyvsp[-2].node), (yyvsp[0].node)); }
#line 2039 "build/parse.c"
    break;

  case 14: /* type_list: type_name  */
#line 152 "src/parse.y"
                                    { (yyval.node) = ast_make(N_TYPE_LIST, NULL, 1, (yyvsp[0].node)); }
#line 2045 "build/parse.c"
    break;

  case 15: /* type_name: INT  */
#line 156 "src/parse.y"
                                    { (yyval.node) = ast_new(N_TYPE, "int"); }
#line 2051 "build/parse.c"
    break;

  case 16: /* type_name: UINT  */
#line 157 "src/parse.y"
                                    { (yyval.node) = ast_new(N_TYPE, "uint"); }
#line 2057 "build/parse.c"
    break;

  case 17: /* type_name: DOUBLE  */
#line 158 "src/parse.y"
                                    { (yyval.node) = ast_new(N_TYPE, "double"); }
#line 2063 "build/parse.c"
    break;

  case 18: /* type_name: CHAR  */
#line 159 "src/parse.y"
                                    { (yyval.node) = ast_new(N_TYPE, "char"); }
#line 2069 "build/parse.c"
    break;

  case 19: /* type_name: STRING  */
#line 160 "src/parse.y"
                                    { (yyval.node) = ast_new(N_TYPE, "string"); }
#line 2075 "build/parse.c"
    break;

  case 20: /* type_name: FILETYPE  */
#line 161 "src/parse.y"
                                    { (yyval.node) = ast_new(N_TYPE, "file"); }
#line 2081 "build/parse.c"
    break;

  case 21: /* type_name: IDENTIFIER  */
#line 162 "src/parse.y"
                                    { (yyval.node) = ast_new(N_TYPE, (yyvsp[0].ascii_string)); free((yyvsp[0].ascii_string)); }
#line 2087 "build/parse.c"
    break;

  case 22: /* ops_clause: OPS opt_colon LBRACKET op_list RBRACKET  */
#line 166 "src/parse.y"
                                                    { (yyval.node) = (yyvsp[-1].node); }
#line 2093 "build/parse.c"
    break;

  case 23: /* op_list: op_list COMMA op_name  */
#line 170 "src/parse.y"
                                    { (yyval.node) = ast_add((yyvsp[-2].node), (yyvsp[0].node)); }
#line 2099 "build/parse.c"
    break;

  case 24: /* op_list: op_name  */
#line 171 "src/parse.y"
                                    { (yyval.node) = ast_make(N_OP_LIST, NULL, 1, (yyvsp[0].node)); }
#line 2105 "build/parse.c"
    break;

  case 25: /* op_name: PLUS  */
#line 175 "src/parse.y"
                                    { (yyval.node) = ast_new(N_OP, "+"); }
#line 2111 "build/parse.c"
    break;

  case 26: /* op_name: MINUS  */
#line 176 "src/parse.y"
                                    { (yyval.node) = ast_new(N_OP, "-"); }
#line 2117 "build/parse.c"
    break;

  case 27: /* op_name: MULTIPLY  */
#line 177 "src/parse.y"
                                    { (yyval.node) = ast_new(N_OP, "*"); }
#line 2123 "build/parse.c"
    break;

  case 28: /* op_name: DIVIDE  */
#line 178 "src/parse.y"
                                    { (yyval.node) = ast_new(N_OP, "/"); }
#line 2129 "build/parse.c"
    break;

  case 29: /* op_name: TRAN  */
#line 179 "src/parse.y"
                                    { (yyval.node) = ast_new(N_OP, "tran"); }
#line 2135 "build/parse.c"
    break;

  case 30: /* op_name: IDENTIFIER  */
#line 180 "src/parse.y"
                                    { (yyval.node) = ast_new(N_OP, (yyvsp[0].ascii_string)); free((yyvsp[0].ascii_string)); }
#line 2141 "build/parse.c"
    break;

  case 31: /* subops_clause: SUBOPS opt_colon LBRACKET subop_list RBRACKET  */
#line 184 "src/parse.y"
                                                    { (yyval.node) = (yyvsp[-1].node); }
#line 2147 "build/parse.c"
    break;

  case 32: /* subops_clause: SUBOPS opt_colon DEFAULT  */
#line 185 "src/parse.y"
                                    { (yyval.node) = ast_new(N_SUBOP_LIST, "default"); }
#line 2153 "build/parse.c"
    break;

  case 33: /* subop_list: subop_list subop  */
#line 189 "src/parse.y"
                                    { (yyval.node) = ast_add((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2159 "build/parse.c"
    break;

  case 34: /* subop_list: subop  */
#line 190 "src/parse.y"
                                    { (yyval.node) = ast_make(N_SUBOP_LIST, NULL, 1, (yyvsp[0].node)); }
#line 2165 "build/parse.c"
    break;

  case 35: /* subop: type_pat bin_op type_pat TO INFER COLON DEFAULT SEMICOLON  */
#line 198 "src/parse.y"
        {
            (yyval.node) = ast_make(N_SUBOP, (yyvsp[-6].op_text), 3, (yyvsp[-7].node), (yyvsp[-5].node),
                          ast_new(N_INFER_DEFAULT, NULL));
        }
#line 2174 "build/parse.c"
    break;

  case 36: /* subop: type_pat bin_op type_pat TO INFER COLON type_name SEMICOLON  */
#line 203 "src/parse.y"
        {
            (yyval.node) = ast_make(N_SUBOP, (yyvsp[-6].op_text), 3, (yyvsp[-7].node), (yyvsp[-5].node),
                          ast_make(N_INFER, NULL, 1, (yyvsp[-1].node)));
        }
#line 2183 "build/parse.c"
    break;

  case 37: /* subop: type_pat bin_op type_pat TO type_name COLON custom_modifier_opt LPAREN param_list RPAREN block  */
#line 209 "src/parse.y"
        {
            (yyval.node) = ast_make(N_SUBOP, (yyvsp[-9].op_text), 4, (yyvsp[-10].node), (yyvsp[-8].node), (yyvsp[-6].node),
                          ast_make(N_FUNC, (yyvsp[-4].ascii_string), 2, (yyvsp[-2].node), (yyvsp[0].node)));
            free((yyvsp[-4].ascii_string));   /* free(NULL) is fine when there is no modifier */
        }
#line 2193 "build/parse.c"
    break;

  case 38: /* custom_modifier_opt: %empty  */
#line 217 "src/parse.y"
                                    { (yyval.ascii_string) = NULL; }
#line 2199 "build/parse.c"
    break;

  case 39: /* custom_modifier_opt: IDENTIFIER  */
#line 218 "src/parse.y"
                                    { (yyval.ascii_string) = (yyvsp[0].ascii_string); }
#line 2205 "build/parse.c"
    break;

  case 40: /* type_pat: ALL  */
#line 223 "src/parse.y"
                                    { (yyval.node) = ast_new(N_ALL, NULL); }
#line 2211 "build/parse.c"
    break;

  case 41: /* type_pat: type_name  */
#line 224 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2217 "build/parse.c"
    break;

  case 42: /* type_pat: LBRACKET type_list RBRACKET  */
#line 225 "src/parse.y"
                                    { (yyval.node) = (yyvsp[-1].node); }
#line 2223 "build/parse.c"
    break;

  case 43: /* bin_op: PLUS  */
#line 229 "src/parse.y"
                                    { (yyval.op_text) = "+"; }
#line 2229 "build/parse.c"
    break;

  case 44: /* bin_op: MINUS  */
#line 230 "src/parse.y"
                                    { (yyval.op_text) = "-"; }
#line 2235 "build/parse.c"
    break;

  case 45: /* bin_op: MULTIPLY  */
#line 231 "src/parse.y"
                                    { (yyval.op_text) = "*"; }
#line 2241 "build/parse.c"
    break;

  case 46: /* bin_op: DIVIDE  */
#line 232 "src/parse.y"
                                    { (yyval.op_text) = "/"; }
#line 2247 "build/parse.c"
    break;

  case 47: /* customs_clause_opt: %empty  */
#line 236 "src/parse.y"
                                    { (yyval.node) = ast_new(N_CUSTOM_LIST, NULL); }
#line 2253 "build/parse.c"
    break;

  case 48: /* customs_clause_opt: CUSTOMS opt_colon LBRACKET custom_list RBRACKET  */
#line 237 "src/parse.y"
                                                      { (yyval.node) = (yyvsp[-1].node); }
#line 2259 "build/parse.c"
    break;

  case 49: /* custom_list: custom_list custom  */
#line 241 "src/parse.y"
                                    { (yyval.node) = ast_add((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2265 "build/parse.c"
    break;

  case 50: /* custom_list: custom  */
#line 242 "src/parse.y"
                                    { (yyval.node) = ast_make(N_CUSTOM_LIST, NULL, 1, (yyvsp[0].node)); }
#line 2271 "build/parse.c"
    break;

  case 51: /* custom: IDENTIFIER type_pat TO custom_result COLON LPAREN param_list RPAREN block  */
#line 250 "src/parse.y"
        {
            (yyval.node) = ast_make(N_CUSTOM, (yyvsp[-8].ascii_string), 3,
                          ast_make(N_ARG_TYPES, NULL, 1, (yyvsp[-7].node)),
                          (yyvsp[-5].node),
                          ast_make(N_FUNC, NULL, 2, (yyvsp[-2].node), (yyvsp[0].node)));
            free((yyvsp[-8].ascii_string));
        }
#line 2283 "build/parse.c"
    break;

  case 52: /* custom: IDENTIFIER type_pat type_pat TO custom_result COLON LPAREN param_list RPAREN block  */
#line 259 "src/parse.y"
        {
            (yyval.node) = ast_make(N_CUSTOM, (yyvsp[-9].ascii_string), 3,
                          ast_make(N_ARG_TYPES, NULL, 2, (yyvsp[-8].node), (yyvsp[-7].node)),
                          (yyvsp[-5].node),
                          ast_make(N_FUNC, NULL, 2, (yyvsp[-2].node), (yyvsp[0].node)));
            free((yyvsp[-9].ascii_string));
        }
#line 2295 "build/parse.c"
    break;

  case 53: /* custom_result: type_name  */
#line 269 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2301 "build/parse.c"
    break;

  case 54: /* custom_result: INFER COLON type_name  */
#line 270 "src/parse.y"
                                    { (yyval.node) = ast_make(N_INFER, NULL, 1, (yyvsp[0].node)); }
#line 2307 "build/parse.c"
    break;

  case 55: /* param_list: param_list COMMA IDENTIFIER  */
#line 275 "src/parse.y"
        { (yyval.node) = ast_add((yyvsp[-2].node), ast_new(N_ID, (yyvsp[0].ascii_string))); free((yyvsp[0].ascii_string)); }
#line 2313 "build/parse.c"
    break;

  case 56: /* param_list: IDENTIFIER  */
#line 277 "src/parse.y"
        { (yyval.node) = ast_make(N_PARAMS, NULL, 1, ast_new(N_ID, (yyvsp[0].ascii_string))); free((yyvsp[0].ascii_string)); }
#line 2319 "build/parse.c"
    break;

  case 57: /* statement: decl  */
#line 285 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2325 "build/parse.c"
    break;

  case 58: /* statement: assign_stmt  */
#line 286 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2331 "build/parse.c"
    break;

  case 59: /* statement: init_stmt  */
#line 287 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2337 "build/parse.c"
    break;

  case 60: /* statement: apply_stmt  */
#line 288 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2343 "build/parse.c"
    break;

  case 61: /* statement: write_stmt  */
#line 289 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2349 "build/parse.c"
    break;

  case 62: /* statement: if_stmt  */
#line 290 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2355 "build/parse.c"
    break;

  case 63: /* statement: for_stmt  */
#line 291 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2361 "build/parse.c"
    break;

  case 64: /* statement: ret_stmt  */
#line 292 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2367 "build/parse.c"
    break;

  case 65: /* decl: type_name decl_list SEMICOLON  */
#line 299 "src/parse.y"
                                    { (yyval.node) = ast_prepend((yyvsp[-1].node), (yyvsp[-2].node)); }
#line 2373 "build/parse.c"
    break;

  case 66: /* decl_list: decl_list COMMA decl_item  */
#line 303 "src/parse.y"
                                    { (yyval.node) = ast_add((yyvsp[-2].node), (yyvsp[0].node)); }
#line 2379 "build/parse.c"
    break;

  case 67: /* decl_list: decl_item  */
#line 304 "src/parse.y"
                                    { (yyval.node) = ast_make(N_DECL, NULL, 1, (yyvsp[0].node)); }
#line 2385 "build/parse.c"
    break;

  case 68: /* decl_item: IDENTIFIER  */
#line 309 "src/parse.y"
        { (yyval.node) = ast_new(N_DECL_VAR, (yyvsp[0].ascii_string)); free((yyvsp[0].ascii_string)); }
#line 2391 "build/parse.c"
    break;

  case 69: /* decl_item: IDENTIFIER ASSIGN expr  */
#line 311 "src/parse.y"
        { (yyval.node) = ast_make(N_DECL_VAR, (yyvsp[-2].ascii_string), 1, (yyvsp[0].node)); free((yyvsp[-2].ascii_string)); }
#line 2397 "build/parse.c"
    break;

  case 70: /* assign_stmt: lvalue ASSIGN expr SEMICOLON  */
#line 315 "src/parse.y"
                                        { (yyval.node) = ast_make(N_ASSIGN, "=",  2, (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2403 "build/parse.c"
    break;

  case 71: /* assign_stmt: lvalue PLUS ASSIGN expr SEMICOLON  */
#line 316 "src/parse.y"
                                        { (yyval.node) = ast_make(N_ASSIGN, "+=", 2, (yyvsp[-4].node), (yyvsp[-1].node)); }
#line 2409 "build/parse.c"
    break;

  case 72: /* lvalue: IDENTIFIER  */
#line 321 "src/parse.y"
        { (yyval.node) = ast_new(N_ID, (yyvsp[0].ascii_string)); free((yyvsp[0].ascii_string)); }
#line 2415 "build/parse.c"
    break;

  case 73: /* lvalue: IDENTIFIER LBRACKET index COMMA index RBRACKET  */
#line 323 "src/parse.y"
        { (yyval.node) = ast_make(N_INDEX, NULL, 3, ast_new(N_ID, (yyvsp[-5].ascii_string)), (yyvsp[-3].node), (yyvsp[-1].node)); free((yyvsp[-5].ascii_string)); }
#line 2421 "build/parse.c"
    break;

  case 74: /* init_stmt: INIT IDENTIFIER init_value SEMICOLON  */
#line 328 "src/parse.y"
        { (yyval.node) = ast_make(N_INIT, (yyvsp[-2].ascii_string), 1, (yyvsp[-1].node)); free((yyvsp[-2].ascii_string)); }
#line 2427 "build/parse.c"
    break;

  case 75: /* init_value: ENUMERATE  */
#line 332 "src/parse.y"
                                    { (yyval.node) = ast_new(N_INIT_MODE, "enumerate"); }
#line 2433 "build/parse.c"
    break;

  case 76: /* init_value: RANDOM  */
#line 333 "src/parse.y"
                                    { (yyval.node) = ast_new(N_INIT_MODE, "random"); }
#line 2439 "build/parse.c"
    break;

  case 77: /* init_value: ZEROES  */
#line 334 "src/parse.y"
                                    { (yyval.node) = ast_new(N_INIT_MODE, "zeros"); }
#line 2445 "build/parse.c"
    break;

  case 78: /* init_value: expr  */
#line 335 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2451 "build/parse.c"
    break;

  case 79: /* apply_stmt: APPLY IDENTIFIER region_opt TO INFER COLON LPAREN IDENTIFIER RPAREN block  */
#line 343 "src/parse.y"
        {
            ParserASTNode *params = ast_make(N_PARAMS, NULL, 1, ast_new(N_ID, (yyvsp[-2].ascii_string)));
            (yyval.node) = ast_make(N_APPLY, (yyvsp[-8].ascii_string), 2, (yyvsp[-7].node),
                          ast_make(N_FUNC, NULL, 2, params, (yyvsp[0].node)));
            free((yyvsp[-8].ascii_string));
            free((yyvsp[-2].ascii_string));
        }
#line 2463 "build/parse.c"
    break;

  case 80: /* write_stmt: WRITE IDENTIFIER expr SEMICOLON  */
#line 354 "src/parse.y"
        { (yyval.node) = ast_make(N_WRITE, (yyvsp[-2].ascii_string), 1, (yyvsp[-1].node)); free((yyvsp[-2].ascii_string)); }
#line 2469 "build/parse.c"
    break;

  case 81: /* block: LBRACE stmt_list RBRACE  */
#line 358 "src/parse.y"
                                    { (yyval.node) = (yyvsp[-1].node); }
#line 2475 "build/parse.c"
    break;

  case 82: /* stmt_list: stmt_list statement  */
#line 362 "src/parse.y"
                                    { (yyval.node) = ast_add((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2481 "build/parse.c"
    break;

  case 83: /* stmt_list: %empty  */
#line 363 "src/parse.y"
                                    { (yyval.node) = ast_new(N_BLOCK, NULL); }
#line 2487 "build/parse.c"
    break;

  case 84: /* body: statement  */
#line 369 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BLOCK, NULL, 1, (yyvsp[0].node)); }
#line 2493 "build/parse.c"
    break;

  case 85: /* body: block  */
#line 370 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2499 "build/parse.c"
    break;

  case 86: /* if_stmt: IF expr opt_colon body  */
#line 374 "src/parse.y"
                                    { (yyval.node) = ast_make(N_IF, NULL, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2505 "build/parse.c"
    break;

  case 87: /* for_stmt: FOR IDENTIFIER INRANGE expr opt_colon body  */
#line 379 "src/parse.y"
        { (yyval.node) = ast_make(N_FOR, (yyvsp[-4].ascii_string), 2, (yyvsp[-2].node), (yyvsp[0].node)); free((yyvsp[-4].ascii_string)); }
#line 2511 "build/parse.c"
    break;

  case 88: /* ret_stmt: RET expr SEMICOLON  */
#line 383 "src/parse.y"
                                    { (yyval.node) = ast_make(N_RETURN, NULL, 1, (yyvsp[-1].node)); }
#line 2517 "build/parse.c"
    break;

  case 89: /* expr: cmp_expr  */
#line 393 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2523 "build/parse.c"
    break;

  case 90: /* cmp_expr: add_expr  */
#line 397 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2529 "build/parse.c"
    break;

  case 91: /* cmp_expr: add_expr cmp_op add_expr  */
#line 398 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BINOP, (yyvsp[-1].op_text), 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2535 "build/parse.c"
    break;

  case 92: /* cmp_op: EQ  */
#line 402 "src/parse.y"
                                    { (yyval.op_text) = "=="; }
#line 2541 "build/parse.c"
    break;

  case 93: /* cmp_op: GT  */
#line 403 "src/parse.y"
                                    { (yyval.op_text) = ">"; }
#line 2547 "build/parse.c"
    break;

  case 94: /* cmp_op: GTE  */
#line 404 "src/parse.y"
                                    { (yyval.op_text) = ">="; }
#line 2553 "build/parse.c"
    break;

  case 95: /* cmp_op: LT  */
#line 405 "src/parse.y"
                                    { (yyval.op_text) = "<"; }
#line 2559 "build/parse.c"
    break;

  case 96: /* cmp_op: LTE  */
#line 406 "src/parse.y"
                                    { (yyval.op_text) = "<="; }
#line 2565 "build/parse.c"
    break;

  case 97: /* add_expr: add_expr PLUS mul_expr  */
#line 410 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BINOP, "+", 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2571 "build/parse.c"
    break;

  case 98: /* add_expr: add_expr MINUS mul_expr  */
#line 411 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BINOP, "-", 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2577 "build/parse.c"
    break;

  case 99: /* add_expr: mul_expr  */
#line 412 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2583 "build/parse.c"
    break;

  case 100: /* mul_expr: mul_expr MULTIPLY unary  */
#line 416 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BINOP, "*", 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2589 "build/parse.c"
    break;

  case 101: /* mul_expr: mul_expr DIVIDE unary  */
#line 417 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BINOP, "/", 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2595 "build/parse.c"
    break;

  case 102: /* mul_expr: mul_expr MOD unary  */
#line 418 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BINOP, "%", 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2601 "build/parse.c"
    break;

  case 103: /* mul_expr: unary  */
#line 419 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2607 "build/parse.c"
    break;

  case 104: /* unary: MINUS unary  */
#line 423 "src/parse.y"
                                    { (yyval.node) = ast_make(N_NEG, NULL, 1, (yyvsp[0].node)); }
#line 2613 "build/parse.c"
    break;

  case 105: /* unary: prefix_expr  */
#line 424 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2619 "build/parse.c"
    break;

  case 106: /* unary: postfix  */
#line 425 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2625 "build/parse.c"
    break;

  case 107: /* prefix_expr: TRAN postfix  */
#line 429 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "tran", 1, (yyvsp[0].node)); }
#line 2631 "build/parse.c"
    break;

  case 108: /* prefix_expr: SUM postfix region_opt  */
#line 430 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "sum",  2, (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2637 "build/parse.c"
    break;

  case 109: /* prefix_expr: MIN postfix region_opt  */
#line 431 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "min",  2, (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2643 "build/parse.c"
    break;

  case 110: /* prefix_expr: MAX postfix region_opt  */
#line 432 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "max",  2, (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2649 "build/parse.c"
    break;

  case 111: /* prefix_expr: LEN LPAREN expr RPAREN  */
#line 433 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "len",  1, (yyvsp[-1].node)); }
#line 2655 "build/parse.c"
    break;

  case 112: /* prefix_expr: ROWS LPAREN expr RPAREN  */
#line 434 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "rows", 1, (yyvsp[-1].node)); }
#line 2661 "build/parse.c"
    break;

  case 113: /* prefix_expr: COLS LPAREN expr RPAREN  */
#line 435 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "cols", 1, (yyvsp[-1].node)); }
#line 2667 "build/parse.c"
    break;

  case 114: /* prefix_expr: VSTACK LPAREN expr COMMA expr RPAREN  */
#line 437 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "vstack", 2, (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2673 "build/parse.c"
    break;

  case 115: /* prefix_expr: HSTACK LPAREN expr COMMA expr RPAREN  */
#line 439 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "hstack", 2, (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2679 "build/parse.c"
    break;

  case 116: /* region_opt: ALL  */
#line 445 "src/parse.y"
                                    { (yyval.node) = ast_new(N_REGION, "all"); }
#line 2685 "build/parse.c"
    break;

  case 117: /* region_opt: COL  */
#line 446 "src/parse.y"
                                    { (yyval.node) = ast_new(N_REGION, "col"); }
#line 2691 "build/parse.c"
    break;

  case 118: /* region_opt: ROW  */
#line 447 "src/parse.y"
                                    { (yyval.node) = ast_new(N_REGION, "row"); }
#line 2697 "build/parse.c"
    break;

  case 119: /* region_opt: %empty  */
#line 448 "src/parse.y"
                                    { (yyval.node) = ast_new(N_REGION, "all"); }
#line 2703 "build/parse.c"
    break;

  case 120: /* postfix: primary  */
#line 452 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2709 "build/parse.c"
    break;

  case 121: /* postfix: postfix LBRACKET index COMMA index RBRACKET  */
#line 454 "src/parse.y"
                                    { (yyval.node) = ast_make(N_INDEX, NULL, 3, (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2715 "build/parse.c"
    break;

  case 122: /* index: expr  */
#line 458 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2721 "build/parse.c"
    break;

  case 123: /* index: expr COLON expr  */
#line 459 "src/parse.y"
                                    { (yyval.node) = ast_make(N_RANGE, NULL, 2, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2727 "build/parse.c"
    break;

  case 124: /* primary: INT_LITERAL  */
#line 463 "src/parse.y"
                                    { (yyval.node) = ast_int((yyvsp[0].signed_integer)); }
#line 2733 "build/parse.c"
    break;

  case 125: /* primary: UINT_LITERAL  */
#line 464 "src/parse.y"
                                    { (yyval.node) = ast_uint((yyvsp[0].unsigned_integer)); }
#line 2739 "build/parse.c"
    break;

  case 126: /* primary: DOUBLE_LITERAL  */
#line 465 "src/parse.y"
                                    { (yyval.node) = ast_double((yyvsp[0].floating)); }
#line 2745 "build/parse.c"
    break;

  case 127: /* primary: STRING_LITERAL  */
#line 466 "src/parse.y"
                                    { (yyval.node) = ast_new(N_STRING, (yyvsp[0].ascii_string)); free((yyvsp[0].ascii_string)); }
#line 2751 "build/parse.c"
    break;

  case 128: /* primary: CHAR_LITERAL  */
#line 467 "src/parse.y"
                                    { (yyval.node) = ast_char((yyvsp[0].ascii_char)); }
#line 2757 "build/parse.c"
    break;

  case 129: /* primary: IDENTIFIER  */
#line 468 "src/parse.y"
                                    { (yyval.node) = ast_new(N_ID, (yyvsp[0].ascii_string)); free((yyvsp[0].ascii_string)); }
#line 2763 "build/parse.c"
    break;

  case 130: /* primary: STDOUT  */
#line 469 "src/parse.y"
                                    { (yyval.node) = ast_new(N_STREAM, "stdout"); }
#line 2769 "build/parse.c"
    break;

  case 131: /* primary: STDIN  */
#line 470 "src/parse.y"
                                    { (yyval.node) = ast_new(N_STREAM, "stdin"); }
#line 2775 "build/parse.c"
    break;

  case 132: /* primary: LPAREN expr RPAREN  */
#line 471 "src/parse.y"
                                    { (yyval.node) = (yyvsp[-1].node); }
#line 2781 "build/parse.c"
    break;

  case 133: /* primary: LBRACKET expr_list RBRACKET  */
#line 472 "src/parse.y"
                                    { (yyval.node) = (yyvsp[-1].node); }
#line 2787 "build/parse.c"
    break;

  case 134: /* primary: ZEROES LPAREN expr COMMA expr RPAREN  */
#line 474 "src/parse.y"
                                    { (yyval.node) = ast_make(N_BUILTIN, "zeros", 2, (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2793 "build/parse.c"
    break;

  case 135: /* primary: IDENTIFIER LPAREN arg_list_opt RPAREN  */
#line 476 "src/parse.y"
        {
            /* reuse the list node that holds the arguments,
               just relabel it as a CALL and give it the name */
            (yyval.node) = (yyvsp[-1].node);
            (yyval.node)->kind = N_CALL;
            (yyval.node)->text = (yyvsp[-3].ascii_string);      /* take the lexer's string, no copy needed */
        }
#line 2805 "build/parse.c"
    break;

  case 136: /* expr_list: expr_list COMMA expr  */
#line 488 "src/parse.y"
                                    { (yyval.node) = ast_add((yyvsp[-2].node), (yyvsp[0].node)); }
#line 2811 "build/parse.c"
    break;

  case 137: /* expr_list: expr  */
#line 489 "src/parse.y"
                                    { (yyval.node) = ast_make(N_MATRIX_LIT, NULL, 1, (yyvsp[0].node)); }
#line 2817 "build/parse.c"
    break;

  case 138: /* arg_list_opt: expr_list  */
#line 493 "src/parse.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 2823 "build/parse.c"
    break;

  case 139: /* arg_list_opt: %empty  */
#line 494 "src/parse.y"
                                    { (yyval.node) = ast_new(N_MATRIX_LIT, NULL); }
#line 2829 "build/parse.c"
    break;


#line 2833 "build/parse.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 497 "src/parse.y"


void yyerror(const char *s) {
    fprintf(stderr, "Line %d: syntax error: %s\n", yylineno, s);
}

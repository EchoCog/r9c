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

#ifndef YY_YY_R_TAB_H_INCLUDED
# define YY_YY_R_TAB_H_INCLUDED
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
    IDENTIFIER = 258,              /* IDENTIFIER  */
    STRING = 259,                  /* STRING  */
    FILEPATH = 260,                /* FILEPATH  */
    INTEGER = 261,                 /* INTEGER  */
    REAL = 262,                    /* REAL  */
    ORCHESTRATOR = 263,            /* ORCHESTRATOR  */
    MODEL = 264,                   /* MODEL  */
    AIRCHAT = 265,                 /* AIRCHAT  */
    SESSION = 266,                 /* SESSION  */
    INFERENCE = 267,               /* INFERENCE  */
    PATTERN = 268,                 /* PATTERN  */
    ANALYSIS = 269,                /* ANALYSIS  */
    CREATE = 270,                  /* CREATE  */
    LOAD = 271,                    /* LOAD  */
    START = 272,                   /* START  */
    STOP = 273,                    /* STOP  */
    STATUS = 274,                  /* STATUS  */
    LIST = 275,                    /* LIST  */
    SWITCH = 276,                  /* SWITCH  */
    CONNECT = 277,                 /* CONNECT  */
    NEURAL = 278,                  /* NEURAL  */
    TREE = 279,                    /* TREE  */
    RESONANCE = 280,               /* RESONANCE  */
    ATTENTION = 281,               /* ATTENTION  */
    WEBSOCKET = 282,               /* WEBSOCKET  */
    PORT = 283,                    /* PORT  */
    HYPERGRAPH = 284,              /* HYPERGRAPH  */
    ECAN = 285,                    /* ECAN  */
    PLN = 286,                     /* PLN  */
    COGNITIVE = 287,               /* COGNITIVE  */
    SPATIAL = 288,                 /* SPATIAL  */
    TEMPORAL = 289,                /* TEMPORAL  */
    GGUF = 290,                    /* GGUF  */
    LLAMA = 291,                   /* LLAMA  */
    GUILE = 292,                   /* GUILE  */
    SCHEME = 293,                  /* SCHEME  */
    LBRACE = 294,                  /* LBRACE  */
    RBRACE = 295,                  /* RBRACE  */
    LPAREN = 296,                  /* LPAREN  */
    RPAREN = 297,                  /* RPAREN  */
    LBRACKET = 298,                /* LBRACKET  */
    RBRACKET = 299,                /* RBRACKET  */
    COMMA = 300,                   /* COMMA  */
    SEMICOLON = 301,               /* SEMICOLON  */
    COLON = 302,                   /* COLON  */
    EQUALS = 303,                  /* EQUALS  */
    NEWLINE = 304                  /* NEWLINE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define IDENTIFIER 258
#define STRING 259
#define FILEPATH 260
#define INTEGER 261
#define REAL 262
#define ORCHESTRATOR 263
#define MODEL 264
#define AIRCHAT 265
#define SESSION 266
#define INFERENCE 267
#define PATTERN 268
#define ANALYSIS 269
#define CREATE 270
#define LOAD 271
#define START 272
#define STOP 273
#define STATUS 274
#define LIST 275
#define SWITCH 276
#define CONNECT 277
#define NEURAL 278
#define TREE 279
#define RESONANCE 280
#define ATTENTION 281
#define WEBSOCKET 282
#define PORT 283
#define HYPERGRAPH 284
#define ECAN 285
#define PLN 286
#define COGNITIVE 287
#define SPATIAL 288
#define TEMPORAL 289
#define GGUF 290
#define LLAMA 291
#define GUILE 292
#define SCHEME 293
#define LBRACE 294
#define RBRACE 295
#define LPAREN 296
#define RPAREN 297
#define LBRACKET 298
#define RBRACKET 299
#define COMMA 300
#define SEMICOLON 301
#define COLON 302
#define EQUALS 303
#define NEWLINE 304

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 21 "r.y"

    char *string;
    int integer;
    float real;
    void *pointer;

#line 172 "r.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_R_TAB_H_INCLUDED  */

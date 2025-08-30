%{
/* Lexer for Cognitive rc Shell Components
 * Tokenizes input for orchestrator, execution engine, and airchat grammar
 */

#include "rc.h"
#include "r.h"  /* Generated from r.y */

int yylval_string(char *s);
int yylval_integer(int i);
int yylval_real(float f);

%}

%option noyywrap
%option case-insensitive

/* Define token patterns */

IDENTIFIER      [a-zA-Z_][a-zA-Z0-9_-]*
INTEGER         [0-9]+
REAL            [0-9]+\.[0-9]+
STRING          \"[^\"]*\"
FILEPATH        [a-zA-Z0-9_./\\-]+\.(gguf|bin|model|safetensors)
WHITESPACE      [ \t]+
NEWLINE         \n|\r\n
COMMENT         #.*

%%

{WHITESPACE}    { /* ignore whitespace */ }
{COMMENT}       { /* ignore comments */ }

"orchestrator"  { return ORCHESTRATOR; }
"model"         { return MODEL; }
"airchat"       { return AIRCHAT; }
"session"       { return SESSION; }
"inference"     { return INFERENCE; }
"pattern"       { return PATTERN; }
"analysis"      { return ANALYSIS; }

"create"        { return CREATE; }
"load"          { return LOAD; }
"start"         { return START; }
"stop"          { return STOP; }
"status"        { return STATUS; }
"list"          { return LIST; }
"switch"        { return SWITCH; }
"connect"       { return CONNECT; }

"neural"        { return NEURAL; }
"tree"          { return TREE; }
"resonance"     { return RESONANCE; }
"attention"     { return ATTENTION; }
"websocket"     { return WEBSOCKET; }
"port"          { return PORT; }

"hypergraph"    { return HYPERGRAPH; }
"ecan"          { return ECAN; }
"pln"           { return PLN; }
"cognitive"     { return COGNITIVE; }
"spatial"       { return SPATIAL; }
"temporal"      { return TEMPORAL; }

"gguf"          { return GGUF; }
"llama"         { return LLAMA; }
"guile"         { return GUILE; }
"scheme"        { return SCHEME; }

"{"             { return LBRACE; }
"}"             { return RBRACE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }

","             { return COMMA; }
";"             { return SEMICOLON; }
":"             { return COLON; }
"="             { return EQUALS; }

{FILEPATH}      { return yylval_string(yytext); }
{STRING}        { 
                  /* Remove quotes */
                  yytext[strlen(yytext)-1] = '\0';
                  return yylval_string(yytext+1);
                }
{REAL}          { return yylval_real(atof(yytext)); }
{INTEGER}       { return yylval_integer(atoi(yytext)); }
{IDENTIFIER}    { return yylval_string(yytext); }
{NEWLINE}       { return NEWLINE; }

.               { 
                  fprint(2, "Unexpected character: %c\n", yytext[0]);
                  return yytext[0];
                }

%%

/* Helper functions to set yylval */

int yylval_string(char *s) {
    yylval.string = strdup(s);
    return STRING;
}

int yylval_integer(int i) {
    yylval.integer = i;
    return INTEGER;
}

int yylval_real(float f) {
    yylval.real = f;
    return REAL;
}

/* Lexer initialization */
int cognitive_lexer_init(const char *input) {
    if (!input) return -1;
    
    /* In a full implementation, this would set up the input buffer
     * for the lexer. For now, we simulate basic tokenization. */
    
    return 0;
}

/* Lexer cleanup */
void cognitive_lexer_cleanup(void) {
    /* Cleanup lexer state */
}

/* Simple tokenizer for testing without full flex/yacc setup */
typedef struct {
    char *token;
    int type;
} TokenDef;

static TokenDef tokens[] = {
    {"orchestrator", ORCHESTRATOR},
    {"model", MODEL},
    {"airchat", AIRCHAT},
    {"session", SESSION},
    {"inference", INFERENCE},
    {"pattern", PATTERN},
    {"analysis", ANALYSIS},
    {"create", CREATE},
    {"load", LOAD},
    {"start", START},
    {"stop", STOP},
    {"status", STATUS},
    {"list", LIST},
    {"switch", SWITCH},
    {"neural", NEURAL},
    {"tree", TREE},
    {"resonance", RESONANCE},
    {"attention", ATTENTION},
    {"websocket", WEBSOCKET},
    {"port", PORT},
    {"hypergraph", HYPERGRAPH},
    {"ecan", ECAN},
    {"pln", PLN},
    {"cognitive", COGNITIVE},
    {"spatial", SPATIAL},
    {"temporal", TEMPORAL},
    {"gguf", GGUF},
    {"llama", LLAMA},
    {"guile", GUILE},
    {"scheme", SCHEME},
    {NULL, 0}
};

/* Simple token recognition for testing */
int recognize_token(const char *word) {
    if (!word) return 0;
    
    for (int i = 0; tokens[i].token; i++) {
        if (strcasecmp(word, tokens[i].token) == 0) {
            return tokens[i].type;
        }
    }
    
    /* Check for numbers */
    char *endptr;
    strtol(word, &endptr, 10);
    if (*endptr == '\0') return INTEGER;
    
    strtof(word, &endptr);
    if (*endptr == '\0') return REAL;
    
    /* Check for file paths */
    if (strstr(word, ".gguf") || strstr(word, ".bin") || 
        strstr(word, ".model") || strstr(word, ".safetensors")) {
        return FILEPATH;
    }
    
    /* Check for strings (quoted) */
    if (word[0] == '"' && word[strlen(word)-1] == '"') {
        return STRING;
    }
    
    /* Default to identifier */
    return IDENTIFIER;
}

/* Tokenize a line of input */
int tokenize_line(const char *line, char ***tokens, int **types) {
    if (!line || !tokens || !types) return -1;
    
    char *line_copy = strdup(line);
    if (!line_copy) return -1;
    
    /* Count tokens first */
    int token_count = 0;
    char *temp = strdup(line);
    char *token = strtok(temp, " \t\n\r");
    while (token) {
        token_count++;
        token = strtok(NULL, " \t\n\r");
    }
    free(temp);
    
    if (token_count == 0) {
        free(line_copy);
        *tokens = NULL;
        *types = NULL;
        return 0;
    }
    
    /* Allocate arrays */
    *tokens = malloc(token_count * sizeof(char*));
    *types = malloc(token_count * sizeof(int));
    if (!*tokens || !*types) {
        free(line_copy);
        return -1;
    }
    
    /* Tokenize */
    int i = 0;
    token = strtok(line_copy, " \t\n\r");
    while (token && i < token_count) {
        (*tokens)[i] = strdup(token);
        (*types)[i] = recognize_token(token);
        i++;
        token = strtok(NULL, " \t\n\r");
    }
    
    free(line_copy);
    return token_count;
}

/* Parse cognitive grammar command line */
int parse_cognitive_command(const char *line) {
    if (!line) return -1;
    
    char **tokens = NULL;
    int *types = NULL;
    int count = tokenize_line(line, &tokens, &types);
    
    if (count <= 0) return -1;
    
    /* Simple command parsing */
    if (count >= 2 && types[0] == ORCHESTRATOR && types[1] == CREATE) {
        fprint(1, "Parsed: orchestrator create command\n");
        if (count >= 3) {
            fprint(1, "  Name: %s\n", tokens[2]);
        }
    } else if (count >= 2 && types[0] == AIRCHAT && types[1] == CREATE) {
        fprint(1, "Parsed: airchat create command\n");
        if (count >= 3) {
            fprint(1, "  Session: %s\n", tokens[2]);
        }
    } else if (count >= 2 && types[0] == MODEL && types[1] == LOAD) {
        fprint(1, "Parsed: model load command\n");
        if (count >= 3) {
            fprint(1, "  Path: %s\n", tokens[2]);
        }
    } else {
        fprint(1, "Parsed: unknown command with %d tokens\n", count);
    }
    
    /* Cleanup */
    for (int i = 0; i < count; i++) {
        free(tokens[i]);
    }
    free(tokens);
    free(types);
    
    return 0;
}
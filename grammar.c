/* Simple Grammar Parser for Cognitive rc Shell Components
 * Basic parser for orchestrator, execution engine, and airchat interactions
 */

#include "rc.h"
#include "or.h"
#include "air.h"
#include "gguf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Simple token types */
typedef enum {
    TOK_UNKNOWN = 0,
    TOK_ORCHESTRATOR,
    TOK_MODEL,
    TOK_AIRCHAT,
    TOK_SESSION,
    TOK_INFERENCE,
    TOK_PATTERN,
    TOK_ANALYSIS,
    TOK_CREATE,
    TOK_LOAD,
    TOK_START,
    TOK_STOP,
    TOK_STATUS,
    TOK_LIST,
    TOK_SWITCH,
    TOK_IDENTIFIER,
    TOK_STRING,
    TOK_FILEPATH,
    TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

/* Simple tokenizer */
static Token tokenize_word(const char *word) {
    Token token = {TOK_UNKNOWN, NULL};
    
    if (!word) return token;
    
    token.value = strdup(word);
    
    if (strcasecmp(word, "orchestrator") == 0) token.type = TOK_ORCHESTRATOR;
    else if (strcasecmp(word, "model") == 0) token.type = TOK_MODEL;
    else if (strcasecmp(word, "airchat") == 0) token.type = TOK_AIRCHAT;
    else if (strcasecmp(word, "session") == 0) token.type = TOK_SESSION;
    else if (strcasecmp(word, "inference") == 0) token.type = TOK_INFERENCE;
    else if (strcasecmp(word, "pattern") == 0) token.type = TOK_PATTERN;
    else if (strcasecmp(word, "analysis") == 0) token.type = TOK_ANALYSIS;
    else if (strcasecmp(word, "create") == 0) token.type = TOK_CREATE;
    else if (strcasecmp(word, "load") == 0) token.type = TOK_LOAD;
    else if (strcasecmp(word, "start") == 0) token.type = TOK_START;
    else if (strcasecmp(word, "stop") == 0) token.type = TOK_STOP;
    else if (strcasecmp(word, "status") == 0) token.type = TOK_STATUS;
    else if (strcasecmp(word, "list") == 0) token.type = TOK_LIST;
    else if (strcasecmp(word, "switch") == 0) token.type = TOK_SWITCH;
    else if (strstr(word, ".gguf") || strstr(word, ".bin") || strstr(word, ".model")) {
        token.type = TOK_FILEPATH;
    } else if (word[0] == '"' && word[strlen(word)-1] == '"') {
        token.type = TOK_STRING;
        /* Remove quotes */
        memmove(token.value, token.value + 1, strlen(token.value) - 1);
        token.value[strlen(token.value) - 2] = '\0';
    } else {
        token.type = TOK_IDENTIFIER;
    }
    
    return token;
}

/* Parse line into tokens */
static int parse_tokens(const char *line, Token **tokens) {
    if (!line || !tokens) return -1;
    
    char *line_copy = strdup(line);
    if (!line_copy) return -1;
    
    /* Count tokens */
    int count = 0;
    char *temp = strdup(line);
    char *word = strtok(temp, " \t\n\r");
    while (word) {
        count++;
        word = strtok(NULL, " \t\n\r");
    }
    free(temp);
    
    if (count == 0) {
        free(line_copy);
        *tokens = NULL;
        return 0;
    }
    
    /* Allocate token array */
    *tokens = malloc(count * sizeof(Token));
    if (!*tokens) {
        free(line_copy);
        return -1;
    }
    
    /* Tokenize */
    int i = 0;
    word = strtok(line_copy, " \t\n\r");
    while (word && i < count) {
        (*tokens)[i] = tokenize_word(word);
        i++;
        word = strtok(NULL, " \t\n\r");
    }
    
    free(line_copy);
    return count;
}

/* Free tokens */
static void free_tokens(Token *tokens, int count) {
    if (!tokens) return;
    
    for (int i = 0; i < count; i++) {
        if (tokens[i].value) free(tokens[i].value);
    }
    free(tokens);
}

/* Parse cognitive grammar command */
int cognitive_grammar_parse(const char *input) {
    if (!input) return -1;
    
    Token *tokens = NULL;
    int count = parse_tokens(input, &tokens);
    
    if (count <= 0) return -1;
    
    int result = 0;
    
    /* Simple command parsing */
    if (count >= 2 && tokens[0].type == TOK_ORCHESTRATOR && tokens[1].type == TOK_CREATE) {
        fprint(1, "Grammar: Parsed orchestrator create");
        if (count >= 3 && tokens[2].type == TOK_IDENTIFIER) {
            fprint(1, " '%s'", tokens[2].value);
        }
        fprint(1, "\n");
    } else if (count >= 3 && tokens[0].type == TOK_ORCHESTRATOR && tokens[1].type == TOK_LOAD && 
               tokens[2].type == TOK_IDENTIFIER && count >= 4 && tokens[3].type == TOK_FILEPATH) {
        fprint(1, "Grammar: Parsed orchestrator load '%s' '%s'\n", tokens[2].value, tokens[3].value);
    } else if (count >= 2 && tokens[0].type == TOK_AIRCHAT && tokens[1].type == TOK_CREATE) {
        fprint(1, "Grammar: Parsed airchat create");
        if (count >= 3 && tokens[2].type == TOK_IDENTIFIER) {
            fprint(1, " '%s'", tokens[2].value);
        }
        fprint(1, "\n");
    } else if (count >= 2 && tokens[0].type == TOK_MODEL && tokens[1].type == TOK_LOAD) {
        fprint(1, "Grammar: Parsed model load");
        if (count >= 3 && tokens[2].type == TOK_FILEPATH) {
            fprint(1, " '%s'", tokens[2].value);
        }
        fprint(1, "\n");
    } else if (count >= 2 && tokens[0].type == TOK_PATTERN && tokens[1].type == TOK_ANALYSIS) {
        fprint(1, "Grammar: Parsed pattern analysis");
        if (count >= 3) {
            fprint(1, " '%s'", tokens[2].value);
        }
        fprint(1, "\n");
    } else {
        fprint(1, "Grammar: Parsed unknown command with %d tokens\n", count);
        for (int i = 0; i < count; i++) {
            fprint(1, "  [%d] %s (type: %d)\n", i, tokens[i].value, tokens[i].type);
        }
        result = -1;
    }
    
    free_tokens(tokens, count);
    return result;
}

/* Initialize cognitive grammar parser */
int cognitive_grammar_init(void) {
    fprint(1, "Cognitive grammar parser initialized\n");
    return 0;
}

/* Cleanup cognitive grammar parser */
void cognitive_grammar_cleanup(void) {
    fprint(1, "Cognitive grammar parser cleaned up\n");
}

/* Shell command to test grammar parsing */
void b_grammar_parse(char **av) {
    if (!av[1]) {
        rc_error("grammar-parse: usage: grammar-parse <command>");
        return;
    }
    
    /* Reconstruct command line from arguments */
    int total_len = 0;
    for (int i = 1; av[i]; i++) {
        total_len += strlen(av[i]) + 1;
    }
    
    char *command = malloc(total_len + 1);
    if (!command) {
        rc_error("grammar-parse: memory allocation failed");
        return;
    }
    
    strcpy(command, av[1]);
    for (int i = 2; av[i]; i++) {
        strcat(command, " ");
        strcat(command, av[i]);
    }
    
    if (cognitive_grammar_parse(command) == 0) {
        fprint(1, "Grammar parsing successful\n");
    } else {
        fprint(1, "Grammar parsing failed or unknown command\n");
    }
    
    free(command);
}
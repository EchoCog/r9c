%{
/* Yacc Grammar for Cognitive rc Shell Components
 * Defines grammar for orchestrator, execution engine, and airchat interactions
 */

#include "rc.h"
#include "or.h"
#include "air.h"
#include "gguf.h"

int yylex(void);
void yyerror(const char *s);

/* Global state for grammar parsing */
static Orchestrator *current_orchestrator = NULL;
static ChatSession *current_chat_session = NULL;
static gguf_model *current_model = NULL;

%}

%union {
    char *string;
    int integer;
    float real;
    void *pointer;
}

%token <string> IDENTIFIER STRING FILEPATH
%token <integer> INTEGER
%token <real> REAL
%token ORCHESTRATOR MODEL AIRCHAT SESSION INFERENCE PATTERN ANALYSIS
%token CREATE LOAD START STOP STATUS LIST SWITCH CONNECT
%token NEURAL TREE RESONANCE ATTENTION WEBSOCKET PORT
%token HYPERGRAPH ECAN PLN COGNITIVE SPATIAL TEMPORAL
%token GGUF LLAMA GUILE SCHEME
%token LBRACE RBRACE LPAREN RPAREN LBRACKET RBRACKET
%token COMMA SEMICOLON COLON EQUALS
%token NEWLINE

%type <pointer> orchestrator_spec model_spec airchat_spec
%type <pointer> neural_tree_spec pattern_analysis_spec
%type <string> identifier_list parameter_list

%start program

%%

program:
    /* empty */
    | program statement NEWLINE
    | program NEWLINE
    ;

statement:
    orchestrator_statement
    | model_statement
    | airchat_statement
    | analysis_statement
    | websocket_statement
    ;

orchestrator_statement:
    ORCHESTRATOR CREATE IDENTIFIER {
        Orchestrator *orc = orchestrator_create($3, (uint32_t)time(NULL));
        if (orc) {
            orchestrator_init(orc);
            orchestrator_start(orc);
            current_orchestrator = orc;
            fprint(1, "Created orchestrator: %s\n", $3);
        }
        free($3);
    }
    | ORCHESTRATOR LOAD IDENTIFIER FILEPATH {
        if (current_orchestrator) {
            if (orchestrator_load_model(current_orchestrator, $4) == 0) {
                fprint(1, "Loaded model %s into orchestrator %s\n", $4, $3);
            }
        }
        free($3);
        free($4);
    }
    | ORCHESTRATOR INFERENCE IDENTIFIER STRING {
        if (current_orchestrator) {
            char *response = NULL;
            if (orchestrator_inference(current_orchestrator, $4, &response) == 0 && response) {
                fprint(1, "Inference: %s\n", response);
                free(response);
            }
        }
        free($3);
        free($4);
    }
    | ORCHESTRATOR STATUS {
        /* Show orchestrator status - implemented in shell command */
        fprint(1, "Orchestrator status requested\n");
    }
    ;

model_statement:
    MODEL LOAD FILEPATH {
        gguf_model *model = gguf_load_model($3);
        if (model) {
            current_model = model;
            fprint(1, "Loaded GGUF model: %s\n", $3);
        }
        free($3);
    }
    | MODEL STATUS {
        if (current_model) {
            char *info = NULL;
            if (gguf_get_model_info(current_model, &info) == 0 && info) {
                fprint(1, "%s", info);
                free(info);
            }
        } else {
            fprint(1, "No model loaded\n");
        }
    }
    ;

airchat_statement:
    AIRCHAT CREATE IDENTIFIER {
        ChatSession *session = airchat_create_session($3, NULL);
        if (session) {
            current_chat_session = session;
            fprint(1, "Created airchat session: %s\n", $3);
        }
        free($3);
    }
    | AIRCHAT SESSION IDENTIFIER STRING {
        if (current_chat_session) {
            char *response = NULL;
            if (airchat_send_message(current_chat_session, $4, &response) == 0 && response) {
                fprint(1, "Response: %s\n", response);
                free(response);
            }
        }
        free($3);
        free($4);
    }
    | AIRCHAT SWITCH IDENTIFIER {
        if (airchat_switch_session($3) == 0) {
            fprint(1, "Switched to session: %s\n", $3);
        }
        free($3);
    }
    | AIRCHAT LIST {
        /* List sessions - implemented in shell command */
        fprint(1, "Airchat session list requested\n");
    }
    ;

analysis_statement:
    PATTERN ANALYSIS STRING {
        /* Trigger pattern analysis */
        fprint(1, "Pattern analysis requested for: %s\n", $3);
        free($3);
    }
    | NEURAL TREE ANALYSIS {
        /* Trigger neural tree analysis */
        fprint(1, "Neural tree analysis requested\n");
    }
    | RESONANCE ANALYSIS {
        /* Trigger resonance analysis */
        fprint(1, "Resonance analysis requested\n");
    }
    | SPATIAL ANALYSIS STRING {
        /* Trigger spatial analysis */
        fprint(1, "Spatial analysis requested for: %s\n", $3);
        free($3);
    }
    ;

websocket_statement:
    WEBSOCKET START PORT INTEGER {
        fprint(1, "WebSocket server start requested on port %d\n", $4);
    }
    | WEBSOCKET STOP {
        fprint(1, "WebSocket server stop requested\n");
    }
    | WEBSOCKET STATUS {
        fprint(1, "WebSocket status requested\n");
    }
    ;

orchestrator_spec:
    LBRACE parameter_list RBRACE {
        /* Parse orchestrator parameters */
        $$ = NULL; /* Placeholder */
    }
    ;

model_spec:
    LBRACE parameter_list RBRACE {
        /* Parse model parameters */
        $$ = NULL; /* Placeholder */
    }
    ;

airchat_spec:
    LBRACE parameter_list RBRACE {
        /* Parse airchat parameters */
        $$ = NULL; /* Placeholder */
    }
    ;

neural_tree_spec:
    NEURAL TREE LBRACE parameter_list RBRACE {
        /* Parse neural tree specification */
        $$ = NULL; /* Placeholder */
    }
    ;

pattern_analysis_spec:
    PATTERN ANALYSIS LBRACE parameter_list RBRACE {
        /* Parse pattern analysis specification */
        $$ = NULL; /* Placeholder */
    }
    ;

parameter_list:
    /* empty */ {
        $$ = NULL;
    }
    | IDENTIFIER EQUALS STRING {
        /* Single parameter */
        $$ = $3;
        free($1);
    }
    | parameter_list COMMA IDENTIFIER EQUALS STRING {
        /* Multiple parameters */
        $$ = $1; /* Keep first parameter for now */
        free($3);
        free($5);
    }
    ;

identifier_list:
    IDENTIFIER {
        $$ = $1;
    }
    | identifier_list COMMA IDENTIFIER {
        /* Concatenate identifiers - simplified */
        $$ = $1;
        free($3);
    }
    ;

%%

void yyerror(const char *s) {
    fprint(2, "Grammar error: %s\n", s);
}

/* Initialize cognitive grammar parser */
int cognitive_grammar_init(void) {
    current_orchestrator = NULL;
    current_chat_session = NULL;
    current_model = NULL;
    return 0;
}

/* Parse cognitive grammar input */
int cognitive_grammar_parse(const char *input) {
    if (!input) return -1;
    
    /* In a full implementation, this would set up the lexer with the input
     * and call yyparse(). For now, we'll simulate basic parsing. */
    
    if (strstr(input, "orchestrator create")) {
        fprint(1, "Parsing orchestrator creation command\n");
        return 0;
    } else if (strstr(input, "airchat create")) {
        fprint(1, "Parsing airchat creation command\n");
        return 0;
    } else if (strstr(input, "model load")) {
        fprint(1, "Parsing model load command\n");
        return 0;
    }
    
    fprint(2, "Unknown cognitive grammar input: %s\n", input);
    return -1;
}

/* Cleanup cognitive grammar parser */
void cognitive_grammar_cleanup(void) {
    current_orchestrator = NULL;
    current_chat_session = NULL;
    if (current_model) {
        gguf_free_model(current_model);
        current_model = NULL;
    }
}
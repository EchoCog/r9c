/* Cognitive Extensions for rc Shell
 * Provides hooks and interfaces for cognitive grammar integration
 */

#ifndef COGNITIVE_H
#define COGNITIVE_H

/* Don't re-include rc.h as it should already be included by the caller */

/* Configuration flags for cognitive features */
#ifndef ENABLE_COGNITIVE_GRAMMAR
#define ENABLE_COGNITIVE_GRAMMAR 0
#endif

#ifndef ENABLE_IPC_EXTENSIONS  
#define ENABLE_IPC_EXTENSIONS 0
#endif

#ifndef ENABLE_SCHEME_INTEGRATION
#define ENABLE_SCHEME_INTEGRATION 0
#endif

#ifndef ENABLE_TENSOR_OPERATIONS
#define ENABLE_TENSOR_OPERATIONS 0
#endif

/* Forward declarations */
typedef struct CognitiveModule CognitiveModule;
typedef struct TensorMembrane TensorMembrane;
typedef struct AttentionState AttentionState;

/* IPC Extension Interface */
#if ENABLE_IPC_EXTENSIONS
extern int rc_ipc_init(void);
extern int rc_ipc_listen(const char *path);
extern int rc_ipc_connect(const char *path);
extern int rc_ipc_send(int fd, const char *data, size_t len);
extern int rc_ipc_recv(int fd, char *buffer, size_t len);
extern void rc_ipc_cleanup(void);
#else
#define rc_ipc_init() 0
#define rc_ipc_listen(path) -1
#define rc_ipc_connect(path) -1
#define rc_ipc_send(fd, data, len) -1
#define rc_ipc_recv(fd, buffer, len) -1
#define rc_ipc_cleanup() do {} while(0)
#endif

/* Scheme Integration Interface */
#if ENABLE_SCHEME_INTEGRATION
extern int scheme_init(void);
extern int scheme_eval(const char *expr);
extern char *scheme_call(const char *func, char **args);
extern void scheme_cleanup(void);
#else
#define scheme_init() 0
#define scheme_eval(expr) 0
#define scheme_call(func, args) NULL
#define scheme_cleanup() do {} while(0)
#endif

/* Tensor Operations Interface */
#if ENABLE_TENSOR_OPERATIONS
extern void *tensor_create(int *dims, int ndims);
extern void tensor_destroy(void *tensor);
extern int tensor_compute(void *tensor, const char *op);
extern void *tensor_membrane_alloc(int prime_factors[], int count);
extern void tensor_membrane_free(void *membrane);
#else
#define tensor_create(dims, ndims) NULL
#define tensor_destroy(tensor) do {} while(0)
#define tensor_compute(tensor, op) -1
#define tensor_membrane_alloc(factors, count) NULL
#define tensor_membrane_free(membrane) do {} while(0)
#endif

/* Cognitive Module Plugin System */
struct CognitiveModule {
    const char *name;
    const char *version;
    int (*init)(void);
    int (*process)(const char *input, char **output);
    void (*cleanup)(void);
    struct CognitiveModule *next;
};

extern int register_cognitive_module(CognitiveModule *module);
extern CognitiveModule *find_cognitive_module(const char *name);
extern void unregister_cognitive_module(const char *name);
extern void list_cognitive_modules(void);

/* Command Processing Hooks */
typedef enum {
    HOOK_PRE_PARSE,     /* Before command parsing */
    HOOK_POST_PARSE,    /* After command parsing */
    HOOK_PRE_EXEC,      /* Before command execution */
    HOOK_POST_EXEC,     /* After command execution */
    HOOK_ERROR,         /* On error conditions */
    HOOK_COUNT
} HookType;

typedef int (*HookFunction)(HookType type, void *data);

extern int register_cognitive_hook(HookType type, HookFunction func);
extern int unregister_cognitive_hook(HookType type, HookFunction func);
extern int invoke_cognitive_hooks(HookType type, void *data);

/* Cognitive State Management */
struct AttentionState {
    float total_attention;
    int active_patterns;
    void *pattern_data;
    unsigned long timestamp;
};

extern AttentionState *get_attention_state(void);
extern int update_attention_state(AttentionState *state);
extern void reset_attention_state(void);

/* Built-in Cognitive Commands */
extern void b_ipc_listen(char **);
extern void b_ipc_connect(char **);
extern void b_ipc_send(char **);
extern void b_ipc_recv(char **);
extern void b_scheme_eval(char **);
extern void b_hypergraph_encode(char **);
extern void b_pattern_match(char **);
extern void b_attention_allocate(char **);
extern void b_tensor_create(char **);
extern void b_tensor_op(char **);
extern void b_membrane_alloc(char **);
extern void b_cognitive_status(char **);

/* Example Cognitive Commands (when ENABLE_COGNITIVE_EXAMPLES is defined) */
extern void b_load_example_modules(char **);
extern void b_test_pattern(char **);
extern void b_test_attention(char **);

/* Initialization and Cleanup */
extern int cognitive_init(void);
extern void cognitive_cleanup(void);

#endif /* COGNITIVE_H */
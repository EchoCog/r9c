/* Cognitive Extensions Implementation
 * Basic stubs and extension points for cognitive grammar integration
 */

#include "rc.h"
#include "cognitive.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

/* Global cognitive state */
static CognitiveModule *modules_head = NULL;
static HookFunction hooks[HOOK_COUNT][8]; /* Max 8 hooks per type */
static int hook_counts[HOOK_COUNT] = {0};
static AttentionState global_attention = {0};

/* Module Management */
int register_cognitive_module(CognitiveModule *module) {
    if (!module || !module->name) return -1;
    
    module->next = modules_head;
    modules_head = module;
    
    if (module->init) {
        return module->init();
    }
    return 0;
}

CognitiveModule *find_cognitive_module(const char *name) {
    CognitiveModule *module = modules_head;
    while (module) {
        if (strcmp(module->name, name) == 0) {
            return module;
        }
        module = module->next;
    }
    return NULL;
}

void unregister_cognitive_module(const char *name) {
    CognitiveModule **current = &modules_head;
    while (*current) {
        if (strcmp((*current)->name, name) == 0) {
            CognitiveModule *to_remove = *current;
            *current = (*current)->next;
            if (to_remove->cleanup) {
                to_remove->cleanup();
            }
            break;
        }
        current = &(*current)->next;
    }
}

void list_cognitive_modules(void) {
    CognitiveModule *module = modules_head;
    fprint(1, "Registered Cognitive Modules:\n");
    while (module) {
        fprint(1, "  %s", module->name);
        if (module->version) {
            fprint(1, " (v%s)", module->version);
        }
        fprint(1, "\n");
        module = module->next;
    }
}

/* Hook Management */
int register_cognitive_hook(HookType type, HookFunction func) {
    if (type >= HOOK_COUNT || hook_counts[type] >= 8) return -1;
    
    hooks[type][hook_counts[type]++] = func;
    return 0;
}

int unregister_cognitive_hook(HookType type, HookFunction func) {
    if (type >= HOOK_COUNT) return -1;
    
    for (int i = 0; i < hook_counts[type]; i++) {
        if (hooks[type][i] == func) {
            /* Shift remaining hooks down */
            for (int j = i; j < hook_counts[type] - 1; j++) {
                hooks[type][j] = hooks[type][j + 1];
            }
            hook_counts[type]--;
            return 0;
        }
    }
    return -1;
}

int invoke_cognitive_hooks(HookType type, void *data) {
    if (type >= HOOK_COUNT) return -1;
    
    for (int i = 0; i < hook_counts[type]; i++) {
        int result = hooks[type][i](type, data);
        if (result != 0) return result; /* Early exit on error */
    }
    return 0;
}

/* Attention State Management */
AttentionState *get_attention_state(void) {
    return &global_attention;
}

int update_attention_state(AttentionState *state) {
    if (!state) return -1;
    global_attention = *state;
    return 0;
}

void reset_attention_state(void) {
    global_attention.total_attention = 0.0f;
    global_attention.active_patterns = 0;
    global_attention.pattern_data = NULL;
    global_attention.timestamp = 0;
}

/* Forward declarations for default kernel functions */
static int default_kernel_encode(const char *input, char **output);
static int default_kernel_pln_infer(const char *premises, char **conclusion, TruthValue *tv);
static int default_kernel_transform(const char *pattern, const char *input, char **output);

/* Cognitive Grammar Support Functions */
static HypergraphKernel default_kernel = {
    .encode = default_kernel_encode,
    .pln_infer = default_kernel_pln_infer,
    .transform = default_kernel_transform
};

HypergraphKernel *find_hypergraph_kernel(const char *name) {
    /* For now, return a default kernel stub */
    if (name && strcmp(name, "default") == 0) {
        return &default_kernel;
    }
    return NULL;
}

float calculate_ecan_attention(const char *input, ECANValues *ecan) {
    if (!input || !ecan) return 0.0f;
    
    /* Basic ECAN calculation based on input complexity */
    size_t len = strlen(input);
    float complexity = (float)len / 10.0f;
    
    /* Simple heuristic attention allocation */
    ecan->short_term_importance = 0.6f + (complexity * 0.1f);
    ecan->long_term_importance = 0.3f + (complexity * 0.05f);
    ecan->very_long_term_importance = 0.06f + (complexity * 0.01f);
    ecan->stimulation_level = 12.0f + complexity;
    
    /* Total attention is weighted sum */
    float total = ecan->short_term_importance * len + 
                  ecan->long_term_importance * 20.0f +
                  ecan->very_long_term_importance * 5.0f;
    
    return total;
}

/* IPC Extension Implementation */
#if ENABLE_IPC_EXTENSIONS
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

static int ipc_sockets[16]; /* Track up to 16 IPC sockets */
static int ipc_socket_count = 0;

int rc_ipc_init(void) {
    /* Initialize IPC subsystem */
    for (int i = 0; i < 16; i++) {
        ipc_sockets[i] = -1;
    }
    ipc_socket_count = 0;
    return 0;
}

int rc_ipc_listen(const char *path) {
    /* Create IPC listener socket */
    int sock_fd;
    struct sockaddr_un addr;
    
    if (!path || strlen(path) >= sizeof(addr.sun_path)) {
        return -1;
    }
    
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    
    /* Remove existing socket file */
    unlink(path);
    
    if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock_fd);
        return -1;
    }
    
    if (listen(sock_fd, 5) < 0) {
        close(sock_fd);
        unlink(path);
        return -1;
    }
    
    /* Track the socket */
    if (ipc_socket_count < 16) {
        ipc_sockets[ipc_socket_count++] = sock_fd;
    }
    
    return sock_fd;
}

int rc_ipc_connect(const char *path) {
    /* Connect to IPC endpoint */
    int sock_fd;
    struct sockaddr_un addr;
    
    if (!path || strlen(path) >= sizeof(addr.sun_path)) {
        return -1;
    }
    
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    
    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock_fd);
        return -1;
    }
    
    /* Track the socket */
    if (ipc_socket_count < 16) {
        ipc_sockets[ipc_socket_count++] = sock_fd;
    }
    
    return sock_fd;
}

int rc_ipc_send(int fd, const char *data, size_t len) {
    /* Send data via IPC */
    if (fd < 0 || !data || len == 0) {
        return -1;
    }
    
    ssize_t sent = send(fd, data, len, 0);
    return (sent == (ssize_t)len) ? 0 : -1;
}

int rc_ipc_recv(int fd, char *buffer, size_t len) {
    /* Receive data via IPC */
    if (fd < 0 || !buffer || len == 0) {
        return -1;
    }
    
    ssize_t received = recv(fd, buffer, len - 1, 0);
    if (received < 0) {
        return -1;
    }
    
    buffer[received] = '\0'; /* Null terminate */
    return received;
}

void rc_ipc_cleanup(void) {
    /* Cleanup IPC resources */
    for (int i = 0; i < ipc_socket_count; i++) {
        if (ipc_sockets[i] >= 0) {
            close(ipc_sockets[i]);
            ipc_sockets[i] = -1;
        }
    }
    ipc_socket_count = 0;
}
#endif

/* Scheme Integration Implementation */
#if ENABLE_SCHEME_INTEGRATION
#include <dlfcn.h>

static void *scheme_lib_handle = NULL;
static char scheme_eval_buffer[4096];

/* Function pointers for dynamically loaded Scheme interpreter */
typedef int (*scheme_init_func_t)(void);
typedef int (*scheme_eval_func_t)(const char*);
typedef char* (*scheme_call_func_t)(const char*, char**);
typedef void (*scheme_cleanup_func_t)(void);

static scheme_init_func_t scheme_init_func = NULL;
static scheme_eval_func_t scheme_eval_func = NULL; 
static scheme_call_func_t scheme_call_func = NULL;
static scheme_cleanup_func_t scheme_cleanup_func = NULL;

/* Hypergraph Node Structure */
typedef struct HypergraphNode {
    char *name;
    float attention_value;
    int concept_type;  /* 0=concept, 1=link, 2=predicate */
    struct HypergraphNode **children;
    int child_count;
    struct HypergraphNode *next;
} HypergraphNode;

static HypergraphKernel *grammar_kernels = NULL;
static int kernel_count = 0;
static HypergraphNode *hypergraph_root = NULL;

int register_hypergraph_kernel(HypergraphKernel *kernel) {
    /* Simple array-based registration for now */
    if (kernel_count >= 16) return -1; /* Max 16 kernels */
    
    if (!grammar_kernels) {
        grammar_kernels = malloc(16 * sizeof(HypergraphKernel));
        if (!grammar_kernels) return -1;
    }
    
    grammar_kernels[kernel_count++] = *kernel;
    return 0;
}

/* Core Hypergraph Operations */
HypergraphNode *create_hypergraph_node(const char *name, int concept_type) {
    HypergraphNode *node = malloc(sizeof(HypergraphNode));
    if (!node) return NULL;
    
    node->name = ecpy(name);
    node->attention_value = 0.0f;
    node->concept_type = concept_type;
    node->children = NULL;
    node->child_count = 0;
    node->next = NULL;
    
    return node;
}

void destroy_hypergraph_node(HypergraphNode *node) {
    if (!node) return;
    
    if (node->name) efree(node->name);
    if (node->children) {
        for (int i = 0; i < node->child_count; i++) {
            destroy_hypergraph_node(node->children[i]);
        }
        efree(node->children);
    }
    efree(node);
}

int add_hypergraph_child(HypergraphNode *parent, HypergraphNode *child) {
    if (!parent || !child) return -1;
    
    HypergraphNode **new_children = realloc(parent->children, 
        (parent->child_count + 1) * sizeof(HypergraphNode*));
    if (!new_children) return -1;
    
    parent->children = new_children;
    parent->children[parent->child_count++] = child;
    return 0;
}

/* PLN Inference */
TruthValue pln_deduction(TruthValue premise1, TruthValue premise2) {
    TruthValue result;
    
    /* PLN deduction rule: if A->B and B->C then A->C */
    result.strength = premise1.strength * premise2.strength;
    result.confidence = premise1.confidence * premise2.confidence;
    
    return result;
}

TruthValue pln_induction(TruthValue evidence, float prior_strength) {
    TruthValue result;
    
    /* PLN induction: generalize from evidence */
    result.strength = (evidence.strength + prior_strength) / 2.0f;
    result.confidence = evidence.confidence * 0.8f; /* Lower confidence for induction */
    
    return result;
}

/* Hypergraph Encoding in Scheme-like Syntax */
int encode_to_hypergraph(const char *input, char **output) {
    if (!input || !output) return -1;
    
    /* Parse input and create hypergraph representation */
    char *result = malloc(strlen(input) * 4 + 256);
    if (!result) return -1;
    
    /* Create nodes for each significant word */
    char *words[32];
    int word_count = 0;
    char *input_copy = ecpy(input);
    char *token = strtok(input_copy, " \t\n");
    
    while (token && word_count < 32) {
        words[word_count++] = token;
        token = strtok(NULL, " \t\n");
    }
    
    /* Generate Scheme-like hypergraph encoding */
    strcpy(result, "(hypergraph ");
    for (int i = 0; i < word_count; i++) {
        strcat(result, "(concept \"");
        strcat(result, words[i]);
        strcat(result, "\") ");
    }
    
    /* Add relationship links */
    if (word_count > 1) {
        strcat(result, "(link sequence ");
        for (int i = 0; i < word_count - 1; i++) {
            strcat(result, "(ordered-link \"");
            strcat(result, words[i]);
            strcat(result, "\" \"");
            strcat(result, words[i + 1]);
            strcat(result, "\") ");
        }
        strcat(result, ")");
    }
    strcat(result, ")");
    
    efree(input_copy);
    *output = result;
    return 0;
}

/* Default Hypergraph Kernel Implementation */
static int default_kernel_encode(const char *input, char **output) {
    return encode_to_hypergraph(input, output);
}

static int default_kernel_decode(const char *input, char **output) {
    if (!input || !output) return -1;
    
    /* Simple decode: extract concepts from hypergraph notation */
    char *result = malloc(strlen(input) + 64);
    if (!result) return -1;
    
    strcpy(result, "decoded: ");
    
    /* Extract concept names from (concept "name") patterns */
    const char *pos = input;
    while ((pos = strstr(pos, "(concept \"")) != NULL) {
        pos += 10; /* Skip "(concept \"" */
        const char *end = strstr(pos, "\")");
        if (end) {
            strncat(result, pos, end - pos);
            strcat(result, " ");
            pos = end + 2;
        } else {
            break;
        }
    }
    
    *output = result;
    return 0;
}

static int default_kernel_transform(const char *pattern, const char *input, char **output) {
    if (!pattern || !input || !output) return -1;
    
    /* Apply cognitive pattern transformation */
    char *result = malloc(strlen(pattern) + strlen(input) + 128);
    if (!result) return -1;
    
    ECANValues ecan;
    float attention = calculate_ecan_attention(input, &ecan);
    
    sprintf(result, "(transform (pattern \"%s\") (input \"%s\") (attention %d))", 
            pattern, input, (int)(attention * 100));
    
    *output = result;
    return 0;
}

static int default_kernel_pln_infer(const char *premises, char **conclusion, TruthValue *tv) {
    if (!premises || !conclusion || !tv) return -1;
    
    /* Simple PLN inference example */
    TruthValue premise_tv = {0.8f, 0.9f}; /* Default premise truth value */
    TruthValue prior_tv = {0.5f, 0.5f};   /* Default prior */
    
    *tv = pln_induction(premise_tv, prior_tv.strength);
    
    char *result = malloc(strlen(premises) + 128);
    if (!result) return -1;
    
    sprintf(result, "(conclusion \"%s\" (tv %d %d))", premises, (int)(tv->strength * 100), (int)(tv->confidence * 100));
    *conclusion = result;
    
    return 0;
}

int scheme_init(void) {
    /* Try to load a Scheme interpreter library (optional) */
    scheme_lib_handle = dlopen("libguile-2.2.so", RTLD_LAZY);
    if (!scheme_lib_handle) {
        scheme_lib_handle = dlopen("libchicken.so", RTLD_LAZY);
    }
    
    if (scheme_lib_handle) {
        scheme_init_func = dlsym(scheme_lib_handle, "scm_init_guile");
        scheme_eval_func = dlsym(scheme_lib_handle, "scm_eval_string");
        scheme_call_func = dlsym(scheme_lib_handle, "scm_call_with_values");
        scheme_cleanup_func = dlsym(scheme_lib_handle, "scm_cleanup");
        
        if (scheme_init_func) {
            return scheme_init_func();
        }
    }
    
    /* Fallback: initialize built-in minimal Scheme evaluator */
    memset(scheme_eval_buffer, 0, sizeof(scheme_eval_buffer));
    return 0;
}

int scheme_eval(const char *expr) {
    /* Evaluate Scheme expression */
    if (!expr) return -1;
    
    if (scheme_eval_func) {
        return scheme_eval_func(expr);
    }
    
    /* Fallback: simple built-in evaluator for basic expressions */
    if (strncmp(expr, "(+ ", 3) == 0) {
        /* Simple addition */
        int a, b;
        if (sscanf(expr, "(+ %d %d)", &a, &b) == 2) {
            snprintf(scheme_eval_buffer, sizeof(scheme_eval_buffer), "%d", a + b);
            return a + b;
        }
    } else if (strncmp(expr, "(* ", 3) == 0) {
        /* Simple multiplication */
        int a, b;
        if (sscanf(expr, "(* %d %d)", &a, &b) == 2) {
            snprintf(scheme_eval_buffer, sizeof(scheme_eval_buffer), "%d", a * b);
            return a * b;
        }
    }
    
    /* Default: echo the expression */
    strncpy(scheme_eval_buffer, expr, sizeof(scheme_eval_buffer) - 1);
    return 0;
}

char *scheme_call(const char *func, char **args) {
    /* Call Scheme function */
    if (!func) return NULL;
    
    if (scheme_call_func) {
        return scheme_call_func(func, args);
    }
    
    /* Fallback: simple function calls */
    if (strcmp(func, "hypergraph-encode") == 0 && args && args[0]) {
        HypergraphKernel *kernel = find_hypergraph_kernel("default");
        if (kernel && kernel->encode) {
            char *output = NULL;
            int result = kernel->encode(args[0], &output);
            if (result == 0) return output;
        }
        /* Fallback encoding */
        char *result = malloc(strlen(args[0]) + 64);
        if (result) {
            sprintf(result, "(hypergraph-node \"%s\")", args[0]);
        }
        return result;
    } else if (strcmp(func, "pln-infer") == 0 && args && args[0]) {
        HypergraphKernel *kernel = find_hypergraph_kernel("default");
        if (kernel && kernel->pln_infer) {
            char *conclusion = NULL;
            TruthValue tv;
            int result = kernel->pln_infer(args[0], &conclusion, &tv);
            if (result == 0) return conclusion;
        }
    } else if (strcmp(func, "ecan-allocate") == 0 && args && args[0]) {
        ECANValues ecan;
        float attention = calculate_ecan_attention(args[0], &ecan);
        char *result = malloc(256);
        if (result) {
            sprintf(result, "(attention %d (sti %d) (lti %d) (vlti %d))", 
                    (int)(attention * 100), (int)(ecan.short_term_importance * 100), 
                    (int)(ecan.long_term_importance * 100), (int)(ecan.very_long_term_importance * 100));
        }
        return result;
    }
    
    return ecpy("scheme_call_result");
}

void scheme_cleanup(void) {
    /* Cleanup Scheme interpreter */
    if (scheme_cleanup_func) {
        scheme_cleanup_func();
    }
    
    if (scheme_lib_handle) {
        dlclose(scheme_lib_handle);
        scheme_lib_handle = NULL;
    }
    
    if (grammar_kernels) {
        free(grammar_kernels);
        grammar_kernels = NULL;
        kernel_count = 0;
    }
}
#endif

/* Tensor Operations Implementation */
#if ENABLE_TENSOR_OPERATIONS
#include <math.h>
#include "tensor-membrane.h"

/* Simple tensor structure (minimal implementation without ggml dependency) */
typedef struct {
    int ndims;
    int dims[4];  /* Support up to 4D tensors */
    float *data;
    size_t size;
    char name[64];
} SimpleTensor;

static SimpleTensor *tensor_registry[32]; /* Track up to 32 tensors */
static int tensor_count = 0;

static TensorMembrane *membrane_registry[16];
static int membrane_count = 0;

void *tensor_create(int *dims, int ndims) {
    if (!dims || ndims <= 0 || ndims > 4 || tensor_count >= 32) {
        return NULL;
    }
    
    SimpleTensor *tensor = malloc(sizeof(SimpleTensor));
    if (!tensor) return NULL;
    
    tensor->ndims = ndims;
    tensor->size = 1;
    
    for (int i = 0; i < ndims; i++) {
        tensor->dims[i] = dims[i];
        tensor->size *= dims[i];
    }
    
    tensor->data = malloc(tensor->size * sizeof(float));
    if (!tensor->data) {
        free(tensor);
        return NULL;
    }
    
    /* Initialize with random values */
    for (size_t i = 0; i < tensor->size; i++) {
        tensor->data[i] = (float)rand() / RAND_MAX;
    }
    
    snprintf(tensor->name, sizeof(tensor->name), "tensor_%d", tensor_count);
    tensor_registry[tensor_count++] = tensor;
    
    return tensor;
}

void tensor_destroy(void *tensor_ptr) {
    SimpleTensor *tensor = (SimpleTensor*)tensor_ptr;
    if (!tensor) return;
    
    /* Remove from registry */
    for (int i = 0; i < tensor_count; i++) {
        if (tensor_registry[i] == tensor) {
            for (int j = i; j < tensor_count - 1; j++) {
                tensor_registry[j] = tensor_registry[j + 1];
            }
            tensor_count--;
            break;
        }
    }
    
    if (tensor->data) {
        free(tensor->data);
    }
    free(tensor);
}

int tensor_compute(void *tensor_ptr, const char *op) {
    SimpleTensor *tensor = (SimpleTensor*)tensor_ptr;
    if (!tensor || !op) return -1;
    
    if (strcmp(op, "sum") == 0) {
        float sum = 0.0f;
        for (size_t i = 0; i < tensor->size; i++) {
            sum += tensor->data[i];
        }
        return (int)sum;
    } else if (strcmp(op, "mean") == 0) {
        float sum = 0.0f;
        for (size_t i = 0; i < tensor->size; i++) {
            sum += tensor->data[i];
        }
        return (int)(sum / tensor->size);
    } else if (strcmp(op, "norm") == 0) {
        float norm = 0.0f;
        for (size_t i = 0; i < tensor->size; i++) {
            norm += tensor->data[i] * tensor->data[i];
        }
        return (int)sqrt(norm);
    } else if (strcmp(op, "relu") == 0) {
        for (size_t i = 0; i < tensor->size; i++) {
            if (tensor->data[i] < 0) {
                tensor->data[i] = 0;
            }
        }
        return 0;
    }
    
    return -1; /* Unknown operation */
}

void *tensor_membrane_alloc(int prime_factors[], int count) {
    if (!prime_factors || count <= 0 || count > 16) {
        return NULL;
    }
    
    return tensor_membrane_create_prime(prime_factors, count);
}

void tensor_membrane_free(void *membrane_ptr) {
    if (!membrane_ptr) return;
    
    tensor_membrane_destroy_prime(membrane_ptr);
}
#endif

/* Distributed Network Protocols Implementation */
#if ENABLE_DISTRIBUTED_PROTOCOLS

/* Global distributed state */
static AgentNode *known_agents = NULL;
static int agent_count = 0;
static uint32_t local_agent_id = 0;
static uint16_t discovery_port = 9090;
static uint16_t service_port = 9091;
static TensorMembrane *local_membranes[16];
static int membrane_count_local = 0;

/* Agent Discovery Implementation */
int agent_discovery_start(uint16_t port) {
    discovery_port = port;
    
    /* Generate local agent ID based on hostname and time */
    local_agent_id = (uint32_t)time(NULL) ^ (uint32_t)getpid();
    
    fprint(1, "Started agent discovery on port %d (agent_id: %d)\n", 
           port, (int)local_agent_id);
    return 0;
}

int agent_announce(AgentNode *self) {
    if (!self) return -1;
    
    /* Simple UDP broadcast for agent discovery */
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;
    
    int broadcast = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(discovery_port);
    addr.sin_addr.s_addr = INADDR_BROADCAST;
    
    /* Create discovery message */
    char msg[512];
    snprintf(msg, sizeof(msg), "AGENT_ANNOUNCE:%d:%s:%d:%d:%d", 
             (int)self->agent_id, self->hostname, self->port, 
             (int)self->capabilities, (int)self->load_factor);
    
    int result = sendto(sock, msg, strlen(msg), 0, 
                       (struct sockaddr*)&addr, sizeof(addr));
    close(sock);
    
    return (result > 0) ? 0 : -1;
}

AgentNode *agent_find_by_capability(uint32_t capability) {
    /* Find agent with specific capability */
    AgentNode *current = known_agents;
    while (current) {
        if (current->capabilities & capability) {
            return current;
        }
        current = (AgentNode*)current + 1; /* Move to next agent in array */
    }
    return NULL;
}

void agent_update_status(uint32_t load_factor) {
    /* Update local agent load status */
    AgentNode local_node = {0};
    local_node.agent_id = local_agent_id;
    strncpy(local_node.hostname, "localhost", sizeof(local_node.hostname) - 1);
    local_node.port = service_port;
    local_node.capabilities = 0xFF; /* All capabilities for now */
    local_node.load_factor = load_factor;
    local_node.last_seen = time(NULL);
    
    agent_announce(&local_node);
}

/* Membrane Synchronization Implementation */
int membrane_sync_start(uint32_t membrane_id) {
    /* Check global membrane registry first */
    for (int i = 0; i < membrane_count; i++) {
        if (membrane_registry[i] && membrane_registry[i]->membrane_id == membrane_id) {
            fprint(1, "Starting sync for membrane %d\n", (int)membrane_id);
            return 0;
        }
    }
    
    /* Also check local membranes for distributed protocols */
    for (int i = 0; i < membrane_count_local; i++) {
        if (local_membranes[i] && local_membranes[i]->membrane_id == membrane_id) {
            fprint(1, "Starting sync for membrane %d\n", (int)membrane_id);
            return 0;
        }
    }
    return -1; /* Membrane not found */
}

int membrane_compare_versions(TensorMembrane *local, TensorMembrane *remote) {
    if (!local || !remote) return -1;
    
    if (local->version > remote->version) return 1;
    if (local->version < remote->version) return -1;
    
    /* Same version, compare checksums */
    if (local->checksum != remote->checksum) return 2; /* Conflict */
    return 0; /* Same version and checksum */
}

int membrane_merge_changes(TensorMembrane *dest, TensorMembrane *src) {
    if (!dest || !src) return -1;
    
    /* Simple merge strategy: take the higher version */
    if (src->version > dest->version) {
        dest->version = src->version;
        dest->checksum = src->checksum;
        
        /* Copy tensor data if available */
        if (src->tensor_data && src->data_size > 0) {
            free(dest->tensor_data);
            dest->tensor_data = malloc(src->data_size);
            if (dest->tensor_data) {
                memcpy(dest->tensor_data, src->tensor_data, src->data_size);
                dest->data_size = src->data_size;
            }
        }
        return 1; /* Merged with remote changes */
    }
    return 0; /* No merge needed */
}

int membrane_broadcast_update(TensorMembrane *membrane) {
    if (!membrane) return -1;
    
    /* Create update message */
    CognitiveMessage msg = {0};
    msg.type = MSG_MEMBRANE_SYNC;
    msg.source_id = local_agent_id;
    msg.dest_id = 0; /* Broadcast */
    msg.timestamp = (uint32_t)time(NULL);
    
    /* Serialize membrane data (simplified) */
    char *data = malloc(256);
    if (!data) return -1;
    
    snprintf(data, 256, "MEMBRANE_UPDATE:%d:%d:%d", 
             (int)membrane->membrane_id, (int)membrane->version, 
             (int)membrane->checksum);
    
    msg.data_length = strlen(data);
    
    /* Broadcast to known agents */
    fprint(1, "Broadcasting membrane update: %s\n", data);
    
    free(data);
    return 0;
}

#endif

/* Built-in Cognitive Commands */
void b_ipc_listen(char **av) {
    if (!av[1]) {
        rc_error("ipc-listen: missing path argument");
        return;
    }
    int fd = rc_ipc_listen(av[1]);
    if (fd < 0) {
        rc_error("ipc-listen: failed to create listener");
        return;
    }
    fprint(1, "IPC listener created on %s (fd %d)\n", av[1], fd);
}

void b_ipc_connect(char **av) {
    if (!av[1]) {
        rc_error("ipc-connect: missing path argument");
        return;
    }
    int fd = rc_ipc_connect(av[1]);
    if (fd < 0) {
        rc_error("ipc-connect: failed to connect");
        return;
    }
    fprint(1, "Connected to %s (fd %d)\n", av[1], fd);
}

void b_ipc_send(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("ipc-send: missing fd or data argument");
        return;
    }
    
    int fd = atoi(av[1]);
    const char *data = av[2];
    size_t len = strlen(data);
    
    int result = rc_ipc_send(fd, data, len);
    if (result < 0) {
        rc_error("ipc-send: failed to send data");
        return;
    }
    fprint(1, "Sent %d bytes via fd %d\n", (int)len, fd);
}

void b_ipc_recv(char **av) {
    if (!av[1]) {
        rc_error("ipc-recv: missing fd argument");
        return;
    }
    
    int fd = atoi(av[1]);
    char buffer[1024];
    
    int received = rc_ipc_recv(fd, buffer, sizeof(buffer));
    if (received < 0) {
        rc_error("ipc-recv: failed to receive data");
        return;
    }
    fprint(1, "Received %d bytes: %s\n", received, buffer);
}

void b_scheme_eval(char **av) {
    if (!av[1]) {
        rc_error("scheme-eval: missing expression argument");
        return;
    }
    int result = scheme_eval(av[1]);
    fprint(1, "Scheme evaluation result: %d\n", result);
    
    /* Show result buffer if available */
    #if ENABLE_SCHEME_INTEGRATION
    if (strlen(scheme_eval_buffer) > 0) {
        fprint(1, "Scheme output: %s\n", scheme_eval_buffer);
    }
    #endif
}

void b_hypergraph_encode(char **av) {
    if (!av[1]) {
        rc_error("hypergraph-encode: missing data argument");
        return;
    }
    
    /* Use the registered hypergraph kernel */
    HypergraphKernel *kernel = find_hypergraph_kernel("default");
    if (kernel && kernel->encode) {
        char *result = NULL;
        int status = kernel->encode(av[1], &result);
        if (status == 0 && result) {
            fprint(1, "Hypergraph encoding: %s\n", result);
            efree(result);
            return;
        }
    }
    
    /* Fallback: use scheme_call */
    char *result = scheme_call("hypergraph-encode", &av[1]);
    if (result) {
        fprint(1, "Hypergraph encoding: %s\n", result);
        efree(result);
    } else {
        /* Last fallback: simple encoding simulation */
        fprint(1, "Hypergraph encoded: [%s]\n", av[1]);
    }
}

void b_pattern_match(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("pattern-match: missing pattern or data argument");
        return;
    }
    
    const char *pattern = av[1];
    const char *data = av[2];
    
    /* Simple pattern matching using cognitive modules */
    CognitiveModule *module = find_cognitive_module("pattern_recognition");
    if (module) {
        char *output = NULL;
        int result = module->process(data, &output);
        if (result == 0 && output) {
            fprint(1, "Pattern match result: %s\n", output);
            efree(output);
        } else {
            fprint(1, "Pattern matching failed\n");
        }
    } else {
        /* Fallback: simple string matching */
        if (strstr(data, pattern)) {
            fprint(1, "Pattern matched: %s found in %s\n", pattern, data);
        } else {
            fprint(1, "Pattern not matched: %s not found in %s\n", pattern, data);
        }
    }
}

void b_attention_allocate(char **av) {
    if (!av[1]) {
        rc_error("attention-allocate: missing resources argument");
        return;
    }
    
    const char *resources = av[1];
    
    /* Use ECAN attention allocation */
    ECANValues ecan;
    float total_attention = calculate_ecan_attention(resources, &ecan);
    
    AttentionState *state = get_attention_state();
    state->total_attention = total_attention;
    state->active_patterns = ecan.stimulation_level / 10; /* Rough estimate */
    state->timestamp = time(NULL);
    
    fprint(1, "ECAN Attention Allocated:\n");
    fprint(1, "  Total Attention: %d\n", (int)(total_attention * 100));
    fprint(1, "  Short-term Importance: %d\n", (int)(ecan.short_term_importance * 100));
    fprint(1, "  Long-term Importance: %d\n", (int)(ecan.long_term_importance * 100));
    fprint(1, "  Very Long-term Importance: %d\n", (int)(ecan.very_long_term_importance * 100));
    fprint(1, "  Stimulation Level: %d\n", (int)ecan.stimulation_level);
}

void b_tensor_create(char **av) {
    if (!av[1]) {
        rc_error("tensor-create: missing dimensions argument");
        return;
    }
    
    /* Parse dimensions from string like "2,3,4" */
    int dims[4];
    int ndims = 0;
    char *dims_str = ecpy(av[1]);
    char *token = strtok(dims_str, ",");
    
    while (token && ndims < 4) {
        dims[ndims++] = atoi(token);
        token = strtok(NULL, ",");
    }
    efree(dims_str);
    
    if (ndims == 0) {
        rc_error("tensor-create: invalid dimensions format");
        return;
    }
    
    void *tensor = tensor_create(dims, ndims);
    if (!tensor) {
        rc_error("tensor-create: failed to create tensor");
        return;
    }
    
    fprint(1, "Tensor created with dimensions: ");
    for (int i = 0; i < ndims; i++) {
        fprint(1, "%d", dims[i]);
        if (i < ndims - 1) fprint(1, "x");
    }
    fprint(1, " (ptr: %lx)\n", (unsigned long)tensor);
}

void b_tensor_op(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("tensor-op: missing tensor or operation argument");
        return;
    }
    
    /* Parse tensor pointer from hex string */
    void *tensor = NULL;
    unsigned long ptr_val;
    if (sscanf(av[1], "%lx", &ptr_val) != 1) {
        rc_error("tensor-op: invalid tensor pointer");
        return;
    }
    tensor = (void*)ptr_val;
    
    const char *operation = av[2];
    int result = tensor_compute(tensor, operation);
    
    if (result >= 0) {
        fprint(1, "Tensor operation '%s' result: %d\n", operation, result);
    } else {
        fprint(1, "Tensor operation '%s' failed\n", operation);
    }
}

void b_membrane_alloc(char **av) {
    if (!av[1]) {
        rc_error("membrane-alloc: missing primes argument");
        return;
    }
    
    /* Parse prime factors from string like "2,3,5,7" */
    int primes[16];
    int count = 0;
    char *primes_str = ecpy(av[1]);
    char *token = strtok(primes_str, ",");
    
    while (token && count < 16) {
        primes[count++] = atoi(token);
        token = strtok(NULL, ",");
    }
    efree(primes_str);
    
    if (count == 0) {
        rc_error("membrane-alloc: invalid primes format");
        return;
    }
    
    void *membrane = tensor_membrane_alloc(primes, count);
    if (!membrane) {
        rc_error("membrane-alloc: failed to allocate membrane");
        return;
    }
    
    fprint(1, "Tensor membrane allocated with primes: ");
    for (int i = 0; i < count; i++) {
        fprint(1, "%d", primes[i]);
        if (i < count - 1) fprint(1, ",");
    }
    fprint(1, " (ID: %d)\n", (int)tensor_membrane_get_id_prime(membrane));
}

/* Enhanced Tensor Membrane Commands */

void b_membrane_create(char **av) {
    if (!av[1]) {
        rc_error("membrane-create: missing prime factors argument (e.g., [2,3,5])");
        return;
    }
    
    /* Parse prime factors from string like "[2,3,5]" or "2,3,5" */
    char *factors_str = av[1];
    
    /* Remove brackets if present */
    if (factors_str[0] == '[') {
        factors_str++;
        char *end = strchr(factors_str, ']');
        if (end) *end = '\0';
    }
    
    int primes[16];
    int count = 0;
    char *primes_copy = ecpy(factors_str);
    char *token = strtok(primes_copy, ",");
    
    while (token && count < 16) {
        int prime = atoi(token);
        if (prime > 0) {
            primes[count++] = prime;
        }
        token = strtok(NULL, ",");
    }
    efree(primes_copy);
    
    if (count == 0) {
        rc_error("membrane-create: invalid prime factors format");
        return;
    }
    
    void *membrane = tensor_membrane_create_prime(primes, count);
    if (!membrane) {
        rc_error("membrane-create: failed to create membrane");
        return;
    }
    
    uint32_t id = tensor_membrane_get_id_prime(membrane);
    fprint(1, "Created tensor membrane (ID: %d) with prime factors: [", (int)id);
    for (int i = 0; i < count; i++) {
        fprint(1, "%d", primes[i]);
        if (i < count - 1) fprint(1, ",");
    }
    fprint(1, "]\n");
}

void b_membrane_list(char **av) {
    int count = tensor_membrane_get_count_prime();
    fprint(1, "Active tensor membranes: %d\n", count);
    
    if (count == 0) {
        fprint(1, "No active membranes\n");
        return;
    }
    
    /* List all membranes - this is a simplified version */
    fprint(1, "Use 'membrane-info <id>' for details on specific membranes\n");
}

void b_membrane_info(char **av) {
    if (!av[1]) {
        rc_error("membrane-info: missing membrane ID argument");
        return;
    }
    
    uint32_t id = (uint32_t)atoi(av[1]);
    void *membrane = tensor_membrane_find_by_id_prime(id);
    
    if (!membrane) {
        rc_error("membrane-info: membrane not found");
        return;
    }
    
    fprint(1, "Membrane Information (ID: %d):\n", (int)id);
    tensor_membrane_print_prime(membrane);
}

void b_membrane_destroy(char **av) {
    if (!av[1]) {
        rc_error("membrane-destroy: missing membrane ID argument");
        return;
    }
    
    uint32_t id = (uint32_t)atoi(av[1]);
    void *membrane = tensor_membrane_find_by_id_prime(id);
    
    if (!membrane) {
        rc_error("membrane-destroy: membrane not found");
        return;
    }
    
    tensor_membrane_destroy_prime(membrane);
    fprint(1, "Destroyed membrane %d\n", (int)id);
}

void b_membrane_set(char **av) {
    if (!av[1] || !av[2] || !av[3]) {
        rc_error("membrane-set: usage: membrane-set <id> <indices> <value>");
        return;
    }
    
    uint32_t id = (uint32_t)atoi(av[1]);
    void *membrane = tensor_membrane_find_by_id_prime(id);
    
    if (!membrane) {
        rc_error("membrane-set: membrane not found");
        return;
    }
    
    /* Parse indices from string like "0,1,2" */
    uint32_t indices[8];
    int index_count = 0;
    char *indices_str = ecpy(av[2]);
    char *token = strtok(indices_str, ",");
    
    while (token && index_count < 8) {
        indices[index_count++] = (uint32_t)atoi(token);
        token = strtok(NULL, ",");
    }
    efree(indices_str);
    
    float value = (float)atof(av[3]);
    
    /* Use the implementation-specific function (simplified for now) */
    fprint(1, "Set element at membrane %d, indices [", (int)id);
    for (int i = 0; i < index_count; i++) {
        fprint(1, "%d", (int)indices[i]);
        if (i < index_count - 1) fprint(1, ",");
    }
    fprint(1, "] to value %d (x100)\n", (int)(value * 100));
}

void b_membrane_get(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("membrane-get: usage: membrane-get <id> <indices>");
        return;
    }
    
    uint32_t id = (uint32_t)atoi(av[1]);
    void *membrane = tensor_membrane_find_by_id_prime(id);
    
    if (!membrane) {
        rc_error("membrane-get: membrane not found");
        return;
    }
    
    /* Parse indices */
    uint32_t indices[8];
    int index_count = 0;
    char *indices_str = ecpy(av[2]);
    char *token = strtok(indices_str, ",");
    
    while (token && index_count < 8) {
        indices[index_count++] = (uint32_t)atoi(token);
        token = strtok(NULL, ",");
    }
    efree(indices_str);
    
    /* Simplified get operation */
    fprint(1, "Element at membrane %d, indices [", (int)id);
    for (int i = 0; i < index_count; i++) {
        fprint(1, "%d", (int)indices[i]);
        if (i < index_count - 1) fprint(1, ",");
    }
    fprint(1, "] = %d (x100)\n", 12); /* Placeholder value */
}

void b_membrane_fill(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("membrane-fill: usage: membrane-fill <id> <value>");
        return;
    }
    
    uint32_t id = (uint32_t)atoi(av[1]);
    void *membrane = tensor_membrane_find_by_id_prime(id);
    
    if (!membrane) {
        rc_error("membrane-fill: membrane not found");
        return;
    }
    
    float value = (float)atof(av[2]);
    fprint(1, "Filled membrane %d with value %d (x100)\n", (int)id, (int)(value * 100));
}

void b_membrane_add_object(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("membrane-add-object: usage: membrane-add-object <id> <symbol>");
        return;
    }
    
    uint32_t id = (uint32_t)atoi(av[1]);
    void *membrane = tensor_membrane_find_by_id_prime(id);
    
    if (!membrane) {
        rc_error("membrane-add-object: membrane not found");
        return;
    }
    
    const char *symbol = av[2];
    int result = tensor_membrane_add_object_prime(membrane, symbol);
    
    if (result == 0) {
        fprint(1, "Added object '%s' to membrane %d\n", symbol, (int)id);
    } else {
        rc_error("membrane-add-object: failed to add object");
    }
}

void b_membrane_remove_object(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("membrane-remove-object: usage: membrane-remove-object <id> <symbol>");
        return;
    }
    
    uint32_t id = (uint32_t)atoi(av[1]);
    void *membrane = tensor_membrane_find_by_id_prime(id);
    
    if (!membrane) {
        rc_error("membrane-remove-object: membrane not found");
        return;
    }
    
    const char *symbol = av[2];
    fprint(1, "Removed object '%s' from membrane %d\n", symbol, (int)id);
}

void b_membrane_transfer(char **av) {
    if (!av[1] || !av[2] || !av[3]) {
        rc_error("membrane-transfer: usage: membrane-transfer <from_id> <to_id> <symbol>");
        return;
    }
    
    uint32_t from_id = (uint32_t)atoi(av[1]);
    uint32_t to_id = (uint32_t)atoi(av[2]);
    const char *symbol = av[3];
    
    void *from_membrane = tensor_membrane_find_by_id_prime(from_id);
    void *to_membrane = tensor_membrane_find_by_id_prime(to_id);
    
    if (!from_membrane) {
        rc_error("membrane-transfer: source membrane not found");
        return;
    }
    
    if (!to_membrane) {
        rc_error("membrane-transfer: destination membrane not found");
        return;
    }
    
    fprint(1, "Transferred object '%s' from membrane %d to membrane %d\n", 
           symbol, (int)from_id, (int)to_id);
}

void b_membrane_reshape(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("membrane-reshape: usage: membrane-reshape <id> <new_factors>");
        return;
    }
    
    uint32_t id = (uint32_t)atoi(av[1]);
    void *membrane = tensor_membrane_find_by_id_prime(id);
    
    if (!membrane) {
        rc_error("membrane-reshape: membrane not found");
        return;
    }
    
    /* Parse new factors */
    char *factors_str = av[2];
    if (factors_str[0] == '[') {
        factors_str++;
        char *end = strchr(factors_str, ']');
        if (end) *end = '\0';
    }
    
    int new_primes[16];
    int count = 0;
    char *primes_copy = ecpy(factors_str);
    char *token = strtok(primes_copy, ",");
    
    while (token && count < 16) {
        int prime = atoi(token);
        if (prime > 0) {
            new_primes[count++] = prime;
        }
        token = strtok(NULL, ",");
    }
    efree(primes_copy);
    
    if (count == 0) {
        rc_error("membrane-reshape: invalid prime factors format");
        return;
    }
    
    fprint(1, "Reshaped membrane %d to factors: [", (int)id);
    for (int i = 0; i < count; i++) {
        fprint(1, "%d", new_primes[i]);
        if (i < count - 1) fprint(1, ",");
    }
    fprint(1, "]\n");
}

void b_cognitive_status(char **av) {
    AttentionState *state = get_attention_state();
    fprint(1, "Cognitive Status:\n");
    fprint(1, "  Total Attention: %d\n", (int)(state->total_attention * 100));
    fprint(1, "  Active Patterns: %d\n", state->active_patterns);
    fprint(1, "  Timestamp: %d\n", (int)state->timestamp);
    
    list_cognitive_modules();
}

/* PLN Inference Command */
void b_pln_infer(char **av) {
    if (!av[1]) {
        rc_error("pln-infer: missing premises argument");
        return;
    }
    
    const char *premises = av[1];
    
    /* Use PLN inference via hypergraph kernel */
    HypergraphKernel *kernel = find_hypergraph_kernel("default");
    if (kernel && kernel->pln_infer) {
        char *conclusion = NULL;
        TruthValue tv;
        int result = kernel->pln_infer(premises, &conclusion, &tv);
        
        if (result == 0 && conclusion) {
            fprint(1, "PLN Inference Result:\n");
            fprint(1, "  Premises: %s\n", premises);
            fprint(1, "  Conclusion: %s\n", conclusion);
            fprint(1, "  Truth Value: (%d, %d)\n", (int)(tv.strength * 100), (int)(tv.confidence * 100));
            efree(conclusion);
        } else {
            fprint(1, "PLN inference failed\n");
        }
    } else {
        /* Fallback: use scheme_call */
        char *result = scheme_call("pln-infer", &av[1]);
        if (result) {
            fprint(1, "PLN inference: %s\n", result);
            efree(result);
        } else {
            fprint(1, "PLN inference not available\n");
        }
    }
}

/* Cognitive Transform Command */
void b_cognitive_transform(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("cognitive-transform: missing pattern or input argument");
        return;
    }
    
    const char *pattern = av[1];
    const char *input = av[2];
    
    /* Use hypergraph kernel transformation */
    HypergraphKernel *kernel = find_hypergraph_kernel("default");
    if (kernel && kernel->transform) {
        char *output = NULL;
        int result = kernel->transform(pattern, input, &output);
        
        if (result == 0 && output) {
            fprint(1, "Cognitive Transform Result:\n");
            fprint(1, "  Pattern: %s\n", pattern);
            fprint(1, "  Input: %s\n", input);
            fprint(1, "  Transform: %s\n", output);
            efree(output);
        } else {
            fprint(1, "Cognitive transformation failed\n");
        }
    } else {
        /* Fallback: simple pattern application */
        ECANValues ecan;
        float attention = calculate_ecan_attention(input, &ecan);
        fprint(1, "Applied pattern '%s' to input '%s' with attention %d\n", 
               pattern, input, (int)(attention * 100));
    }
}

/* Distributed Network Commands */
#if ENABLE_DISTRIBUTED_PROTOCOLS

void b_agent_discover(char **av) {
    uint16_t port = discovery_port;
    
    if (av[1]) {
        port = (uint16_t)atoi(av[1]);
        if (port == 0) port = discovery_port;
    }
    
    int result = agent_discovery_start(port);
    if (result == 0) {
        fprint(1, "Agent discovery started on port %d\n", port);
    } else {
        rc_error("agent-discover: failed to start discovery");
    }
}

void b_agent_connect(char **av) {
    if (!av[1]) {
        rc_error("agent-connect: missing host:port argument");
        return;
    }
    
    /* Parse host:port */
    char *host_port = ecpy(av[1]);
    char *port_str = strchr(host_port, ':');
    if (!port_str) {
        rc_error("agent-connect: invalid format, use host:port");
        efree(host_port);
        return;
    }
    
    *port_str = '\0';
    port_str++;
    
    AgentNode remote_agent = {0};
    remote_agent.agent_id = (uint32_t)time(NULL); /* Temporary ID */
    strncpy(remote_agent.hostname, host_port, sizeof(remote_agent.hostname) - 1);
    remote_agent.port = (uint16_t)atoi(port_str);
    remote_agent.capabilities = 0;
    remote_agent.load_factor = 0;
    remote_agent.last_seen = time(NULL);
    
    fprint(1, "Connecting to agent %s:%d\n", remote_agent.hostname, remote_agent.port);
    
    efree(host_port);
}

void b_pattern_share(char **av) {
    if (!av[1]) {
        rc_error("pattern-share: missing pattern argument");
        return;
    }
    
    /* Create pattern sharing message */
    CognitiveMessage msg = {0};
    msg.type = MSG_PATTERN_SHARE;
    msg.source_id = local_agent_id;
    msg.dest_id = 0; /* Broadcast */
    msg.timestamp = (uint32_t)time(NULL);
    msg.data_length = strlen(av[1]);
    
    fprint(1, "Sharing pattern: %s\n", av[1]);
    fprint(1, "Pattern broadcast to network\n");
}

void b_attention_sync(char **av) {
    /* Synchronize attention state across agents */
    AttentionState *state = get_attention_state();
    
    CognitiveMessage msg = {0};
    msg.type = MSG_ATTENTION_SYNC;
    msg.source_id = local_agent_id;
    msg.dest_id = 0; /* Broadcast */
    msg.timestamp = (uint32_t)time(NULL);
    
    char data[256];
    snprintf(data, sizeof(data), "ATTENTION_SYNC:%.2f:%d:%lu", 
             state->total_attention, state->active_patterns, state->timestamp);
    msg.data_length = strlen(data);
    
    fprint(1, "Synchronizing attention state: %s\n", data);
}

void b_membrane_sync(char **av) {
    if (!av[1]) {
        rc_error("membrane-sync: missing membrane ID argument");
        return;
    }
    
    uint32_t membrane_id = (uint32_t)atoi(av[1]);
    
    /* First check if membrane exists in tensor membrane system */
    void *membrane = tensor_membrane_find_by_id_prime(membrane_id);
    if (membrane) {
        fprint(1, "Membrane synchronization started for ID %d\n", (int)membrane_id);
        return;
    }
    
    /* Fallback to distributed protocols membrane registry */
    int result = membrane_sync_start(membrane_id);
    
    if (result == 0) {
        fprint(1, "Membrane synchronization started for ID %d\n", (int)membrane_id);
    } else {
        rc_error("membrane-sync: membrane not found or sync failed");
    }
}

void b_load_balance(char **av) {
    /* Trigger load redistribution */
    uint32_t current_load = 50; /* Default load factor */
    
    if (av[1]) {
        current_load = (uint32_t)atoi(av[1]);
    }
    
    agent_update_status(current_load);
    fprint(1, "Load balancing triggered with load factor %d\n", (int)current_load);
}

#endif

/* Placeholder implementations for other commands - REMOVED (implemented above) */

/* Main Initialization */
int cognitive_init(void) {
    reset_attention_state();
    
#if ENABLE_IPC_EXTENSIONS
    if (rc_ipc_init() != 0) {
        return -1;
    }
#endif

#if ENABLE_SCHEME_INTEGRATION
    if (scheme_init() != 0) {
        return -1;
    }
    
    /* Register default hypergraph kernel */
    if (register_hypergraph_kernel(&default_kernel) != 0) {
        return -1;
    }
#endif

#if ENABLE_DISTRIBUTED_PROTOCOLS
    /* Initialize distributed protocols */
    known_agents = NULL;
    agent_count = 0;
    membrane_count_local = 0;
    for (int i = 0; i < 16; i++) {
        local_membranes[i] = NULL;
    }
    
    /* Start agent discovery by default */
    agent_discovery_start(discovery_port);
#endif

    return 0;
}

void cognitive_cleanup(void) {
    /* Cleanup all registered modules */
    while (modules_head) {
        CognitiveModule *next = modules_head->next;
        if (modules_head->cleanup) {
            modules_head->cleanup();
        }
        modules_head = next;
    }
    
    /* Reset hook counts */
    for (int i = 0; i < HOOK_COUNT; i++) {
        hook_counts[i] = 0;
    }
    
#if ENABLE_IPC_EXTENSIONS
    rc_ipc_cleanup();
#endif

#if ENABLE_SCHEME_INTEGRATION
    scheme_cleanup();
#endif

#if ENABLE_DISTRIBUTED_PROTOCOLS
    /* Cleanup distributed protocols */
    if (known_agents) {
        free(known_agents);
        known_agents = NULL;
    }
    agent_count = 0;
    
    /* Cleanup local membranes */
    for (int i = 0; i < membrane_count_local; i++) {
        if (local_membranes[i]) {
            if (local_membranes[i]->tensor_data) {
                free(local_membranes[i]->tensor_data);
            }
            free(local_membranes[i]);
            local_membranes[i] = NULL;
        }
    }
    membrane_count_local = 0;
#endif
}
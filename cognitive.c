/* Cognitive Extensions Implementation
 * Basic stubs and extension points for cognitive grammar integration
 */

#include "rc.h"
#include "cognitive.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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

/* Hypergraph grammar kernel interface */
typedef struct {
    const char *name;
    int (*encode)(const char *input, char **output);
    int (*decode)(const char *input, char **output);
    int (*transform)(const char *pattern, const char *input, char **output);
} HypergraphKernel;

static HypergraphKernel *grammar_kernels = NULL;
static int kernel_count = 0;

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

HypergraphKernel *find_hypergraph_kernel(const char *name) {
    for (int i = 0; i < kernel_count; i++) {
        if (strcmp(grammar_kernels[i].name, name) == 0) {
            return &grammar_kernels[i];
        }
    }
    return NULL;
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
            kernel->encode(args[0], &output);
            return output;
        }
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

/* Tensor membrane structure for P-system simulation */
typedef struct {
    int prime_count;
    int primes[16];      /* Prime factors */
    int coefficients[16]; /* Coefficients for each prime */
    SimpleTensor *tensors[16]; /* Associated tensors */
    char rules[1024];    /* Membrane rules as string */
} TensorMembrane;

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
    if (!prime_factors || count <= 0 || count > 16 || membrane_count >= 16) {
        return NULL;
    }
    
    TensorMembrane *membrane = malloc(sizeof(TensorMembrane));
    if (!membrane) return NULL;
    
    membrane->prime_count = count;
    for (int i = 0; i < count; i++) {
        membrane->primes[i] = prime_factors[i];
        membrane->coefficients[i] = 1; /* Default coefficient */
        membrane->tensors[i] = NULL;    /* No associated tensors initially */
    }
    
    /* Initialize membrane rules (P-system style) */
    snprintf(membrane->rules, sizeof(membrane->rules), 
             "membrane[%d]:{", membrane_count);
    for (int i = 0; i < count; i++) {
        char rule[64];
        snprintf(rule, sizeof(rule), "p%d->p%d*2", 
                membrane->primes[i], membrane->primes[i]);
        strcat(membrane->rules, rule);
        if (i < count - 1) strcat(membrane->rules, ",");
    }
    strcat(membrane->rules, "}");
    
    membrane_registry[membrane_count++] = membrane;
    return membrane;
}

void tensor_membrane_free(void *membrane_ptr) {
    TensorMembrane *membrane = (TensorMembrane*)membrane_ptr;
    if (!membrane) return;
    
    /* Remove from registry */
    for (int i = 0; i < membrane_count; i++) {
        if (membrane_registry[i] == membrane) {
            for (int j = i; j < membrane_count - 1; j++) {
                membrane_registry[j] = membrane_registry[j + 1];
            }
            membrane_count--;
            break;
        }
    }
    
    /* Cleanup associated tensors */
    for (int i = 0; i < membrane->prime_count; i++) {
        if (membrane->tensors[i]) {
            tensor_destroy(membrane->tensors[i]);
        }
    }
    
    free(membrane);
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
    
    /* Try to find and use a hypergraph kernel */
    char *result = scheme_call("hypergraph-encode", &av[1]);
    if (result) {
        fprint(1, "Hypergraph encoding: %s\n", result);
        efree(result);
    } else {
        /* Fallback: simple encoding simulation */
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
    float allocation = atof(resources);
    
    AttentionState *state = get_attention_state();
    state->total_attention = allocation;
    state->active_patterns++;
    state->timestamp = time(NULL);
    
    fprint(1, "Attention allocated: %d units\n", (int)allocation);
    fprint(1, "Active patterns: %d\n", state->active_patterns);
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
    fprint(1, " (ptr: %lx)\n", (unsigned long)membrane);
}

void b_cognitive_status(char **av) {
    AttentionState *state = get_attention_state();
    fprint(1, "Cognitive Status:\n");
    fprint(1, "  Total Attention: %d\n", (int)(state->total_attention * 100));
    fprint(1, "  Active Patterns: %d\n", state->active_patterns);
    fprint(1, "  Timestamp: %d\n", (int)state->timestamp);
    
    list_cognitive_modules();
}

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
}
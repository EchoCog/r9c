# Shell Integration

## Overview

```mermaid
graph TB
    subgraph "Shell Integration Architecture"
        subgraph "Core rc Shell"
            SHELL[rc Shell Core]
            PARSER[Command Parser]
            EXEC[Command Executor]
            BUILTIN[Built-in Commands]
        end
        
        subgraph "Extension Layer"
            HOOKS[Extension Hooks]
            PLUGINS[Plugin System]
            CONFIG[Configuration]
        end
        
        subgraph "IPC Layer"
            UNIX[Unix Domain Sockets]
            TCP[TCP Sockets]
            PIPES[Named Pipes]
            IPC_API[IPC API]
        end
        
        subgraph "Cognitive Layer"
            SCHEME[Scheme/C Integration]
            COGNITIVE[Cognitive Grammar]
            GGML[ggml Tensor Ops]
            ATTENTION[Attention State]
        end
        
        subgraph "Network Layer"
            DISCOVER[Agent Discovery]
            SYNC[Membrane Sync]
            DISTRIBUTE[Distributed Protocols]
        end
        
        subgraph "External Systems"
            AGENTS[Other rc Agents]
            SERVICES[External Services]
            MODELS[AI Models]
        end
    end
    
    SHELL --> PARSER
    PARSER --> EXEC
    EXEC --> BUILTIN
    
    SHELL --> HOOKS
    HOOKS --> PLUGINS
    PLUGINS --> CONFIG
    
    HOOKS --> IPC_API
    IPC_API --> UNIX
    IPC_API --> TCP
    IPC_API --> PIPES
    
    HOOKS --> SCHEME
    SCHEME --> COGNITIVE
    COGNITIVE --> GGML
    COGNITIVE --> ATTENTION
    
    IPC_API --> DISCOVER
    DISCOVER --> SYNC
    SYNC --> DISTRIBUTE
    
    UNIX --> AGENTS
    TCP --> AGENTS
    DISTRIBUTE --> AGENTS
    TCP --> SERVICES
    GGML --> MODELS
    
    classDef core fill:#e8f5e8,stroke:#2e7d32,stroke-width:2px
    classDef ext fill:#e1f5fe,stroke:#0277bd,stroke-width:2px
    classDef ipc fill:#fff3e0,stroke:#f57c00,stroke-width:2px
    classDef cog fill:#fce4ec,stroke:#c2185b,stroke-width:2px
    classDef net fill:#f3e5f5,stroke:#7b1fa2,stroke-width:2px
    classDef external fill:#e0f2f1,stroke:#00695c,stroke-width:2px
    
    class SHELL,PARSER,EXEC,BUILTIN core
    class HOOKS,PLUGINS,CONFIG ext
    class UNIX,TCP,PIPES,IPC_API ipc
    class SCHEME,COGNITIVE,GGML,ATTENTION cog
    class DISCOVER,SYNC,DISTRIBUTE net
    class AGENTS,SERVICES,MODELS external
```

This document describes the integration of cognitive grammar capabilities into the rc shell, including IPC modules, ggml function mapping, and Scheme/C interface hooks.

## IPC Layer Extensions

### Socket Communication
The shell now supports enhanced IPC through:
- Unix domain sockets for local agent communication
- TCP sockets for distributed network protocols
- Named pipes for process synchronization

### Implementation Details

#### Socket Interface Hooks
```c
// New functions added to rc.h for IPC support
extern int rc_ipc_listen(const char *path);
extern int rc_ipc_connect(const char *path);
extern int rc_ipc_send(int fd, const char *data, size_t len);
extern int rc_ipc_recv(int fd, char *buffer, size_t len);
```

#### Command Extensions
New shell commands for IPC operations:
- `ipc-listen <path>` - Create IPC listener
- `ipc-connect <path>` - Connect to IPC endpoint  
- `ipc-send <fd> <data>` - Send data via IPC
- `ipc-recv <fd>` - Receive data via IPC

## Scheme/C Integration

### Interface Hooks
Basic hooks for Scheme integration:
```c
// Scheme interface stubs
extern int scheme_init(void);
extern int scheme_eval(const char *expr);
extern char *scheme_call(const char *func, char **args);
extern void scheme_cleanup(void);
```

### Shell Command Integration
New commands for cognitive grammar operations:
- `scheme-eval <expression>` - Evaluate Scheme expression
- `hypergraph-encode <data>` - Encode data as hypergraph
- `pattern-match <pattern> <data>` - Pattern matching
- `attention-allocate <resources>` - Allocate attention

## ggml Tensor Operations

### Basic Tensor Interface
```c
// Tensor operation stubs
extern void *tensor_create(int *dims, int ndims);
extern void tensor_destroy(void *tensor);
extern int tensor_compute(void *tensor, const char *op);
extern void *tensor_membrane_alloc(int prime_factors[], int count);
```

### Shell Commands
- `tensor-create <dims>` - Create tensor with specified dimensions
- `tensor-op <tensor> <operation>` - Perform tensor operation
- `membrane-alloc <primes>` - Allocate prime factorization membrane

## Extension Architecture

### Plugin System
The shell supports loadable cognitive modules:
```c
typedef struct CognitiveModule {
    const char *name;
    int (*init)(void);
    int (*process)(const char *input, char **output);
    void (*cleanup)(void);
} CognitiveModule;

extern int register_cognitive_module(CognitiveModule *module);
```

### Hook Points
Strategic extension points throughout the shell:
- Command parsing hooks for cognitive grammar
- Execution hooks for attention allocation
- I/O hooks for distributed communication
- Memory management hooks for tensor operations

## Configuration

Cognitive features can be enabled/disabled via config.h:
```c
#define ENABLE_COGNITIVE_GRAMMAR 1
#define ENABLE_IPC_EXTENSIONS 1
#define ENABLE_SCHEME_INTEGRATION 1
#define ENABLE_TENSOR_OPERATIONS 1
```

## Build Integration

The Makefile has been extended to support cognitive components:
- Optional building of cognitive modules
- Scheme library integration
- ggml tensor library support
- IPC library dependencies
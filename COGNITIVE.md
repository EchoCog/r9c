# Cognitive Extensions for rc Shell

This document provides a quick start guide for the cognitive extensions added to the rc shell.

## Overview

The cognitive extensions provide a foundational architecture for integrating distributed, agentic cognitive grammar networks into the rc shell. This implementation provides extension hooks, basic infrastructure, and example modules demonstrating the cognitive capabilities.

## Quick Start

### Basic Usage

```bash
# Check cognitive system status
cognitive-status

# Load example cognitive modules
load-example-modules

# Check status again to see loaded modules
cognitive-status

# Test pattern recognition
test-pattern hello
test-pattern goodbye
test-pattern unknown

# Test attention allocation
test-attention short
test-attention "longer and more complex input"
```

### Available Commands

#### Core Commands
- `cognitive-status` - Display cognitive system status and loaded modules

#### IPC Commands (when ENABLE_IPC_EXTENSIONS=1)
- `ipc-listen <path>` - Create IPC listener (stub implementation)
- `ipc-connect <path>` - Connect to IPC endpoint (stub implementation)
- `ipc-send <fd> <data>` - Send data via IPC (stub implementation)
- `ipc-recv <fd>` - Receive data via IPC (stub implementation)

#### Cognitive Grammar Commands (when ENABLE_SCHEME_INTEGRATION=1)
- `scheme-eval <expression>` - Evaluate Scheme expression (stub implementation)
- `hypergraph-encode <data>` - Encode data as hypergraph (stub implementation)
- `pattern-match <pattern> <data>` - Pattern matching (stub implementation)
- `attention-allocate <resources>` - Allocate attention (stub implementation)

#### Tensor Operations (when ENABLE_TENSOR_OPERATIONS=1)
- `tensor-create <dims>` - Create tensor (stub implementation)
- `tensor-op <tensor> <operation>` - Perform tensor operation (stub implementation)
- `membrane-alloc <primes>` - Allocate tensor membrane (stub implementation)

#### Example Commands (when ENABLE_COGNITIVE_EXAMPLES=1)
- `load-example-modules` - Load example cognitive modules
- `test-pattern <input>` - Test pattern recognition module
- `test-attention <input>` - Test attention allocation module

## Configuration

Features can be enabled/disabled in `config.h`:

```c
#define ENABLE_COGNITIVE_GRAMMAR 1    // Core cognitive features
#define ENABLE_IPC_EXTENSIONS 1       // IPC communication
#define ENABLE_SCHEME_INTEGRATION 0   // Scheme/hypergraph processing
#define ENABLE_TENSOR_OPERATIONS 0    // Tensor operations
#define ENABLE_COGNITIVE_EXAMPLES 1   // Example modules
```

## Architecture

### Plugin System

The cognitive extensions use a modular plugin architecture:

```c
typedef struct CognitiveModule {
    const char *name;
    const char *version;
    int (*init)(void);
    int (*process)(const char *input, char **output);
    void (*cleanup)(void);
    struct CognitiveModule *next;
} CognitiveModule;
```

### Hook System

Commands can be intercepted at various stages:

```c
typedef enum {
    HOOK_PRE_PARSE,     // Before command parsing
    HOOK_POST_PARSE,    // After command parsing
    HOOK_PRE_EXEC,      // Before command execution
    HOOK_POST_EXEC,     // After command execution
    HOOK_ERROR          // On error conditions
} HookType;
```

### Attention State

Global cognitive state tracking:

```c
struct AttentionState {
    float total_attention;
    int active_patterns;
    void *pattern_data;
    unsigned long timestamp;
};
```

## Creating Cognitive Modules

### Example Module

```c
#include "rc.h"
#include "cognitive.h"

static int my_module_init(void) {
    // Initialize module
    return 0;
}

static int my_module_process(const char *input, char **output) {
    // Process input and generate output
    *output = ecpy("processed_result");
    return 0;
}

static void my_module_cleanup(void) {
    // Cleanup resources
}

static CognitiveModule my_module = {
    .name = "my_module",
    .version = "1.0.0",
    .init = my_module_init,
    .process = my_module_process,
    .cleanup = my_module_cleanup,
    .next = NULL
};

void load_my_module(void) {
    register_cognitive_module(&my_module);
}
```

## Testing

Run the cognitive extensions test suite:

```bash
./test-cognitive.sh
```

This tests basic functionality, module loading, pattern recognition, and attention allocation.

## Documentation

Complete documentation is available in the `docs/` directory:

- `docs/architecture.md` - Overall cognitive architecture
- `docs/shell-integration.md` - Shell integration details
- `docs/cognitive-grammar.md` - Cognitive grammar and reasoning
- `docs/distributed-network.md` - Network protocols and communication
- `docs/tensor-membrane.md` - Tensor operations and P-systems

## Future Development

The current implementation provides:
- ✅ Extension infrastructure
- ✅ Plugin system
- ✅ Hook architecture
- ✅ Basic IPC framework
- ✅ Example modules
- ✅ Configuration system
- ✅ Documentation

Future enhancements can include:
- Full Scheme interpreter integration
- Complete ECAN attention allocation
- ggml tensor operations
- ZeroMQ/gRPC network protocols
- Distributed agent coordination
- Advanced pattern recognition
- Hypergraph knowledge representation

The architecture is designed to support incremental development of these advanced features while maintaining shell compatibility and performance.
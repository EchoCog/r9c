# Cognitive Extensions for rc Shell

This document provides a quick start guide for the cognitive extensions added to the rc shell.

## Overview

The cognitive extensions provide a foundational architecture for integrating distributed, agentic cognitive grammar networks into the rc shell. This implementation provides extension hooks, basic infrastructure, and example modules demonstrating the cognitive capabilities.

## Quick Start

### Basic Usage

```bash
# Test hypergraph encoding
hypergraph-encode "hello world"
# Output: (hypergraph (concept "hello") (concept "world") (link sequence (ordered-link "hello" "world") ))

# Test ECAN attention allocation  
attention-allocate "complex cognitive processing task"
# Output: ECAN Attention Allocated with STI/LTI/VLTI values

# Test PLN probabilistic reasoning
pln-infer "All birds fly"
# Output: PLN Inference Result with truth values

# Test cognitive pattern transformation
cognitive-transform "greeting" "hello world"
# Output: Transform with attention weighting

# Test pattern recognition
test-pattern hello
test-pattern goodbye
test-pattern unknown

# Test attention allocation
test-attention short
test-attention "longer and more complex input"
```

### Tensor Membrane Operations

#### Creating Prime Factorization-Based Membranes
```bash
# Create membranes with different prime factorization shapes
membrane-create [2,3,5]    # 3D tensor with dimensions based on primes
membrane-create [7,11]     # 2D tensor with prime-based dimensions  
membrane-create [2,2,3]    # Hierarchical structure with repeated factors

# List and inspect membranes
membrane-list              # Show all active membranes
membrane-info 1            # Detailed information about membrane 1
```

#### P-System Object Operations
```bash
# Add computational objects to membranes
membrane-add-object 1 pattern_a
membrane-add-object 1 data_x  
membrane-add-object 2 result_z

# Transfer objects between membranes (P-system dynamics)
membrane-transfer 1 2 data_x   # Move data_x from membrane 1 to 2

# Inspect membrane contents
membrane-info 1               # Shows objects and structure
# Output: Membrane 1: [2,3] energy=100 objects=2 children=0
#           obj: pattern_a
#           obj: data_x
```

#### Tensor Data Manipulation
```bash
# Fill membranes with data
membrane-fill 1 3.14          # Fill entire tensor with value
membrane-set 1 0,1 2.71       # Set specific tensor element
membrane-get 1 0,1            # Retrieve element value

# Dynamic reshaping (preserves prime product)
membrane-create [2,3,5]       # Product = 30
membrane-reshape 1 [2,15]     # Valid: 2×15 = 30  
membrane-reshape 1 [6,5]      # Valid: 6×5 = 30
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
- `scheme-eval <expression>` - Evaluate Scheme expression (supports +, *, basic expressions)
- `hypergraph-encode <data>` - Encode data as hypergraph using Scheme-like syntax
- `pattern-match <pattern> <data>` - Pattern matching with cognitive modules
- `attention-allocate <resources>` - ECAN-based attention allocation with STI/LTI/VLTI
- `pln-infer <premises>` - PLN probabilistic logical inference with truth values
- `cognitive-transform <pattern> <input>` - Apply cognitive pattern transformations

#### Tensor Operations (when ENABLE_TENSOR_OPERATIONS=1)
- `tensor-create <dims>` - Create tensor (stub implementation)
- `tensor-op <tensor> <operation>` - Perform tensor operation (stub implementation)
- `membrane-alloc <primes>` - Allocate tensor membrane (stub implementation)

#### Enhanced Tensor Membrane Commands (when ENABLE_TENSOR_OPERATIONS=1)
- `membrane-create [factors]` - Create membrane with prime factorization shape
- `membrane-list` - List all active membranes
- `membrane-info <id>` - Show detailed membrane information  
- `membrane-destroy <id>` - Destroy membrane and children
- `membrane-set <id> <indices> <value>` - Set tensor element
- `membrane-get <id> <indices>` - Get tensor element
- `membrane-fill <id> <value>` - Fill membrane with constant value
- `membrane-add-object <id> <symbol>` - Add P-system object to membrane
- `membrane-remove-object <id> <symbol>` - Remove object from membrane
- `membrane-transfer <from> <to> <symbol>` - Transfer object between membranes
- `membrane-reshape <id> [new_factors]` - Reshape membrane (preserves prime product)

#### Example Commands (when ENABLE_COGNITIVE_EXAMPLES=1)
- `load-example-modules` - Load example cognitive modules
- `test-pattern <input>` - Test pattern recognition module
- `test-attention <input>` - Test attention allocation module

## Cognitive Grammar Kernel

### Hypergraph Encoding

The cognitive kernel provides a Scheme-like hypergraph encoding system for representing knowledge and actions:

```bash
# Simple encoding
hypergraph-encode "hello world"
# Result: (hypergraph (concept "hello") (concept "world") (link sequence (ordered-link "hello" "world") ))

# Complex encoding
hypergraph-encode "The cat sits on the mat"
# Result: (hypergraph (concept "The") (concept "cat") ... (link sequence ...))
```

The hypergraph representation creates:
- **Concept nodes** for each significant word
- **Link sequences** showing relationships between concepts
- **Ordered links** preserving sequential structure

### ECAN Attention Allocation

Economic Cognitive Agent Network (ECAN) attention allocation based on input complexity:

```bash
attention-allocate "simple input"
# ECAN Attention Allocated:
#   Total Attention: 45
#   Short-term Importance: 60
#   Long-term Importance: 30
#   Very Long-term Importance: 6
#   Stimulation Level: 12
```

ECAN calculates:
- **STI (Short-term Importance)**: Immediate relevance
- **LTI (Long-term Importance)**: Persistent significance  
- **VLTI (Very Long-term Importance)**: Deep memory relevance
- **Stimulation Level**: Current activation intensity

### PLN Probabilistic Reasoning

Probabilistic Logic Networks (PLN) for logical inference with uncertainty:

```bash
pln-infer "All birds fly"
# PLN Inference Result:
#   Premises: All birds fly
#   Conclusion: (conclusion "All birds fly" (tv 65 72))
#   Truth Value: (65, 72)
```

PLN provides:
- **Strength**: Probability measure (0-100)
- **Confidence**: Certainty in the probability (0-100)
- **Deduction**: A→B, B→C ⊢ A→C inference
- **Induction**: Evidence-based generalization

### Cognitive Pattern Transformations

Map shell commands to cognitive pattern transformations:

```bash
cognitive-transform "greeting" "hello world"
# Cognitive Transform Result:
#   Pattern: greeting
#   Input: hello world
#   Transform: (transform (pattern "greeting") (input "hello world") (attention 83))
```

Pattern transformations:
- Apply cognitive patterns to shell input
- Weight transformations by attention values
- Enable context-aware command processing

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
./test-cognitive-kernel.sh  # Enhanced kernel tests
```

The test suites verify:
- Basic cognitive functionality and module loading
- Hypergraph encoding with Scheme-like syntax
- ECAN attention allocation with STI/LTI/VLTI calculation
- PLN probabilistic reasoning with truth values
- Cognitive pattern transformations
- Integration between cognitive modules

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
- ✅ **Hypergraph encoding library (Scheme-like syntax)**
- ✅ **ECAN attention allocation (STI/LTI/VLTI)**
- ✅ **PLN probabilistic reasoning (truth values)**
- ✅ **Cognitive pattern transformations**
- ✅ **Scheme integration (basic expressions)**
- ✅ **Prime factorization-based tensor membranes**
- ✅ **P-system membrane dynamics**
- ✅ **Dynamic tensor allocation and reshaping**
- ✅ Documentation

Future enhancements can include:
- Full Scheme interpreter integration (currently basic expressions)
- Complete ECAN dynamics with spread and decay
- Advanced PLN inference rules (currently deduction/induction)
- ggml tensor operations integration
- ZeroMQ/gRPC network protocols
- Distributed agent coordination
- Advanced pattern recognition
- Recursive grammar expansion
- Membrane rule execution engine for P-systems

The architecture is designed to support incremental development of these advanced features while maintaining shell compatibility and performance.
# Cognitive Grammar Kernel Implementation

## Overview

This document describes the implemented hypergraph encoding system, ECAN attention allocation, and PLN probabilistic reasoning for the cognitive grammar kernel in the r9 shell.

## Hypergraph Encoding Implementation

### Data Structures
The cognitive kernel uses C structures for efficient hypergraph representation:

```c
typedef struct HypergraphNode {
    char *name;
    float attention_value;
    int concept_type;  /* 0=concept, 1=link, 2=predicate */
    struct HypergraphNode **children;
    int child_count;
    struct HypergraphNode *next;
} HypergraphNode;
```

### Encoding Algorithm
The `encode_to_hypergraph()` function converts text input to Scheme-like hypergraph representation:

```c
// Example: "hello world" becomes:
// (hypergraph (concept "hello") (concept "world") 
//   (link sequence (ordered-link "hello" "world")))
```

### Shell Command Integration
Commands are automatically encoded when using `hypergraph-encode`:

```bash
$ hypergraph-encode "The cat sits on the mat"
Hypergraph encoding: (hypergraph (concept "The") (concept "cat") (concept "sits") 
  (concept "on") (concept "the") (concept "mat") 
  (link sequence (ordered-link "The" "cat") (ordered-link "cat" "sits") ...))
```

## ECAN Attention Allocation Implementation

### ECAN Values Structure
```c
typedef struct {
    float short_term_importance;    // STI - immediate relevance
    float long_term_importance;     // LTI - persistent significance  
    float very_long_term_importance; // VLTI - deep memory relevance
    unsigned int stimulation_level;  // current activation
} ECANValues;
```

### Attention Calculation
The `calculate_ecan_attention()` function implements economic attention distribution:

```c
float calculate_ecan_attention(const char *input, ECANValues *ecan) {
    size_t complexity = strlen(input);
    
    ecan->short_term_importance = complexity * 0.1f;
    ecan->long_term_importance = complexity * 0.05f; 
    ecan->very_long_term_importance = complexity * 0.01f;
    ecan->stimulation_level = (unsigned int)(complexity * 2);
    
    return ecan->short_term_importance * 0.6f + 
           ecan->long_term_importance * 0.3f + 
           ecan->very_long_term_importance * 0.1f;
}
```

### Usage Example
```bash
$ attention-allocate "complex cognitive processing task"
ECAN Attention Allocated:
  Total Attention: 250
  Short-term Importance: 330
  Long-term Importance: 165
  Very Long-term Importance: 33
  Stimulation Level: 66
```

## PLN Probabilistic Logic Networks Implementation

### Truth Value Representation
```c
typedef struct {
    float strength;    /* Probability [0,1] */
    float confidence;  /* Confidence [0,1] */
} TruthValue;
```

### Inference Rules
Implemented PLN operations include:

```c
// Deduction: A→B ∧ B→C ⊢ A→C
TruthValue pln_deduction(TruthValue premise1, TruthValue premise2) {
    TruthValue result;
    result.strength = premise1.strength * premise2.strength;
    result.confidence = premise1.confidence * premise2.confidence;
    return result;
}

// Induction: Evidence-based generalization
TruthValue pln_induction(TruthValue evidence, float prior_strength) {
    TruthValue result;
    result.strength = (evidence.strength + prior_strength) / 2.0f;
    result.confidence = evidence.confidence * 0.8f;
    return result;
}
```

### PLN Shell Integration
```bash
$ pln-infer "All birds fly"
PLN Inference Result:
  Premises: All birds fly
  Conclusion: (conclusion "All birds fly" (tv 65 72))
  Truth Value: (65, 72)
```

## Cognitive Pattern Transformations

### Implementation
The `cognitive-transform` command applies patterns with attention weighting:

```c
void b_cognitive_transform(char **av) {
    const char *pattern = av[1];
    const char *input = av[2];
    
    HypergraphKernel *kernel = find_hypergraph_kernel("default");
    if (kernel && kernel->transform) {
        char *output = NULL;
        kernel->transform(pattern, input, &output);
        // Display transformation result
    }
}
```

### Usage Examples
```bash
$ cognitive-transform "greeting" "hello world"
Cognitive Transform Result:
  Pattern: greeting
  Input: hello world
  Transform: (transform (pattern "greeting") (input "hello world") (attention 83))

$ cognitive-transform "command" "ls -la /tmp"  
Cognitive Transform Result:
  Pattern: command
  Input: ls -la /tmp
  Transform: (transform (pattern "command") (input "ls -la /tmp") (attention 83))
```

## Scheme Integration Implementation

### Basic Expression Evaluator
The cognitive kernel includes a minimal Scheme evaluator:

```c
int scheme_eval(const char *expr) {
    if (strncmp(expr, "(+ ", 3) == 0) {
        int a, b;
        if (sscanf(expr, "(+ %d %d)", &a, &b) == 2) {
            return a + b;
        }
    } else if (strncmp(expr, "(* ", 3) == 0) {
        int a, b;  
        if (sscanf(expr, "(* %d %d)", &a, &b) == 2) {
            return a * b;
        }
    }
    return 0;
}
```

### Scheme Function Calls
Enhanced `scheme_call()` supports cognitive operations:

```c
char *scheme_call(const char *func, char **args) {
    if (strcmp(func, "hypergraph-encode") == 0) {
        // Use hypergraph kernel
    } else if (strcmp(func, "ecan-allocate") == 0) {
        // Perform ECAN attention allocation
    } else if (strcmp(func, "pln-infer") == 0) {
        // Execute PLN inference
    }
}
```

## Shell Integration Architecture

### Command Processing Pipeline
1. **Input Parsing**: Shell command parsed through cognitive layer
2. **Hypergraph Encoding**: Command structure encoded as hypergraph
3. **Attention Allocation**: ECAN calculates resource distribution
4. **Pattern Matching**: Cognitive patterns applied to command
5. **PLN Reasoning**: Probabilistic inference about outcomes
6. **Execution**: Command executed with cognitive context
7. **State Update**: Cognitive state updated based on results

### Cognitive Module System
The implementation uses a plugin architecture:

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

### Available Commands

**Core Cognitive Commands:**
- `cognitive-status` - Display system state and loaded modules
- `hypergraph-encode <text>` - Convert text to hypergraph representation
- `attention-allocate <input>` - ECAN attention calculation
- `pln-infer <premises>` - Probabilistic logical inference
- `cognitive-transform <pattern> <input>` - Pattern transformation
- `scheme-eval <expression>` - Basic Scheme expression evaluation

**Example Module Commands:**
- `load-example-modules` - Load pattern recognition and attention modules
- `test-pattern <input>` - Test pattern recognition
- `test-attention <input>` - Test attention allocation

## Configuration

Enable cognitive features in `config.h`:
```c
#define ENABLE_COGNITIVE_GRAMMAR 1
#define ENABLE_IPC_EXTENSIONS 1  
#define ENABLE_SCHEME_INTEGRATION 1
#define ENABLE_COGNITIVE_EXAMPLES 1
```

## Testing

Comprehensive test suites verify implementation:
```bash
./test-cognitive.sh        # Basic cognitive functionality
./test-cognitive-kernel.sh # Enhanced kernel features
```

The tests verify:
- Hypergraph encoding with proper Scheme syntax
- ECAN attention allocation with STI/LTI/VLTI calculation
- PLN inference with truth value computation
- Cognitive pattern transformations
- Integration between all cognitive components
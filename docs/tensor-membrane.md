# Tensor Membrane

## Overview

This document describes the tensor design, prime factorization-based shapes, and P-system embedding for the cognitive membrane architecture.

## Conceptual Framework

### Tensor Membranes as Cognitive Boundaries
Tensor membranes serve as computational boundaries that:
- Encapsulate cognitive processes and data
- Define interaction interfaces between cognitive agents
- Implement P-system-like computational models
- Enable dynamic resource allocation and attention management

### Prime Factorization Geometry
Membrane shapes are defined by prime factorization to create hierarchical, composable structures:
```
Prime: 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37...

Membrane Shapes:
- [2] → Linear processing (1D)
- [2,3] → Matrix operations (2D) 
- [2,3,5] → Volume processing (3D)
- [2,2,3] → Hierarchical matrix (nested 2D)
- [7,11] → High-dimensional sparse patterns
```

## Data Structures

### Core Tensor Membrane
```c
typedef struct TensorMembrane {
    uint32_t id;                    // Unique membrane identifier
    uint32_t prime_factors[16];     // Prime factorization shape
    uint32_t factor_count;          // Number of prime factors
    float *data;                    // Tensor data storage
    size_t data_size;               // Size in bytes
    uint64_t version;               // Version for synchronization
    struct TensorMembrane *parent;  // Parent membrane (for nesting)
    struct TensorMembrane **children; // Child membranes
    uint32_t child_count;           // Number of children
    void *cognitive_context;        // Associated cognitive state
    uint32_t access_permissions;    // Access control flags
} TensorMembrane;

// Memory pool for efficient allocation
typedef struct MembranePool {
    TensorMembrane *membranes;
    uint32_t pool_size;
    uint32_t allocated_count;
    pthread_mutex_t allocation_lock;
} MembranePool;
```

### Prime Factorization Utilities
```c
// Prime factorization functions
extern bool is_prime(uint32_t n);
extern uint32_t *prime_factors(uint32_t n, uint32_t *count);
extern uint32_t prime_product(uint32_t *factors, uint32_t count);
extern bool factors_compatible(uint32_t *f1, uint32_t c1, uint32_t *f2, uint32_t c2);

// Shape analysis
extern uint32_t compute_tensor_dimensions(uint32_t *factors, uint32_t count);
extern size_t compute_tensor_size(uint32_t *factors, uint32_t count);
extern bool can_reshape(uint32_t *from_factors, uint32_t *to_factors, 
                       uint32_t from_count, uint32_t to_count);
```

## P-System Implementation

### Membrane Rules and Operations
```c
typedef enum {
    RULE_REWRITE,       // Object rewriting within membrane
    RULE_COMMUNICATE,   // Send objects between membranes  
    RULE_DISSOLVE,      // Dissolve membrane boundary
    RULE_CREATE,        // Create new child membrane
    RULE_MERGE,         // Merge two membranes
    RULE_DIVIDE         // Divide membrane into children
} MembraneRuleType;

typedef struct {
    MembraneRuleType type;
    uint32_t source_membrane;
    uint32_t target_membrane;
    char *condition;            // Rule activation condition
    char *transformation;       // Object transformation
    float priority;             // Rule priority
    uint32_t energy_cost;       // Energy required for execution
} MembraneRule;

// Rule execution engine
extern int execute_membrane_rule(MembraneRule *rule, TensorMembrane *membrane);
extern MembraneRule *find_applicable_rules(TensorMembrane *membrane);
extern int membrane_step(TensorMembrane *membrane); // Single P-system step
```

### Object Management
```c
typedef struct {
    char *symbol;               // Object symbol/name
    float *properties;          // Numerical properties
    uint32_t property_count;    // Number of properties
    uint32_t multiplicity;      // Object count in membrane
} MembraneObject;

// Object operations
extern int membrane_add_object(TensorMembrane *membrane, MembraneObject *obj);
extern int membrane_remove_object(TensorMembrane *membrane, const char *symbol);
extern MembraneObject *membrane_find_object(TensorMembrane *membrane, const char *symbol);
extern int membrane_transfer_object(TensorMembrane *from, TensorMembrane *to, 
                                   const char *symbol, uint32_t count);
```

## Dynamic Allocation

### Membrane Lifecycle Management
```c
// Membrane creation with prime factorization
extern TensorMembrane *membrane_create(uint32_t *prime_factors, uint32_t count);
extern TensorMembrane *membrane_create_child(TensorMembrane *parent, 
                                            uint32_t *factors, uint32_t count);
extern int membrane_destroy(TensorMembrane *membrane);
extern int membrane_resize(TensorMembrane *membrane, uint32_t *new_factors, 
                          uint32_t count);

// Memory management
extern MembranePool *membrane_pool_create(uint32_t initial_size);
extern TensorMembrane *membrane_pool_allocate(MembranePool *pool);
extern int membrane_pool_deallocate(MembranePool *pool, TensorMembrane *membrane);
extern void membrane_pool_destroy(MembranePool *pool);
```

### Adaptive Reshaping
```c
// Dynamic membrane adaptation
typedef struct {
    float utilization_threshold;   // Trigger for reshaping
    uint32_t access_pattern[16];   // Access frequency per dimension
    uint64_t last_reshape_time;    // Prevent frequent reshaping
    uint32_t reshape_cost;         // Cost of last reshape operation
} ReshapeMetrics;

extern int membrane_analyze_access_pattern(TensorMembrane *membrane);
extern bool membrane_should_reshape(TensorMembrane *membrane, ReshapeMetrics *metrics);
extern int membrane_adaptive_reshape(TensorMembrane *membrane);
```

## Tensor Operations

### Basic Operations
```c
// Element access and modification
extern float membrane_get_element(TensorMembrane *membrane, uint32_t *indices);
extern int membrane_set_element(TensorMembrane *membrane, uint32_t *indices, float value);
extern int membrane_fill(TensorMembrane *membrane, float value);
extern int membrane_copy(TensorMembrane *dest, TensorMembrane *src);

// Mathematical operations
extern int membrane_add(TensorMembrane *result, TensorMembrane *a, TensorMembrane *b);
extern int membrane_multiply(TensorMembrane *result, TensorMembrane *a, TensorMembrane *b);
extern int membrane_transform(TensorMembrane *membrane, const char *operation);
extern float membrane_reduce(TensorMembrane *membrane, const char *reduction_op);
```

### Advanced Tensor Computations
```c
// Cognitive-specific operations
extern int membrane_attention_focus(TensorMembrane *membrane, float *attention_weights);
extern int membrane_pattern_match(TensorMembrane *pattern, TensorMembrane *data, 
                                 float *similarity);
extern int membrane_associative_lookup(TensorMembrane *memory, TensorMembrane *query,
                                      TensorMembrane *result);
extern int membrane_cognitive_update(TensorMembrane *membrane, float learning_rate,
                                    TensorMembrane *gradient);
```

## Integration with ggml

### ggml Tensor Bridging
```c
// Bridge between TensorMembrane and ggml
extern struct ggml_tensor *membrane_to_ggml(TensorMembrane *membrane, 
                                           struct ggml_context *ctx);
extern TensorMembrane *ggml_to_membrane(struct ggml_tensor *tensor);
extern int membrane_ggml_compute(TensorMembrane *membrane, const char *operation);

// Batch operations using ggml backend
extern int membrane_ggml_batch_process(TensorMembrane **membranes, 
                                      uint32_t count, const char *operation);
```

### Memory Optimization
```c
// ggml-backed memory management
typedef struct {
    struct ggml_context *ggml_ctx;
    size_t memory_size;
    void *memory_buffer;
    uint32_t active_membranes;
} GgmlMembraneContext;

extern GgmlMembraneContext *ggml_membrane_context_create(size_t memory_size);
extern TensorMembrane *ggml_membrane_allocate(GgmlMembraneContext *ctx,
                                             uint32_t *factors, uint32_t count);
extern void ggml_membrane_context_destroy(GgmlMembraneContext *ctx);
```

## Visualization and Debugging

### Membrane Inspection
```c
// Debug and visualization functions
extern void membrane_print_structure(TensorMembrane *membrane, int depth);
extern char *membrane_serialize_state(TensorMembrane *membrane);
extern TensorMembrane *membrane_deserialize_state(const char *state);
extern int membrane_export_dot(TensorMembrane *membrane, const char *filename);
extern int membrane_validate_integrity(TensorMembrane *membrane);
```

### Performance Monitoring
```c
typedef struct {
    uint64_t operation_count;
    uint64_t total_compute_time;
    uint64_t memory_accesses;
    uint64_t cache_hits;
    uint64_t cache_misses;
    float average_utilization;
} MembraneMetrics;

extern MembraneMetrics *membrane_get_metrics(TensorMembrane *membrane);
extern void membrane_reset_metrics(TensorMembrane *membrane);
extern void membrane_print_performance_report(TensorMembrane *membrane);
```

## Shell Integration

### Membrane Shell Commands
New shell commands for tensor membrane operations:
```bash
# Membrane creation and management
membrane-create [2,3,5]           # Create membrane with prime shape
membrane-list                     # List all active membranes  
membrane-info <id>               # Show membrane details
membrane-destroy <id>            # Destroy membrane

# Data operations
membrane-set <id> <indices> <value>  # Set tensor element
membrane-get <id> <indices>          # Get tensor element
membrane-fill <id> <value>           # Fill with constant value
membrane-copy <src_id> <dest_id>     # Copy membrane data

# P-system operations
membrane-add-rule <id> <rule>        # Add computation rule
membrane-step <id>                   # Execute one P-system step
membrane-run <id> <steps>            # Run multiple steps

# Advanced operations
membrane-reshape <id> [new_shape]    # Reshape membrane
membrane-merge <id1> <id2>           # Merge membranes
membrane-split <id> <factor>         # Split membrane
```

## Configuration

Tensor membrane settings in config.h:
```c
#define MAX_MEMBRANE_COUNT 1024
#define MAX_PRIME_FACTORS 16
#define DEFAULT_MEMBRANE_POOL_SIZE 256
#define MEMBRANE_CACHE_SIZE (64 * 1024 * 1024)  // 64MB
#define MAX_MEMBRANE_NESTING_DEPTH 8
#define RESHAPE_ANALYSIS_INTERVAL 1000  // operations
#define GGML_MEMORY_SIZE (512 * 1024 * 1024)  // 512MB
```
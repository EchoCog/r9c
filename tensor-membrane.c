/* Tensor Membrane Implementation
 * Prime factorization-based tensor shapes and P-system dynamics
 */

#include "rc.h"
#include "cognitive.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Prime Factorization Utilities */

bool is_prime(uint32_t n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    uint32_t limit = (uint32_t)sqrt((double)n);
    for (uint32_t i = 3; i <= limit; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

uint32_t *prime_factors(uint32_t n, uint32_t *count) {
    if (!count) return NULL;
    
    uint32_t *factors = malloc(16 * sizeof(uint32_t));
    if (!factors) return NULL;
    
    *count = 0;
    uint32_t temp = n;
    
    /* Handle factor 2 */
    while (temp % 2 == 0 && *count < 16) {
        factors[(*count)++] = 2;
        temp /= 2;
    }
    
    /* Handle odd factors */
    for (uint32_t i = 3; i <= sqrt((double)temp) && *count < 16; i += 2) {
        while (temp % i == 0 && *count < 16) {
            factors[(*count)++] = i;
            temp /= i;
        }
    }
    
    /* If temp is still > 2, it's a prime factor */
    if (temp > 2 && *count < 16) {
        factors[(*count)++] = temp;
    }
    
    return factors;
}

uint32_t prime_product(uint32_t *factors, uint32_t count) {
    if (!factors || count == 0) return 1;
    
    uint32_t product = 1;
    for (uint32_t i = 0; i < count; i++) {
        product *= factors[i];
    }
    return product;
}

bool factors_compatible(uint32_t *f1, uint32_t c1, uint32_t *f2, uint32_t c2) {
    if (!f1 || !f2) return false;
    
    /* Two factor sets are compatible if they have the same prime product */
    return prime_product(f1, c1) == prime_product(f2, c2);
}

uint32_t compute_tensor_dimensions(uint32_t *factors, uint32_t count) {
    if (!factors || count == 0) return 0;
    
    /* Each unique prime factor represents a dimension */
    uint32_t unique_primes[16];
    uint32_t unique_count = 0;
    
    for (uint32_t i = 0; i < count && unique_count < 16; i++) {
        bool found = false;
        for (uint32_t j = 0; j < unique_count; j++) {
            if (unique_primes[j] == factors[i]) {
                found = true;
                break;
            }
        }
        if (!found) {
            unique_primes[unique_count++] = factors[i];
        }
    }
    
    return unique_count;
}

size_t compute_tensor_size(uint32_t *factors, uint32_t count) {
    if (!factors || count == 0) return 0;
    
    /* Group factors by prime value and use powers as dimension sizes */
    uint32_t unique_primes[16];
    uint32_t prime_powers[16];
    uint32_t unique_count = 0;
    
    for (uint32_t i = 0; i < count && unique_count < 16; i++) {
        uint32_t prime = factors[i];
        bool found = false;
        
        for (uint32_t j = 0; j < unique_count; j++) {
            if (unique_primes[j] == prime) {
                prime_powers[j]++;
                found = true;
                break;
            }
        }
        
        if (!found) {
            unique_primes[unique_count] = prime;
            prime_powers[unique_count] = 1;
            unique_count++;
        }
    }
    
    /* Calculate total size as product of dimension sizes */
    size_t total_size = 1;
    for (uint32_t i = 0; i < unique_count; i++) {
        total_size *= prime_powers[i];
    }
    
    return total_size;
}

bool can_reshape(uint32_t *from_factors, uint32_t *to_factors, 
                uint32_t from_count, uint32_t to_count) {
    /* Can reshape if the prime products are equal */
    return factors_compatible(from_factors, from_count, to_factors, to_count);
}

/* Enhanced Tensor Membrane Structure */
typedef struct TensorMembraneImpl {
    uint32_t id;                    /* Unique membrane identifier */
    uint32_t prime_factors[16];     /* Prime factorization shape */
    uint32_t factor_count;          /* Number of prime factors */
    float *data;                    /* Tensor data storage */
    size_t data_size;               /* Size in bytes */
    uint64_t version;               /* Version for synchronization */
    struct TensorMembraneImpl *parent;  /* Parent membrane (for nesting) */
    struct TensorMembraneImpl **children; /* Child membranes */
    uint32_t child_count;           /* Number of children */
    uint32_t max_children;          /* Maximum child capacity */
    void *cognitive_context;        /* Associated cognitive state */
    uint32_t access_permissions;    /* Access control flags */
    
    /* P-system specific fields */
    uint32_t energy_level;          /* Available energy for operations */
    char **objects;                 /* Object symbols in membrane */
    uint32_t object_count;          /* Number of objects */
    uint32_t max_objects;           /* Maximum object capacity */
    
    /* Performance metrics */
    uint64_t operation_count;
    uint64_t access_count;
    float utilization;
} TensorMembraneImpl;

/* Global membrane registry */
static TensorMembraneImpl *membrane_registry[64];
static uint32_t membrane_count = 0;
static uint32_t next_membrane_id = 1;

/* Membrane Lifecycle Management */

TensorMembraneImpl *membrane_create(uint32_t *prime_factors, uint32_t count) {
    if (!prime_factors || count == 0 || count > 16 || membrane_count >= 64) {
        return NULL;
    }
    
    TensorMembraneImpl *membrane = malloc(sizeof(TensorMembraneImpl));
    if (!membrane) return NULL;
    
    /* Initialize membrane structure */
    membrane->id = next_membrane_id++;
    membrane->factor_count = count;
    for (uint32_t i = 0; i < count; i++) {
        membrane->prime_factors[i] = prime_factors[i];
    }
    for (uint32_t i = count; i < 16; i++) {
        membrane->prime_factors[i] = 0; /* Zero padding */
    }
    
    /* Calculate and allocate tensor data */
    membrane->data_size = compute_tensor_size(prime_factors, count) * sizeof(float);
    membrane->data = malloc(membrane->data_size);
    if (!membrane->data) {
        free(membrane);
        return NULL;
    }
    
    /* Initialize tensor data with small random values */
    size_t element_count = membrane->data_size / sizeof(float);
    for (size_t i = 0; i < element_count; i++) {
        membrane->data[i] = ((float)rand() / RAND_MAX) * 0.1f;
    }
    
    /* Initialize P-system state */
    membrane->version = 1;
    membrane->parent = NULL;
    membrane->children = NULL;
    membrane->child_count = 0;
    membrane->max_children = 8;
    membrane->cognitive_context = NULL;
    membrane->access_permissions = 0xFFFFFFFF; /* Full access by default */
    membrane->energy_level = 100; /* Start with full energy */
    
    /* Initialize object container */
    membrane->objects = malloc(16 * sizeof(char*));
    membrane->object_count = 0;
    membrane->max_objects = 16;
    if (!membrane->objects) {
        free(membrane->data);
        free(membrane);
        return NULL;
    }
    
    /* Initialize metrics */
    membrane->operation_count = 0;
    membrane->access_count = 0;
    membrane->utilization = 0.0f;
    
    /* Register membrane */
    membrane_registry[membrane_count++] = membrane;
    
    return membrane;
}

TensorMembraneImpl *membrane_create_child(TensorMembraneImpl *parent, 
                                         uint32_t *factors, uint32_t count) {
    if (!parent || parent->child_count >= parent->max_children) {
        return NULL;
    }
    
    TensorMembraneImpl *child = membrane_create(factors, count);
    if (!child) return NULL;
    
    /* Set up parent-child relationship */
    child->parent = parent;
    
    /* Allocate child array if needed */
    if (!parent->children) {
        parent->children = malloc(parent->max_children * sizeof(TensorMembraneImpl*));
        if (!parent->children) {
            membrane_destroy(child);
            return NULL;
        }
    }
    
    parent->children[parent->child_count++] = child;
    return child;
}

int membrane_destroy(TensorMembraneImpl *membrane) {
    if (!membrane) return -1;
    
    /* Remove from registry */
    for (uint32_t i = 0; i < membrane_count; i++) {
        if (membrane_registry[i] == membrane) {
            /* Shift remaining entries */
            for (uint32_t j = i; j < membrane_count - 1; j++) {
                membrane_registry[j] = membrane_registry[j + 1];
            }
            membrane_count--;
            break;
        }
    }
    
    /* Destroy all children first */
    if (membrane->children) {
        for (uint32_t i = 0; i < membrane->child_count; i++) {
            if (membrane->children[i]) {
                membrane_destroy(membrane->children[i]);
            }
        }
        free(membrane->children);
    }
    
    /* Remove from parent's child list */
    if (membrane->parent) {
        for (uint32_t i = 0; i < membrane->parent->child_count; i++) {
            if (membrane->parent->children[i] == membrane) {
                /* Shift remaining children */
                for (uint32_t j = i; j < membrane->parent->child_count - 1; j++) {
                    membrane->parent->children[j] = membrane->parent->children[j + 1];
                }
                membrane->parent->child_count--;
                break;
            }
        }
    }
    
    /* Free resources */
    if (membrane->data) free(membrane->data);
    if (membrane->objects) {
        for (uint32_t i = 0; i < membrane->object_count; i++) {
            if (membrane->objects[i]) free(membrane->objects[i]);
        }
        free(membrane->objects);
    }
    
    free(membrane);
    return 0;
}

int membrane_resize(TensorMembraneImpl *membrane, uint32_t *new_factors, uint32_t count) {
    if (!membrane || !new_factors || count == 0 || count > 16) return -1;
    
    /* Check if reshape is possible */
    if (!can_reshape(membrane->prime_factors, new_factors, 
                    membrane->factor_count, count)) {
        return -1; /* Incompatible shapes */
    }
    
    /* Calculate new size */
    size_t new_size = compute_tensor_size(new_factors, count) * sizeof(float);
    
    /* Reallocate data if size changed */
    if (new_size != membrane->data_size) {
        float *new_data = realloc(membrane->data, new_size);
        if (!new_data) return -1;
        
        membrane->data = new_data;
        membrane->data_size = new_size;
    }
    
    /* Update shape */
    membrane->factor_count = count;
    for (uint32_t i = 0; i < count; i++) {
        membrane->prime_factors[i] = new_factors[i];
    }
    for (uint32_t i = count; i < 16; i++) {
        membrane->prime_factors[i] = 0;
    }
    
    membrane->version++;
    return 0;
}

/* P-System Operations */

int membrane_add_object(TensorMembraneImpl *membrane, const char *symbol) {
    if (!membrane || !symbol || membrane->object_count >= membrane->max_objects) {
        return -1;
    }
    
    /* Check if object already exists */
    for (uint32_t i = 0; i < membrane->object_count; i++) {
        if (membrane->objects[i] && strcmp(membrane->objects[i], symbol) == 0) {
            return 0; /* Already present */
        }
    }
    
    /* Add new object */
    membrane->objects[membrane->object_count] = malloc(strlen(symbol) + 1);
    if (!membrane->objects[membrane->object_count]) return -1;
    
    strcpy(membrane->objects[membrane->object_count], symbol);
    membrane->object_count++;
    
    return 0;
}

int membrane_remove_object(TensorMembraneImpl *membrane, const char *symbol) {
    if (!membrane || !symbol) return -1;
    
    for (uint32_t i = 0; i < membrane->object_count; i++) {
        if (membrane->objects[i] && strcmp(membrane->objects[i], symbol) == 0) {
            free(membrane->objects[i]);
            
            /* Shift remaining objects */
            for (uint32_t j = i; j < membrane->object_count - 1; j++) {
                membrane->objects[j] = membrane->objects[j + 1];
            }
            membrane->object_count--;
            return 0;
        }
    }
    
    return -1; /* Object not found */
}

const char *membrane_find_object(TensorMembraneImpl *membrane, const char *symbol) {
    if (!membrane || !symbol) return NULL;
    
    for (uint32_t i = 0; i < membrane->object_count; i++) {
        if (membrane->objects[i] && strcmp(membrane->objects[i], symbol) == 0) {
            return membrane->objects[i];
        }
    }
    
    return NULL;
}

int membrane_transfer_object(TensorMembraneImpl *from, TensorMembraneImpl *to, 
                            const char *symbol) {
    if (!from || !to || !symbol) return -1;
    
    /* Check if object exists in source */
    if (!membrane_find_object(from, symbol)) return -1;
    
    /* Add to destination */
    if (membrane_add_object(to, symbol) != 0) return -1;
    
    /* Remove from source */
    return membrane_remove_object(from, symbol);
}

/* Basic Tensor Operations */

float membrane_get_element(TensorMembraneImpl *membrane, uint32_t *indices) {
    if (!membrane || !indices || !membrane->data) return 0.0f;
    
    /* Calculate flat index from multi-dimensional indices */
    size_t flat_index = 0;
    size_t stride = 1;
    
    /* Simple row-major calculation for now */
    uint32_t dims = compute_tensor_dimensions(membrane->prime_factors, membrane->factor_count);
    for (int i = dims - 1; i >= 0; i--) {
        flat_index += indices[i] * stride;
        stride *= membrane->prime_factors[i]; /* Use prime as dimension size */
    }
    
    size_t max_elements = membrane->data_size / sizeof(float);
    if (flat_index >= max_elements) return 0.0f;
    
    membrane->access_count++;
    return membrane->data[flat_index];
}

int membrane_set_element(TensorMembraneImpl *membrane, uint32_t *indices, float value) {
    if (!membrane || !indices || !membrane->data) return -1;
    
    /* Calculate flat index */
    size_t flat_index = 0;
    size_t stride = 1;
    
    uint32_t dims = compute_tensor_dimensions(membrane->prime_factors, membrane->factor_count);
    for (int i = dims - 1; i >= 0; i--) {
        flat_index += indices[i] * stride;
        stride *= membrane->prime_factors[i];
    }
    
    size_t max_elements = membrane->data_size / sizeof(float);
    if (flat_index >= max_elements) return -1;
    
    membrane->data[flat_index] = value;
    membrane->operation_count++;
    membrane->version++;
    
    return 0;
}

int membrane_fill(TensorMembraneImpl *membrane, float value) {
    if (!membrane || !membrane->data) return -1;
    
    size_t element_count = membrane->data_size / sizeof(float);
    for (size_t i = 0; i < element_count; i++) {
        membrane->data[i] = value;
    }
    
    membrane->operation_count++;
    membrane->version++;
    return 0;
}

/* Utility Functions */

TensorMembraneImpl *find_membrane_by_id(uint32_t id) {
    for (uint32_t i = 0; i < membrane_count; i++) {
        if (membrane_registry[i] && membrane_registry[i]->id == id) {
            return membrane_registry[i];
        }
    }
    return NULL;
}

void membrane_print_structure(TensorMembraneImpl *membrane, int depth) {
    if (!membrane) return;
    
    /* Print indentation */
    for (int i = 0; i < depth; i++) {
        fprint(1, "  ");
    }
    
    fprint(1, "Membrane %d: [", (int)membrane->id);
    for (uint32_t i = 0; i < membrane->factor_count; i++) {
        fprint(1, "%d", (int)membrane->prime_factors[i]);
        if (i < membrane->factor_count - 1) fprint(1, ",");
    }
    fprint(1, "] energy=%d objects=%d children=%d\n", 
           (int)membrane->energy_level, (int)membrane->object_count, 
           (int)membrane->child_count);
    
    /* Print objects */
    for (uint32_t i = 0; i < membrane->object_count; i++) {
        for (int j = 0; j < depth + 1; j++) fprint(1, "  ");
        fprint(1, "obj: %s\n", membrane->objects[i] ? membrane->objects[i] : "NULL");
    }
    
    /* Recursively print children */
    for (uint32_t i = 0; i < membrane->child_count; i++) {
        membrane_print_structure(membrane->children[i], depth + 1);
    }
}

/* Shell Command Integration */

/* These functions are called from cognitive.c shell commands */
void *tensor_membrane_create_prime(int prime_factors[], int count) {
    if (!prime_factors || count <= 0) return NULL;
    
    uint32_t factors[16];
    for (int i = 0; i < count && i < 16; i++) {
        factors[i] = (uint32_t)prime_factors[i];
    }
    
    return (void*)membrane_create(factors, (uint32_t)count);
}

void tensor_membrane_destroy_prime(void *membrane_ptr) {
    TensorMembraneImpl *membrane = (TensorMembraneImpl*)membrane_ptr;
    membrane_destroy(membrane);
}

int tensor_membrane_add_object_prime(void *membrane_ptr, const char *symbol) {
    TensorMembraneImpl *membrane = (TensorMembraneImpl*)membrane_ptr;
    return membrane_add_object(membrane, symbol);
}

void tensor_membrane_print_prime(void *membrane_ptr) {
    TensorMembraneImpl *membrane = (TensorMembraneImpl*)membrane_ptr;
    membrane_print_structure(membrane, 0);
}

uint32_t tensor_membrane_get_id_prime(void *membrane_ptr) {
    TensorMembraneImpl *membrane = (TensorMembraneImpl*)membrane_ptr;
    return membrane ? membrane->id : 0;
}

void *tensor_membrane_find_by_id_prime(uint32_t id) {
    return (void*)find_membrane_by_id(id);
}

int tensor_membrane_get_count_prime(void) {
    return (int)membrane_count;
}
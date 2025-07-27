/* Tensor Membrane Header
 * Prime factorization-based tensor shapes and P-system dynamics
 */

#ifndef TENSOR_MEMBRANE_H
#define TENSOR_MEMBRANE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Forward declarations */
typedef struct TensorMembraneImpl TensorMembraneImpl;

/* Prime Factorization Utilities */
extern bool is_prime(uint32_t n);
extern uint32_t *prime_factors(uint32_t n, uint32_t *count);
extern uint32_t prime_product(uint32_t *factors, uint32_t count);
extern bool factors_compatible(uint32_t *f1, uint32_t c1, uint32_t *f2, uint32_t c2);

/* Shape analysis */
extern uint32_t compute_tensor_dimensions(uint32_t *factors, uint32_t count);
extern size_t compute_tensor_size(uint32_t *factors, uint32_t count);
extern bool can_reshape(uint32_t *from_factors, uint32_t *to_factors, 
                       uint32_t from_count, uint32_t to_count);

/* Membrane Lifecycle Management */
extern TensorMembraneImpl *membrane_create(uint32_t *prime_factors, uint32_t count);
extern TensorMembraneImpl *membrane_create_child(TensorMembraneImpl *parent, 
                                                 uint32_t *factors, uint32_t count);
extern int membrane_destroy(TensorMembraneImpl *membrane);
extern int membrane_resize(TensorMembraneImpl *membrane, uint32_t *new_factors, 
                          uint32_t count);

/* Object Management (P-system) */
extern int membrane_add_object(TensorMembraneImpl *membrane, const char *symbol);
extern int membrane_remove_object(TensorMembraneImpl *membrane, const char *symbol);
extern const char *membrane_find_object(TensorMembraneImpl *membrane, const char *symbol);
extern int membrane_transfer_object(TensorMembraneImpl *from, TensorMembraneImpl *to, 
                                   const char *symbol);

/* Element access and modification */
extern float membrane_get_element(TensorMembraneImpl *membrane, uint32_t *indices);
extern int membrane_set_element(TensorMembraneImpl *membrane, uint32_t *indices, float value);
extern int membrane_fill(TensorMembraneImpl *membrane, float value);

/* Utility Functions */
extern TensorMembraneImpl *find_membrane_by_id(uint32_t id);
extern void membrane_print_structure(TensorMembraneImpl *membrane, int depth);

/* Shell Command Integration Functions */
extern void *tensor_membrane_create_prime(int prime_factors[], int count);
extern void tensor_membrane_destroy_prime(void *membrane_ptr);
extern int tensor_membrane_add_object_prime(void *membrane_ptr, const char *symbol);
extern void tensor_membrane_print_prime(void *membrane_ptr);
extern uint32_t tensor_membrane_get_id_prime(void *membrane_ptr);
extern void *tensor_membrane_find_by_id_prime(uint32_t id);
extern int tensor_membrane_get_count_prime(void);

#endif /* TENSOR_MEMBRANE_H */
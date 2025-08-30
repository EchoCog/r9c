/* GGUF Model Format Support for rc Shell
 * Provides infrastructure for loading and working with GGUF models
 */

#ifndef GGUF_H
#define GGUF_H

#include <stdint.h>
#include <stddef.h>

/* GGUF file format constants */
#define GGUF_MAGIC 0x46554747  /* "GGUF" */
#define GGUF_VERSION 3

/* GGUF value types */
typedef enum {
    GGUF_TYPE_UINT8 = 0,
    GGUF_TYPE_INT8 = 1,
    GGUF_TYPE_UINT16 = 2,
    GGUF_TYPE_INT16 = 3,
    GGUF_TYPE_UINT32 = 4,
    GGUF_TYPE_INT32 = 5,
    GGUF_TYPE_FLOAT32 = 6,
    GGUF_TYPE_BOOL = 7,
    GGUF_TYPE_STRING = 8,
    GGUF_TYPE_ARRAY = 9,
    GGUF_TYPE_UINT64 = 10,
    GGUF_TYPE_INT64 = 11,
    GGUF_TYPE_FLOAT64 = 12,
    GGUF_TYPE_COUNT
} gguf_type;

/* GGUF tensor type */
typedef enum {
    GGML_TYPE_F32 = 0,
    GGML_TYPE_F16 = 1,
    GGML_TYPE_Q4_0 = 2,
    GGML_TYPE_Q4_1 = 3,
    GGML_TYPE_Q5_0 = 6,
    GGML_TYPE_Q5_1 = 7,
    GGML_TYPE_Q8_0 = 8,
    GGML_TYPE_Q8_1 = 9,
    GGML_TYPE_COUNT
} ggml_type;

/* GGUF structures */
typedef struct {
    char *data;
    size_t size;
} gguf_str;

typedef struct {
    gguf_type type;
    union {
        uint8_t uint8;
        int8_t int8;
        uint16_t uint16;
        int16_t int16;
        uint32_t uint32;
        int32_t int32;
        float float32;
        uint64_t uint64;
        int64_t int64;
        double float64;
        int bool_val;
        gguf_str str;
        struct {
            gguf_type type;
            uint64_t n;
            void *data;
        } arr;
    };
} gguf_value;

typedef struct {
    gguf_str key;
    gguf_value value;
} gguf_kv;

typedef struct {
    gguf_str name;
    uint32_t n_dims;
    uint64_t *ne;
    ggml_type type;
    uint64_t offset;
} gguf_tensor_info;

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint64_t n_tensors;
    uint64_t n_kv;
    gguf_kv *kv;
    gguf_tensor_info *infos;
    size_t alignment;
    size_t offset;
    void *data;
    size_t size;
} gguf_context;

/* GGUF functions */
extern gguf_context *gguf_init_from_file(const char *fname);
extern void gguf_free(gguf_context *ctx);
extern int gguf_find_key(gguf_context *ctx, const char *key);
extern gguf_value *gguf_get_value(gguf_context *ctx, int key_id);
extern const char *gguf_get_key(gguf_context *ctx, int key_id);
extern int gguf_get_n_tensors(gguf_context *ctx);
extern gguf_tensor_info *gguf_get_tensor_info(gguf_context *ctx, int i);
extern void *gguf_get_tensor_data(gguf_context *ctx, int i);

/* Model loading interface */
typedef struct {
    gguf_context *gguf_ctx;
    char *model_path;
    int n_layers;
    int n_embd;
    int n_vocab;
    void *vocab_data;
    int context_length;
} gguf_model;

extern gguf_model *gguf_load_model(const char *model_path);
extern void gguf_free_model(gguf_model *model);
extern int gguf_get_model_info(gguf_model *model, char **info);

#endif /* GGUF_H */
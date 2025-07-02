/* Example Cognitive Module
 * Demonstrates how to create and register cognitive modules
 */

#include "rc.h"
#include "cognitive.h"
#include <string.h>
#include <stdlib.h>

/* Example Pattern Recognition Module */
static int pattern_init(void) {
    return 0; /* Success */
}

static int pattern_process(const char *input, char **output) {
    if (!input || !output) return -1;
    
    /* Simple pattern recognition example */
    if (strstr(input, "hello")) {
        *output = ecpy("greeting_pattern_detected");
        return 0;
    } else if (strstr(input, "goodbye")) {
        *output = ecpy("farewell_pattern_detected");
        return 0;
    }
    
    *output = ecpy("no_pattern_detected");
    return 0;
}

static void pattern_cleanup(void) {
    /* Cleanup pattern recognition resources */
}

/* Module definition */
static CognitiveModule pattern_module = {
    .name = "pattern_recognition",
    .version = "1.0.0",
    .init = pattern_init,
    .process = pattern_process,
    .cleanup = pattern_cleanup,
    .next = NULL
};

/* Example Attention Allocation Module */
static int attention_init(void) {
    AttentionState *state = get_attention_state();
    state->total_attention = 100.0f;
    state->active_patterns = 1;
    state->timestamp = 12345;
    return 0;
}

static int attention_process(const char *input, char **output) {
    if (!input || !output) return -1;
    
    AttentionState *state = get_attention_state();
    
    /* Simulate attention allocation based on input complexity */
    size_t complexity = strlen(input);
    state->total_attention = (float)(complexity * 10);
    state->active_patterns = (int)(complexity / 5) + 1;
    
    *output = ecpy("attention_allocated");
    return 0;
}

static void attention_cleanup(void) {
    reset_attention_state();
}

static CognitiveModule attention_module = {
    .name = "attention_allocator",
    .version = "1.0.0", 
    .init = attention_init,
    .process = attention_process,
    .cleanup = attention_cleanup,
    .next = NULL
};

/* Shell command to load example modules */
void b_load_example_modules(char **av) {
    int result1 = register_cognitive_module(&pattern_module);
    int result2 = register_cognitive_module(&attention_module);
    
    if (result1 == 0 && result2 == 0) {
        fprint(1, "Example cognitive modules loaded successfully\n");
    } else {
        fprint(1, "Failed to load some cognitive modules\n");
    }
}

/* Shell command to test pattern recognition */
void b_test_pattern(char **av) {
    if (!av[1]) {
        rc_error("test-pattern: missing input argument");
        return;
    }
    
    CognitiveModule *module = find_cognitive_module("pattern_recognition");
    if (!module) {
        rc_error("test-pattern: pattern recognition module not loaded");
        return;
    }
    
    char *output = NULL;
    int result = module->process(av[1], &output);
    
    if (result == 0 && output) {
        fprint(1, "Pattern result: %s\n", output);
        efree(output);
    } else {
        fprint(1, "Pattern recognition failed\n");
    }
}

/* Shell command to test attention allocation */
void b_test_attention(char **av) {
    if (!av[1]) {
        rc_error("test-attention: missing input argument");
        return;
    }
    
    CognitiveModule *module = find_cognitive_module("attention_allocator");
    if (!module) {
        rc_error("test-attention: attention allocator module not loaded");
        return;
    }
    
    char *output = NULL;
    int result = module->process(av[1], &output);
    
    if (result == 0) {
        AttentionState *state = get_attention_state();
        fprint(1, "Attention allocated - Total: %d, Patterns: %d\n", 
               (int)(state->total_attention), state->active_patterns);
        if (output) {
            efree(output);
        }
    } else {
        fprint(1, "Attention allocation failed\n");
    }
}
/* Orchestrating Agent Implementation
 * Main coordination center with neural tree structure and pattern analysis
 */

#include "rc.h"
#include "or.h"
#include "cognitive.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

/* Global orchestrator registry */
static Orchestrator **orchestrators = NULL;
static int orchestrator_count = 0;
static pthread_mutex_t orchestrator_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Create orchestrator */
Orchestrator *orchestrator_create(const char *name, uint32_t agent_id) {
    if (!name) return NULL;
    
    Orchestrator *orc = malloc(sizeof(Orchestrator));
    if (!orc) return NULL;
    
    orc->agent_id = agent_id;
    orc->name = strdup(name);
    orc->neural_tree = NULL;
    orc->pattern_state = pattern_analysis_create();
    orc->resonance_state = resonance_create();
    orc->attention_state = malloc(sizeof(AttentionState));
    orc->inference_engine = NULL;
    orc->thread_count = 0;
    orc->is_active = 0;
    orc->last_update = time(NULL);
    
    if (!orc->pattern_state || !orc->resonance_state || !orc->attention_state) {
        orchestrator_destroy(orc);
        return NULL;
    }
    
    /* Initialize attention state */
    memset(orc->attention_state, 0, sizeof(AttentionState));
    
    /* Register orchestrator */
    pthread_mutex_lock(&orchestrator_mutex);
    orchestrators = realloc(orchestrators, (orchestrator_count + 1) * sizeof(Orchestrator*));
    if (orchestrators) {
        orchestrators[orchestrator_count++] = orc;
    }
    pthread_mutex_unlock(&orchestrator_mutex);
    
    return orc;
}

/* Destroy orchestrator */
void orchestrator_destroy(Orchestrator *orc) {
    if (!orc) return;
    
    /* Remove from registry */
    pthread_mutex_lock(&orchestrator_mutex);
    for (int i = 0; i < orchestrator_count; i++) {
        if (orchestrators[i] == orc) {
            memmove(&orchestrators[i], &orchestrators[i+1], 
                   (orchestrator_count - i - 1) * sizeof(Orchestrator*));
            orchestrator_count--;
            break;
        }
    }
    pthread_mutex_unlock(&orchestrator_mutex);
    
    if (orc->name) free(orc->name);
    if (orc->neural_tree) neural_node_destroy(orc->neural_tree);
    if (orc->pattern_state) pattern_analysis_destroy(orc->pattern_state);
    if (orc->resonance_state) resonance_destroy(orc->resonance_state);
    if (orc->attention_state) free(orc->attention_state);
    if (orc->inference_engine) gguf_free_model((gguf_model*)orc->inference_engine);
    
    free(orc);
}

/* Initialize orchestrator */
int orchestrator_init(Orchestrator *orc) {
    if (!orc) return -1;
    
    /* Create root neural node */
    orc->neural_tree = neural_node_create("root", NULL);
    if (!orc->neural_tree) return -1;
    
    /* Initialize pattern analysis */
    pattern_analysis_update(orc->pattern_state, "initialization");
    
    return 0;
}

/* Start orchestrator */
int orchestrator_start(Orchestrator *orc) {
    if (!orc) return -1;
    
    orc->is_active = 1;
    orc->last_update = time(NULL);
    
    fprint(1, "orchestrator: started %s (id: %d)\n", orc->name, (int)orc->agent_id);
    return 0;
}

/* Stop orchestrator */
int orchestrator_stop(Orchestrator *orc) {
    if (!orc) return -1;
    
    orc->is_active = 0;
    
    fprint(1, "orchestrator: stopped %s\n", orc->name);
    return 0;
}

/* Update orchestrator state */
int orchestrator_update(Orchestrator *orc) {
    if (!orc) return -1;
    
    orc->last_update = time(NULL);
    
    /* Update neural tree resonance */
    if (orc->neural_tree) {
        neural_tree_update_resonance(orc->neural_tree);
    }
    
    /* Update pattern analysis */
    if (orc->pattern_state) {
        pattern_calculate_resonance_depth(orc->pattern_state);
        pattern_calculate_temporal_coherence(orc->pattern_state);
        pattern_calculate_spatial_distribution(orc->pattern_state);
    }
    
    /* Update resonance state */
    if (orc->resonance_state && orc->neural_tree) {
        resonance_analyze(orc->resonance_state, orc->neural_tree);
    }
    
    return 0;
}

/* Create neural node */
NeuralNode *neural_node_create(const char *pattern_type, NeuralNode *parent) {
    if (!pattern_type) return NULL;
    
    NeuralNode *node = malloc(sizeof(NeuralNode));
    if (!node) return NULL;
    
    static uint32_t next_id = 1;
    node->node_id = next_id++;
    node->pattern_type = strdup(pattern_type);
    node->activation_level = 0.0f;
    node->resonance_frequency = 1.0f;
    node->truth_value.strength = 0.5f;
    node->truth_value.confidence = 0.5f;
    node->ecan_values.short_term_importance = 0.0f;
    node->ecan_values.long_term_importance = 0.0f;
    node->ecan_values.very_long_term_importance = 0.0f;
    node->ecan_values.stimulation_level = 0.0f;
    node->children = NULL;
    node->child_count = 0;
    node->parent = parent;
    node->pattern_data = NULL;
    node->created = time(NULL);
    node->last_accessed = time(NULL);
    
    return node;
}

/* Destroy neural node */
void neural_node_destroy(NeuralNode *node) {
    if (!node) return;
    
    /* Destroy children recursively */
    for (int i = 0; i < node->child_count; i++) {
        neural_node_destroy(node->children[i]);
    }
    
    if (node->pattern_type) free(node->pattern_type);
    if (node->children) free(node->children);
    if (node->pattern_data) free(node->pattern_data);
    
    free(node);
}

/* Propagate activation through neural tree */
int neural_tree_propagate(NeuralNode *root, float activation) {
    if (!root) return -1;
    
    root->activation_level = activation;
    root->last_accessed = time(NULL);
    
    /* Propagate to children with decay */
    float decay_factor = 0.8f;
    for (int i = 0; i < root->child_count; i++) {
        neural_tree_propagate(root->children[i], activation * decay_factor);
    }
    
    return 0;
}

/* Update resonance in neural tree */
int neural_tree_update_resonance(NeuralNode *root) {
    if (!root) return -1;
    
    /* Update resonance frequency based on activation */
    root->resonance_frequency = 1.0f + root->activation_level;
    
    /* Update children */
    for (int i = 0; i < root->child_count; i++) {
        neural_tree_update_resonance(root->children[i]);
    }
    
    return 0;
}

/* Find pattern in neural tree */
NeuralNode *neural_tree_find_pattern(NeuralNode *root, const char *pattern) {
    if (!root || !pattern) return NULL;
    
    if (root->pattern_type && strcmp(root->pattern_type, pattern) == 0) {
        return root;
    }
    
    /* Search children */
    for (int i = 0; i < root->child_count; i++) {
        NeuralNode *found = neural_tree_find_pattern(root->children[i], pattern);
        if (found) return found;
    }
    
    return NULL;
}

/* Create pattern analysis */
PatternAnalysis *pattern_analysis_create(void) {
    PatternAnalysis *analysis = malloc(sizeof(PatternAnalysis));
    if (!analysis) return NULL;
    
    analysis->resonance_depth = 0.0f;
    analysis->temporal_coherence = 0.0f;
    analysis->spatial_distribution = 0.0f;
    analysis->pattern_count = 0;
    analysis->active_patterns = NULL;
    analysis->pattern_weights = NULL;
    analysis->analysis_time = time(NULL);
    
    return analysis;
}

/* Destroy pattern analysis */
void pattern_analysis_destroy(PatternAnalysis *analysis) {
    if (!analysis) return;
    
    if (analysis->active_patterns) {
        for (int i = 0; i < analysis->pattern_count; i++) {
            if (analysis->active_patterns[i]) free(analysis->active_patterns[i]);
        }
        free(analysis->active_patterns);
    }
    if (analysis->pattern_weights) free(analysis->pattern_weights);
    
    free(analysis);
}

/* Update pattern analysis */
int pattern_analysis_update(PatternAnalysis *analysis, const char *input) {
    if (!analysis || !input) return -1;
    
    analysis->analysis_time = time(NULL);
    
    /* Simple pattern counting based on input length and complexity */
    int input_len = strlen(input);
    analysis->pattern_count = 1 + (input_len / 10);
    
    /* Update pattern arrays */
    analysis->active_patterns = realloc(analysis->active_patterns, 
                                      analysis->pattern_count * sizeof(char*));
    analysis->pattern_weights = realloc(analysis->pattern_weights,
                                      analysis->pattern_count * sizeof(float));
    
    if (!analysis->active_patterns || !analysis->pattern_weights) return -1;
    
    /* Add the input as a pattern */
    analysis->active_patterns[0] = strdup(input);
    analysis->pattern_weights[0] = 1.0f;
    
    return 0;
}

/* Calculate resonance depth */
float pattern_calculate_resonance_depth(PatternAnalysis *analysis) {
    if (!analysis) return 0.0f;
    
    /* Simple calculation based on pattern count and weights */
    float depth = 0.0f;
    for (int i = 0; i < analysis->pattern_count; i++) {
        depth += analysis->pattern_weights[i] * 0.1f;
    }
    
    analysis->resonance_depth = depth;
    return depth;
}

/* Calculate temporal coherence */
float pattern_calculate_temporal_coherence(PatternAnalysis *analysis) {
    if (!analysis) return 0.0f;
    
    /* Time-based coherence calculation */
    time_t now = time(NULL);
    float coherence = 1.0f / (1.0f + (now - analysis->analysis_time));
    
    analysis->temporal_coherence = coherence;
    return coherence;
}

/* Calculate spatial distribution */
float pattern_calculate_spatial_distribution(PatternAnalysis *analysis) {
    if (!analysis) return 0.0f;
    
    /* Simple spatial distribution based on pattern count */
    float distribution = (float)analysis->pattern_count / 10.0f;
    if (distribution > 1.0f) distribution = 1.0f;
    
    analysis->spatial_distribution = distribution;
    return distribution;
}

/* Create resonance depth */
ResonanceDepth *resonance_create(void) {
    ResonanceDepth *resonance = malloc(sizeof(ResonanceDepth));
    if (!resonance) return NULL;
    
    resonance->depth_level = 0.0f;
    resonance->coherence_factor = 1.0f;
    resonance->stability_measure = 0.5f;
    resonance->resonance_nodes = 0;
    resonance->frequency_spectrum = NULL;
    resonance->spectrum_size = 0;
    
    return resonance;
}

/* Destroy resonance depth */
void resonance_destroy(ResonanceDepth *resonance) {
    if (!resonance) return;
    
    if (resonance->frequency_spectrum) free(resonance->frequency_spectrum);
    free(resonance);
}

/* Analyze resonance */
int resonance_analyze(ResonanceDepth *resonance, NeuralNode *tree) {
    if (!resonance || !tree) return -1;
    
    /* Count nodes and calculate average resonance */
    int node_count = 0;
    float total_resonance = 0.0f;
    
    /* Simple recursive count (would need proper tree traversal) */
    node_count++;
    total_resonance += tree->resonance_frequency;
    
    resonance->resonance_nodes = node_count;
    resonance->depth_level = total_resonance / node_count;
    
    /* Update stability based on resonance consistency */
    if (resonance->depth_level > 0.5f) {
        resonance->stability_measure = 0.8f;
        resonance->coherence_factor = 1.2f;
    } else {
        resonance->stability_measure = 0.3f;
        resonance->coherence_factor = 0.8f;
    }
    
    return 0;
}

/* Get stability measure */
float resonance_get_stability(ResonanceDepth *resonance) {
    if (!resonance) return 0.0f;
    return resonance->stability_measure;
}

/* Load model into orchestrator */
int orchestrator_load_model(Orchestrator *orc, const char *model_path) {
    if (!orc || !model_path) return -1;
    
    /* Free existing model */
    if (orc->inference_engine) {
        gguf_free_model((gguf_model*)orc->inference_engine);
        orc->inference_engine = NULL;
    }
    
    /* Load new model */
    gguf_model *model = gguf_load_model(model_path);
    if (!model) {
        fprint(2, "orchestrator: failed to load model %s\n", model_path);
        return -1;
    }
    
    orc->inference_engine = model;
    fprint(1, "orchestrator: loaded model %s into %s\n", model_path, orc->name);
    
    return 0;
}

/* Perform inference */
int orchestrator_inference(Orchestrator *orc, const char *prompt, char **response) {
    if (!orc || !prompt || !response) return -1;
    
    if (!orc->inference_engine) {
        *response = strdup("No model loaded");
        return -1;
    }
    
    /* Simple inference simulation - in real implementation would use llama.cpp */
    char *result = malloc(512);
    if (!result) return -1;
    
    snprintf(result, 512, "Inference response to: \"%s\" (simulated from %s)", 
             prompt, orc->name);
    
    *response = result;
    
    /* Update neural tree with inference activity */
    if (orc->neural_tree) {
        neural_tree_propagate(orc->neural_tree, 0.8f);
    }
    
    /* Update pattern analysis */
    pattern_analysis_update(orc->pattern_state, prompt);
    
    return 0;
}

/* Set context for inference */
int orchestrator_set_context(Orchestrator *orc, const char *context) {
    if (!orc || !context) return -1;
    
    /* In real implementation, would update model context */
    fprint(1, "orchestrator: set context for %s: %.50s...\n", orc->name, context);
    
    return 0;
}

/* Shell commands */
void b_orchestrator_create(char **av) {
    if (!av[1]) {
        rc_error("orchestrator-create: missing name argument");
        return;
    }
    
    uint32_t agent_id = (uint32_t)time(NULL);
    Orchestrator *orc = orchestrator_create(av[1], agent_id);
    
    if (orc) {
        orchestrator_init(orc);
        orchestrator_start(orc);
        fprint(1, "Created orchestrator: %s (id: %d)\n", av[1], (int)agent_id);
    } else {
        rc_error("orchestrator-create: failed to create orchestrator");
    }
}

void b_orchestrator_status(char **av) {
    pthread_mutex_lock(&orchestrator_mutex);
    
    fprint(1, "Orchestrator Status:\n");
    fprint(1, "Active orchestrators: %d\n", orchestrator_count);
    
    for (int i = 0; i < orchestrator_count; i++) {
        Orchestrator *orc = orchestrators[i];
        fprint(1, "  %s (id: %d) - %s, threads: %d\n",
               orc->name, (int)orc->agent_id, 
               orc->is_active ? "active" : "inactive",
               orc->thread_count);
        
        if (orc->pattern_state) {
            fprint(1, "    Patterns: %d, Resonance: %.2f, Coherence: %.2f\n",
                   orc->pattern_state->pattern_count,
                   orc->pattern_state->resonance_depth,
                   orc->pattern_state->temporal_coherence);
        }
        
        if (orc->inference_engine) {
            gguf_model *model = (gguf_model*)orc->inference_engine;
            fprint(1, "    Model: %s\n", model->model_path);
        }
    }
    
    pthread_mutex_unlock(&orchestrator_mutex);
}

void b_orchestrator_load_model(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("orchestrator-load-model: usage: orchestrator-load-model <name> <model_path>");
        return;
    }
    
    /* Find orchestrator */
    Orchestrator *orc = NULL;
    pthread_mutex_lock(&orchestrator_mutex);
    for (int i = 0; i < orchestrator_count; i++) {
        if (strcmp(orchestrators[i]->name, av[1]) == 0) {
            orc = orchestrators[i];
            break;
        }
    }
    pthread_mutex_unlock(&orchestrator_mutex);
    
    if (!orc) {
        rc_error("orchestrator-load-model: orchestrator not found");
        return;
    }
    
    if (orchestrator_load_model(orc, av[2]) == 0) {
        fprint(1, "Model loaded successfully\n");
    } else {
        rc_error("orchestrator-load-model: failed to load model");
    }
}

void b_orchestrator_inference(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("orchestrator-inference: usage: orchestrator-inference <name> <prompt>");
        return;
    }
    
    /* Find orchestrator */
    Orchestrator *orc = NULL;
    pthread_mutex_lock(&orchestrator_mutex);
    for (int i = 0; i < orchestrator_count; i++) {
        if (strcmp(orchestrators[i]->name, av[1]) == 0) {
            orc = orchestrators[i];
            break;
        }
    }
    pthread_mutex_unlock(&orchestrator_mutex);
    
    if (!orc) {
        rc_error("orchestrator-inference: orchestrator not found");
        return;
    }
    
    char *response = NULL;
    if (orchestrator_inference(orc, av[2], &response) == 0 && response) {
        fprint(1, "%s\n", response);
        free(response);
    } else {
        rc_error("orchestrator-inference: inference failed");
    }
}
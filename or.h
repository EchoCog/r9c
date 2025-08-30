/* Orchestrating Agent for Cognitive rc Shell
 * Main coordination center with neural tree structure and pattern analysis
 */

#ifndef OR_H
#define OR_H

#include <stdint.h>
#include <time.h>
#include "cognitive.h"
#include "gguf.h"

/* Neural tree structure types */
typedef struct NeuralNode NeuralNode;
typedef struct PatternAnalysis PatternAnalysis;
typedef struct ResonanceDepth ResonanceDepth;

/* Orchestrator class for main coordination */
typedef struct {
    uint32_t agent_id;
    char *name;
    NeuralNode *neural_tree;
    PatternAnalysis *pattern_state;
    ResonanceDepth *resonance_state;
    AttentionState *attention_state;
    void *inference_engine;  /* Will hold gguf_model */
    int thread_count;
    int is_active;
    time_t last_update;
} Orchestrator;

/* Neural tree node structure */
struct NeuralNode {
    uint32_t node_id;
    char *pattern_type;
    float activation_level;
    float resonance_frequency;
    TruthValue truth_value;
    ECANValues ecan_values;
    NeuralNode **children;
    int child_count;
    NeuralNode *parent;
    void *pattern_data;
    time_t created;
    time_t last_accessed;
};

/* Pattern analysis structure */
struct PatternAnalysis {
    float resonance_depth;
    float temporal_coherence;
    float spatial_distribution;
    int pattern_count;
    char **active_patterns;
    float *pattern_weights;
    time_t analysis_time;
};

/* Resonance depth analysis */
struct ResonanceDepth {
    float depth_level;
    float coherence_factor;
    float stability_measure;
    int resonance_nodes;
    float *frequency_spectrum;
    int spectrum_size;
};

/* Orchestrator functions */
extern Orchestrator *orchestrator_create(const char *name, uint32_t agent_id);
extern void orchestrator_destroy(Orchestrator *orc);
extern int orchestrator_init(Orchestrator *orc);
extern int orchestrator_start(Orchestrator *orc);
extern int orchestrator_stop(Orchestrator *orc);
extern int orchestrator_update(Orchestrator *orc);

/* Neural tree operations */
extern NeuralNode *neural_node_create(const char *pattern_type, NeuralNode *parent);
extern void neural_node_destroy(NeuralNode *node);
extern int neural_tree_propagate(NeuralNode *root, float activation);
extern int neural_tree_update_resonance(NeuralNode *root);
extern NeuralNode *neural_tree_find_pattern(NeuralNode *root, const char *pattern);

/* Pattern analysis functions */
extern PatternAnalysis *pattern_analysis_create(void);
extern void pattern_analysis_destroy(PatternAnalysis *analysis);
extern int pattern_analysis_update(PatternAnalysis *analysis, const char *input);
extern float pattern_calculate_resonance_depth(PatternAnalysis *analysis);
extern float pattern_calculate_temporal_coherence(PatternAnalysis *analysis);
extern float pattern_calculate_spatial_distribution(PatternAnalysis *analysis);

/* Resonance functions */
extern ResonanceDepth *resonance_create(void);
extern void resonance_destroy(ResonanceDepth *resonance);
extern int resonance_analyze(ResonanceDepth *resonance, NeuralNode *tree);
extern float resonance_get_stability(ResonanceDepth *resonance);

/* Inference integration */
extern int orchestrator_load_model(Orchestrator *orc, const char *model_path);
extern int orchestrator_inference(Orchestrator *orc, const char *prompt, char **response);
extern int orchestrator_set_context(Orchestrator *orc, const char *context);

/* Multi-threaded execution */
extern int orchestrator_start_workers(Orchestrator *orc, int thread_count);
extern int orchestrator_stop_workers(Orchestrator *orc);
extern int orchestrator_dispatch_task(Orchestrator *orc, const char *task, void *data);

/* Coordination functions */
extern int orchestrator_coordinate_agents(Orchestrator *primary, Orchestrator **agents, int count);
extern int orchestrator_sync_attention(Orchestrator *orc, AttentionState *global_state);
extern int orchestrator_sync_patterns(Orchestrator *orc, PatternAnalysis *global_analysis);

/* Shell command interface */
extern void b_orchestrator_create(char **av);
extern void b_orchestrator_status(char **av);
extern void b_orchestrator_load_model(char **av);
extern void b_orchestrator_inference(char **av);
extern void b_neural_tree_show(char **av);
extern void b_pattern_analysis(char **av);

#endif /* OR_H */
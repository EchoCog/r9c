/* AI Chatbot Interface for rc Shell
 * Shell terminal interface with integrated AI and session management
 */

#ifndef AIR_H
#define AIR_H

#include <stdint.h>
#include <time.h>
#include "cognitive.h"
#include "gguf.h"
#include "or.h"

/* Chat session types */
typedef struct ChatSession ChatSession;
typedef struct SessionState SessionState;
typedef struct MessageHistory MessageHistory;

/* AI Chat session structure */
struct ChatSession {
    uint32_t session_id;
    char *session_name;
    char *model_path;
    gguf_model *model;
    Orchestrator *orchestrator;
    SessionState *state;
    MessageHistory *history;
    int max_tokens;
    float temperature;
    float top_p;
    int context_length;
    time_t created;
    time_t last_used;
    int is_active;
};

/* Session state for persistence */
struct SessionState {
    float *context_embeddings;
    int context_size;
    char *system_prompt;
    char *conversation_context;
    AttentionState attention;
    PatternAnalysis *patterns;
    uint64_t message_count;
    time_t evolution_timestamp;
};

/* Message history structure */
struct MessageHistory {
    struct {
        char *role;      /* "user", "assistant", "system" */
        char *content;
        time_t timestamp;
        float confidence;
        TruthValue truth_value;
    } *messages;
    int count;
    int capacity;
    size_t total_tokens;
};

/* WebSocket support for live interactions */
typedef struct {
    int socket_fd;
    uint16_t port;
    int is_listening;
    char *websocket_key;
    int client_count;
    int *client_sockets;
} WebSocketServer;

/* Spatial journey and analysis structures */
typedef struct {
    float *spatial_coordinates;
    int dimensions;
    float journey_distance;
    char *trajectory_description;
    PatternAnalysis *spatial_patterns;
} SpatialJourney;

/* AI Chatbot interface functions */
extern ChatSession *airchat_create_session(const char *session_name, const char *model_path);
extern void airchat_destroy_session(ChatSession *session);
extern int airchat_load_model(ChatSession *session, const char *model_path);
extern int airchat_send_message(ChatSession *session, const char *message, char **response);
extern int airchat_set_system_prompt(ChatSession *session, const char *prompt);
extern int airchat_save_session(ChatSession *session, const char *filepath);
extern int airchat_load_session(const char *filepath, ChatSession **session);

/* Session management */
extern int airchat_list_sessions(char ***session_names, int *count);
extern ChatSession *airchat_get_session(const char *name);
extern int airchat_switch_session(const char *name);
extern int airchat_delete_session(const char *name);

/* Message history operations */
extern int airchat_add_message(ChatSession *session, const char *role, const char *content);
extern int airchat_get_history(ChatSession *session, char **history_text);
extern int airchat_clear_history(ChatSession *session);
extern int airchat_export_history(ChatSession *session, const char *filepath);

/* WebSocket server functions */
extern WebSocketServer *websocket_create_server(uint16_t port);
extern void websocket_destroy_server(WebSocketServer *server);
extern int websocket_start_listening(WebSocketServer *server);
extern int websocket_stop_listening(WebSocketServer *server);
extern int websocket_broadcast_message(WebSocketServer *server, const char *message);
extern int websocket_send_to_client(WebSocketServer *server, int client_id, const char *message);

/* Spatial journey and analysis */
extern SpatialJourney *spatial_journey_create(int dimensions);
extern void spatial_journey_destroy(SpatialJourney *journey);
extern int spatial_journey_update(SpatialJourney *journey, float *coordinates);
extern int spatial_journey_analyze(SpatialJourney *journey, char **analysis);

/* Multi-namespace session capabilities */
extern int airchat_create_namespace(const char *namespace_name);
extern int airchat_switch_namespace(const char *namespace_name);
extern int airchat_list_namespaces(char ***namespaces, int *count);

/* Integration with orchestrator */
extern int airchat_set_orchestrator(ChatSession *session, Orchestrator *orc);
extern int airchat_sync_with_orchestrator(ChatSession *session);

/* Real-time coordination */
extern int airchat_start_coordination_thread(ChatSession *session);
extern int airchat_stop_coordination_thread(ChatSession *session);

/* Shell command interface */
extern void b_airchat_create(char **av);
extern void b_airchat_load(char **av);
extern void b_airchat_chat(char **av);
extern void b_airchat_list(char **av);
extern void b_airchat_switch(char **av);
extern void b_airchat_save(char **av);
extern void b_airchat_history(char **av);
extern void b_airchat_clear(char **av);
extern void b_airchat_websocket_start(char **av);
extern void b_airchat_websocket_stop(char **av);
extern void b_airchat_status(char **av);

/* Advanced features similar to aichat */
extern int airchat_function_call(ChatSession *session, const char *function_name, 
                               const char *parameters, char **result);
extern int airchat_tool_use(ChatSession *session, const char *tool_name, 
                          const char *input, char **output);
extern int airchat_code_execution(ChatSession *session, const char *code, 
                                const char *language, char **result);

#endif /* AIR_H */
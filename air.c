/* AI Chatbot Interface Implementation
 * Shell terminal interface with integrated AI and session management
 */

#include "rc.h" 
#include "air.h"
#include "or.h"
#include "gguf.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

/* Global session registry */
static ChatSession **sessions = NULL;
static int session_count = 0;
static ChatSession *current_session = NULL;
static pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER;

/* WebSocket server global */
static WebSocketServer *global_websocket_server = NULL;

/* Create chat session */
ChatSession *airchat_create_session(const char *session_name, const char *model_path) {
    if (!session_name) return NULL;
    
    ChatSession *session = malloc(sizeof(ChatSession));
    if (!session) return NULL;
    
    static uint32_t next_id = 1;
    session->session_id = next_id++;
    session->session_name = strdup(session_name);
    session->model_path = model_path ? strdup(model_path) : NULL;
    session->model = NULL;
    session->orchestrator = NULL;
    session->state = malloc(sizeof(SessionState));
    session->history = malloc(sizeof(MessageHistory));
    session->max_tokens = 2048;
    session->temperature = 0.7f;
    session->top_p = 0.9f;
    session->context_length = 2048;
    session->created = time(NULL);
    session->last_used = time(NULL);
    session->is_active = 0;
    
    if (!session->state || !session->history) {
        airchat_destroy_session(session);
        return NULL;
    }
    
    /* Initialize session state */
    memset(session->state, 0, sizeof(SessionState));
    session->state->system_prompt = strdup("You are a helpful AI assistant integrated into the rc shell.");
    session->state->conversation_context = strdup("");
    session->state->evolution_timestamp = time(NULL);
    
    /* Initialize message history */
    session->history->messages = NULL;
    session->history->count = 0;
    session->history->capacity = 0;
    session->history->total_tokens = 0;
    
    /* Load model if provided */
    if (model_path) {
        airchat_load_model(session, model_path);
    }
    
    /* Register session */
    pthread_mutex_lock(&session_mutex);
    sessions = realloc(sessions, (session_count + 1) * sizeof(ChatSession*));
    if (sessions) {
        sessions[session_count++] = session;
    }
    pthread_mutex_unlock(&session_mutex);
    
    return session;
}

/* Destroy chat session */
void airchat_destroy_session(ChatSession *session) {
    if (!session) return;
    
    /* Remove from registry */
    pthread_mutex_lock(&session_mutex);
    for (int i = 0; i < session_count; i++) {
        if (sessions[i] == session) {
            memmove(&sessions[i], &sessions[i+1], 
                   (session_count - i - 1) * sizeof(ChatSession*));
            session_count--;
            break;
        }
    }
    if (current_session == session) {
        current_session = NULL;
    }
    pthread_mutex_unlock(&session_mutex);
    
    if (session->session_name) free(session->session_name);
    if (session->model_path) free(session->model_path);
    if (session->model) gguf_free_model(session->model);
    
    if (session->state) {
        if (session->state->context_embeddings) free(session->state->context_embeddings);
        if (session->state->system_prompt) free(session->state->system_prompt);
        if (session->state->conversation_context) free(session->state->conversation_context);
        free(session->state);
    }
    
    if (session->history) {
        for (int i = 0; i < session->history->count; i++) {
            if (session->history->messages[i].role) free(session->history->messages[i].role);
            if (session->history->messages[i].content) free(session->history->messages[i].content);
        }
        if (session->history->messages) free(session->history->messages);
        free(session->history);
    }
    
    free(session);
}

/* Load model into session */
int airchat_load_model(ChatSession *session, const char *model_path) {
    if (!session || !model_path) return -1;
    
    /* Free existing model */
    if (session->model) {
        gguf_free_model(session->model);
        session->model = NULL;
    }
    
    /* Update model path */
    if (session->model_path) free(session->model_path);
    session->model_path = strdup(model_path);
    
    /* Load new model */
    session->model = gguf_load_model(model_path);
    if (!session->model) {
        fprint(2, "airchat: failed to load model %s\n", model_path);
        return -1;
    }
    
    fprint(1, "airchat: loaded model %s into session %s\n", model_path, session->session_name);
    return 0;
}

/* Send message and get response */
int airchat_send_message(ChatSession *session, const char *message, char **response) {
    if (!session || !message || !response) return -1;
    
    session->last_used = time(NULL);
    
    /* Add user message to history */
    airchat_add_message(session, "user", message);
    
    /* Generate response */
    char *ai_response = NULL;
    
    if (session->model) {
        /* Simple response generation - in real implementation would use llama.cpp */
        ai_response = malloc(512);
        if (ai_response) {
            snprintf(ai_response, 512, 
                "AI Response (from %s): I understand you said \"%s\". How can I help you further?",
                session->session_name, message);
        }
    } else {
        ai_response = strdup("No model loaded. Please load a model first with airchat-load command.");
    }
    
    if (!ai_response) return -1;
    
    /* Add AI response to history */
    airchat_add_message(session, "assistant", ai_response);
    
    /* Update conversation context */
    if (session->state->conversation_context) {
        char *new_context = malloc(strlen(session->state->conversation_context) + strlen(message) + strlen(ai_response) + 100);
        if (new_context) {
            sprintf(new_context, "%sUser: %s\nAssistant: %s\n", 
                   session->state->conversation_context, message, ai_response);
            free(session->state->conversation_context);
            session->state->conversation_context = new_context;
        }
    }
    
    *response = ai_response;
    return 0;
}

/* Set system prompt */
int airchat_set_system_prompt(ChatSession *session, const char *prompt) {
    if (!session || !prompt) return -1;
    
    if (session->state->system_prompt) free(session->state->system_prompt);
    session->state->system_prompt = strdup(prompt);
    
    return 0;
}

/* Add message to history */
int airchat_add_message(ChatSession *session, const char *role, const char *content) {
    if (!session || !role || !content) return -1;
    
    /* Expand history if needed */
    if (session->history->count >= session->history->capacity) {
        session->history->capacity = session->history->capacity ? session->history->capacity * 2 : 10;
        session->history->messages = realloc(session->history->messages,
            session->history->capacity * sizeof(session->history->messages[0]));
        if (!session->history->messages) return -1;
    }
    
    /* Add message */
    int idx = session->history->count++;
    session->history->messages[idx].role = strdup(role);
    session->history->messages[idx].content = strdup(content);
    session->history->messages[idx].timestamp = time(NULL);
    session->history->messages[idx].confidence = 0.8f;
    session->history->messages[idx].truth_value.strength = 0.7f;
    session->history->messages[idx].truth_value.confidence = 0.6f;
    
    /* Update token count (simple approximation) */
    session->history->total_tokens += strlen(content) / 4;
    
    return 0;
}

/* Get session by name */
ChatSession *airchat_get_session(const char *name) {
    if (!name) return NULL;
    
    pthread_mutex_lock(&session_mutex);
    for (int i = 0; i < session_count; i++) {
        if (strcmp(sessions[i]->session_name, name) == 0) {
            pthread_mutex_unlock(&session_mutex);
            return sessions[i];
        }
    }
    pthread_mutex_unlock(&session_mutex);
    
    return NULL;
}

/* Switch to session */
int airchat_switch_session(const char *name) {
    ChatSession *session = airchat_get_session(name);
    if (!session) return -1;
    
    current_session = session;
    session->is_active = 1;
    session->last_used = time(NULL);
    
    return 0;
}

/* List sessions */
int airchat_list_sessions(char ***session_names, int *count) {
    if (!session_names || !count) return -1;
    
    pthread_mutex_lock(&session_mutex);
    
    *count = session_count;
    if (session_count == 0) {
        *session_names = NULL;
        pthread_mutex_unlock(&session_mutex);
        return 0;
    }
    
    *session_names = malloc(session_count * sizeof(char*));
    if (!*session_names) {
        pthread_mutex_unlock(&session_mutex);
        return -1;
    }
    
    for (int i = 0; i < session_count; i++) {
        (*session_names)[i] = strdup(sessions[i]->session_name);
    }
    
    pthread_mutex_unlock(&session_mutex);
    return 0;
}

/* Create WebSocket server */
WebSocketServer *websocket_create_server(uint16_t port) {
    WebSocketServer *server = malloc(sizeof(WebSocketServer));
    if (!server) return NULL;
    
    server->socket_fd = -1;
    server->port = port;
    server->is_listening = 0;
    server->websocket_key = NULL;
    server->client_count = 0;
    server->client_sockets = NULL;
    
    return server;
}

/* Destroy WebSocket server */
void websocket_destroy_server(WebSocketServer *server) {
    if (!server) return;
    
    websocket_stop_listening(server);
    
    if (server->websocket_key) free(server->websocket_key);
    if (server->client_sockets) free(server->client_sockets);
    
    free(server);
}

/* Start WebSocket listening */
int websocket_start_listening(WebSocketServer *server) {
    if (!server) return -1;
    
    /* Create socket */
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd < 0) return -1;
    
    /* Set socket options */
    int opt = 1;
    setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    /* Bind to port */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(server->port);
    
    if (bind(server->socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server->socket_fd);
        server->socket_fd = -1;
        return -1;
    }
    
    /* Start listening */
    if (listen(server->socket_fd, 5) < 0) {
        close(server->socket_fd);
        server->socket_fd = -1;
        return -1;
    }
    
    server->is_listening = 1;
    fprint(1, "websocket: started listening on port %d\n", server->port);
    
    return 0;
}

/* Stop WebSocket listening */
int websocket_stop_listening(WebSocketServer *server) {
    if (!server) return -1;
    
    if (server->socket_fd >= 0) {
        close(server->socket_fd);
        server->socket_fd = -1;
    }
    
    /* Close client connections */
    for (int i = 0; i < server->client_count; i++) {
        if (server->client_sockets[i] >= 0) {
            close(server->client_sockets[i]);
        }
    }
    server->client_count = 0;
    
    server->is_listening = 0;
    fprint(1, "websocket: stopped listening\n");
    
    return 0;
}

/* Shell Commands */
void b_airchat_create(char **av) {
    if (!av[1]) {
        rc_error("airchat-create: usage: airchat-create <session_name> [model_path]");
        return;
    }
    
    ChatSession *session = airchat_create_session(av[1], av[2]);
    if (session) {
        fprint(1, "Created airchat session: %s (id: %d)\n", av[1], (int)session->session_id);
        current_session = session;
        session->is_active = 1;
    } else {
        rc_error("airchat-create: failed to create session");
    }
}

void b_airchat_load(char **av) {
    if (!av[1] || !av[2]) {
        rc_error("airchat-load: usage: airchat-load <session_name> <model_path>");
        return;
    }
    
    ChatSession *session = airchat_get_session(av[1]);
    if (!session) {
        rc_error("airchat-load: session not found");
        return;
    }
    
    if (airchat_load_model(session, av[2]) == 0) {
        fprint(1, "Model loaded into session %s\n", av[1]);
    } else {
        rc_error("airchat-load: failed to load model");
    }
}

void b_airchat_chat(char **av) {
    if (!av[1]) {
        rc_error("airchat-chat: usage: airchat-chat <message>");
        return;
    }
    
    if (!current_session) {
        rc_error("airchat-chat: no active session. Create one with airchat-create");
        return;
    }
    
    char *response = NULL;
    if (airchat_send_message(current_session, av[1], &response) == 0 && response) {
        fprint(1, "%s\n", response);
        free(response);
    } else {
        rc_error("airchat-chat: failed to get response");
    }
}

void b_airchat_list(char **av) {
    char **names = NULL;
    int count = 0;
    
    if (airchat_list_sessions(&names, &count) == 0) {
        fprint(1, "Chat Sessions (%d):\n", count);
        for (int i = 0; i < count; i++) {
            ChatSession *session = airchat_get_session(names[i]);
            char status = ' ';
            if (session == current_session) status = '*';
            else if (session && session->is_active) status = '+';
            
            fprint(1, "  %c %s", status, names[i]);
            if (session) {
                if (session->model_path) {
                    fprint(1, " (model: %s)", session->model_path);
                }
                fprint(1, " [%d messages]", session->history->count);
            }
            fprint(1, "\n");
            
            free(names[i]);
        }
        free(names);
    } else {
        rc_error("airchat-list: failed to list sessions");
    }
}

void b_airchat_switch(char **av) {
    if (!av[1]) {
        rc_error("airchat-switch: usage: airchat-switch <session_name>");
        return;
    }
    
    if (airchat_switch_session(av[1]) == 0) {
        fprint(1, "Switched to session: %s\n", av[1]);
    } else {
        rc_error("airchat-switch: session not found");
    }
}

void b_airchat_history(char **av) {
    ChatSession *session = current_session;
    
    if (av[1]) {
        session = airchat_get_session(av[1]);
    }
    
    if (!session) {
        rc_error("airchat-history: no session specified and no current session");
        return;
    }
    
    fprint(1, "Chat History for %s (%d messages):\n", session->session_name, session->history->count);
    
    for (int i = 0; i < session->history->count; i++) {
        time_t timestamp = session->history->messages[i].timestamp;
        char *timestr = ctime(&timestamp);
        timestr[strlen(timestr) - 1] = '\0';  /* Remove newline */
        
        fprint(1, "[%s] %s: %s\n", 
               timestr,
               session->history->messages[i].role,
               session->history->messages[i].content);
    }
    
    fprint(1, "Total tokens: %zu\n", session->history->total_tokens);
}

void b_airchat_websocket_start(char **av) {
    uint16_t port = 8080;
    if (av[1]) {
        port = (uint16_t)atoi(av[1]);
    }
    
    if (global_websocket_server) {
        fprint(1, "WebSocket server already running\n");
        return;
    }
    
    global_websocket_server = websocket_create_server(port);
    if (!global_websocket_server) {
        rc_error("airchat-websocket-start: failed to create server");
        return;
    }
    
    if (websocket_start_listening(global_websocket_server) == 0) {
        fprint(1, "WebSocket server started on port %d\n", port);
    } else {
        websocket_destroy_server(global_websocket_server);
        global_websocket_server = NULL;
        rc_error("airchat-websocket-start: failed to start server");
    }
}

void b_airchat_websocket_stop(char **av) {
    if (!global_websocket_server) {
        fprint(1, "No WebSocket server running\n");
        return;
    }
    
    websocket_destroy_server(global_websocket_server);
    global_websocket_server = NULL;
    fprint(1, "WebSocket server stopped\n");
}

void b_airchat_status(char **av) {
    fprint(1, "Airchat Status:\n");
    fprint(1, "Active sessions: %d\n", session_count);
    
    if (current_session) {
        fprint(1, "Current session: %s\n", current_session->session_name);
        if (current_session->model_path) {
            fprint(1, "Current model: %s\n", current_session->model_path);
        }
        fprint(1, "Message count: %d\n", current_session->history->count);
        fprint(1, "Token count: %zu\n", current_session->history->total_tokens);
    } else {
        fprint(1, "No current session\n");
    }
    
    if (global_websocket_server && global_websocket_server->is_listening) {
        fprint(1, "WebSocket server: running on port %d\n", global_websocket_server->port);
        fprint(1, "Connected clients: %d\n", global_websocket_server->client_count);
    } else {
        fprint(1, "WebSocket server: not running\n");
    }
}
# Distributed Network

## Overview

This document describes the network protocols, agent communication mechanisms, and membrane synchronization for distributed cognitive shell agents.

## Network Architecture

### Node Agent Model
Each rc shell instance operates as an autonomous cognitive agent:
- **Local Processing**: Independent cognitive grammar and attention allocation
- **Distributed Coordination**: Synchronized through network protocols
- **Emergent Intelligence**: Collective behavior from agent interactions

### Network Topology
```
    Agent-1     Agent-2     Agent-3
       |           |           |
    [Local]    [Local]    [Local]
   Cognitive  Cognitive  Cognitive
    Grammar    Grammar    Grammar
       |           |           |
       +-----[ Network ]-------+
             Communication
              & Membrane
             Synchronization
```

## Communication Protocols

### Protocol Stack
1. **Transport Layer**: TCP/UDP for reliability/speed trade-offs
2. **Messaging Layer**: ZeroMQ or gRPC for high-level communication  
3. **Cognitive Layer**: Custom protocols for attention and memory sync
4. **Security Layer**: Encrypted channels for sensitive cognitive data

### Message Types
```c
typedef enum {
    MSG_ATTENTION_SYNC,     // Synchronize attention allocation
    MSG_PATTERN_SHARE,      // Share discovered patterns
    MSG_COGNITIVE_STATE,    // Broadcast cognitive state
    MSG_COMMAND_REQUEST,    // Request command execution
    MSG_MEMORY_SYNC,        // Synchronize memory contents
    MSG_INFERENCE_QUERY,    // Distributed inference request
    MSG_HEARTBEAT          // Keep-alive and status
} MessageType;

typedef struct {
    MessageType type;
    uint32_t source_id;
    uint32_t dest_id;
    uint32_t timestamp;
    uint32_t data_length;
    char data[];
} CognitiveMessage;
```

### Network Discovery
```c
// Agent discovery and registration
typedef struct {
    uint32_t agent_id;
    char hostname[256];
    uint16_t port;
    uint32_t capabilities;
    uint32_t load_factor;
    time_t last_seen;
} AgentNode;

// Discovery protocol functions
extern int agent_discovery_start(uint16_t port);
extern int agent_announce(AgentNode *self);
extern AgentNode *agent_find_by_capability(uint32_t capability);
extern void agent_update_status(uint32_t load_factor);
```

## ZeroMQ Integration

### Socket Types and Patterns
```c
// Publisher-Subscriber for attention broadcasts
void *attention_publisher = zmq_socket(context, ZMQ_PUB);
void *attention_subscriber = zmq_socket(context, ZMQ_SUB);

// Request-Reply for direct cognitive queries
void *cognitive_request = zmq_socket(context, ZMQ_REQ);
void *cognitive_reply = zmq_socket(context, ZMQ_REP);

// Push-Pull for distributed task processing
void *task_push = zmq_socket(context, ZMQ_PUSH);
void *task_pull = zmq_socket(context, ZMQ_PULL);
```

### Message Routing
```c
// Route messages based on cognitive load
int route_cognitive_message(CognitiveMessage *msg) {
    AgentNode *target = find_least_loaded_agent();
    if (!target) {
        return process_locally(msg);
    }
    return forward_to_agent(target, msg);
}

// Distribute attention synchronization
void broadcast_attention_state(AttentionState *state) {
    char *serialized = serialize_attention_state(state);
    zmq_send(attention_publisher, serialized, strlen(serialized), 0);
    free(serialized);
}
```

## gRPC Integration

### Service Definition
```protobuf
service CognitiveAgent {
    rpc ProcessCommand(CommandRequest) returns (CommandResponse);
    rpc SyncAttention(AttentionState) returns (SyncResponse);
    rpc SharePattern(PatternData) returns (ShareResponse);
    rpc QueryKnowledge(KnowledgeQuery) returns (KnowledgeResponse);
    rpc SyncMemory(MemorySnapshot) returns (SyncResponse);
}

message CommandRequest {
    string command = 1;
    map<string, string> context = 2;
    AttentionState attention = 3;
}

message AttentionState {
    repeated AttentionNode nodes = 1;
    map<string, float> global_attention = 2;
    uint64 timestamp = 3;
}
```

### Implementation Stubs
```c
// gRPC service implementation
typedef struct {
    grpc_server *server;
    grpc_completion_queue *cq;
    void *cognitive_context;
} CognitiveServer;

extern CognitiveServer *cognitive_server_create(uint16_t port);
extern int cognitive_server_start(CognitiveServer *server);
extern void cognitive_server_shutdown(CognitiveServer *server);
```

## Membrane Synchronization

### Tensor Membrane States
```c
typedef struct {
    uint32_t membrane_id;
    uint32_t prime_factors[16];  // Prime factorization shape
    float *tensor_data;
    size_t data_size;
    uint64_t version;
    uint32_t checksum;
} TensorMembrane;

// Synchronization operations
extern int membrane_sync_start(uint32_t membrane_id);
extern int membrane_compare_versions(TensorMembrane *local, TensorMembrane *remote);
extern int membrane_merge_changes(TensorMembrane *dest, TensorMembrane *src);
extern int membrane_broadcast_update(TensorMembrane *membrane);
```

### Conflict Resolution
```c
// Handle concurrent membrane modifications
typedef enum {
    RESOLVE_LAST_WRITER_WINS,
    RESOLVE_ATTENTION_WEIGHTED,
    RESOLVE_MANUAL_MERGE,
    RESOLVE_SPLIT_MEMBRANE
} ConflictResolution;

extern int resolve_membrane_conflict(
    TensorMembrane *local,
    TensorMembrane *remote,
    ConflictResolution strategy
);
```

## Distributed Algorithms

### Consensus Protocols
```c
// Raft-like consensus for cognitive state
typedef struct {
    uint32_t term;
    uint32_t leader_id;
    CognitiveState *state;
    uint32_t commit_index;
} ConsensusState;

extern int consensus_propose_state(CognitiveState *state);
extern int consensus_vote(uint32_t term, uint32_t candidate_id);
extern ConsensusState *consensus_get_current(void);
```

### Load Balancing
```c
// Distribute cognitive processing load
typedef struct {
    float cpu_usage;
    float memory_usage;
    uint32_t active_patterns;
    uint32_t attention_load;
    time_t last_update;
} LoadMetrics;

extern LoadMetrics *get_local_load_metrics(void);
extern AgentNode *find_least_loaded_agent(void);
extern int redistribute_cognitive_load(void);
```

## Security Considerations

### Authentication
- Agent identity verification through public key cryptography
- Session establishment with key exchange
- Message integrity through digital signatures

### Authorization
- Role-based access to cognitive functions
- Capability-based message routing
- Resource usage quotas and limits

### Privacy
- Encrypted communication channels
- Selective memory sharing policies
- Attention state anonymization options

## Configuration

Network settings in config.h:
```c
#define AGENT_DISCOVERY_PORT 9090
#define COGNITIVE_SERVICE_PORT 9091
#define MAX_CONCURRENT_AGENTS 256
#define MESSAGE_BUFFER_SIZE 65536
#define HEARTBEAT_INTERVAL 5000  // milliseconds
#define CONSENSUS_TIMEOUT 10000  // milliseconds
```

## Shell Commands

New shell commands for distributed operations:
- `agent-discover` - Discover available agents
- `agent-connect <host:port>` - Connect to specific agent
- `pattern-share <pattern>` - Share pattern with network
- `attention-sync` - Synchronize attention state
- `membrane-sync <id>` - Synchronize tensor membrane
- `load-balance` - Trigger load redistribution
# Distributed Network Protocols - Implementation Guide

## Overview

This document describes the implemented distributed network protocols for cognitive shell agents in the rc shell. The implementation provides auto-discovery, peer-to-peer communication, and membrane synchronization across distributed agent nodes.

## Architecture

### Agent Node Model
Each rc shell instance operates as an autonomous cognitive agent with:
- **Local Processing**: Independent cognitive grammar and attention allocation
- **Distributed Coordination**: Synchronized through network protocols  
- **Emergent Intelligence**: Collective behavior from agent interactions

### Protocol Stack
1. **Transport Layer**: UDP for discovery, TCP for reliable communication
2. **Message Layer**: Custom cognitive message protocol
3. **Discovery Layer**: Broadcast-based agent discovery
4. **Synchronization Layer**: Membrane and attention state sync

## Network Protocols

### Message Types
```c
typedef enum {
    MSG_DISCOVERY,          // Agent discovery announcements
    MSG_HEARTBEAT,          // Keep-alive messages
    MSG_ATTENTION_SYNC,     // Synchronize attention allocation
    MSG_PATTERN_SHARE,      // Share discovered patterns
    MSG_COGNITIVE_STATE,    // Broadcast cognitive state
    MSG_COMMAND_REQUEST,    // Request command execution
    MSG_MEMORY_SYNC,        // Synchronize memory contents
    MSG_INFERENCE_QUERY,    // Distributed inference request
    MSG_MEMBRANE_SYNC       // Tensor membrane synchronization
} MessageType;
```

### Agent Discovery
- **Protocol**: UDP broadcast on configurable port (default 9090)
- **Message Format**: `AGENT_ANNOUNCE:id:hostname:port:capabilities:load`
- **Capabilities**: Bitmask of agent capabilities (0xFF = all capabilities)
- **Load Factor**: Current processing load (0-100)

### Communication Patterns
- **Discovery**: UDP broadcast for peer discovery
- **Direct Communication**: TCP connections for reliable message exchange
- **State Synchronization**: Periodic broadcast of cognitive states
- **Load Balancing**: Dynamic redistribution based on load metrics

## Membrane Synchronization

### Tensor Membrane Structure
```c
typedef struct {
    uint32_t membrane_id;       // Unique membrane identifier
    uint32_t prime_factors[16]; // Prime factorization shape
    float *tensor_data;         // Tensor data payload
    size_t data_size;          // Size of tensor data
    uint64_t version;          // Version for conflict resolution
    uint32_t checksum;         // Data integrity checksum
} TensorMembrane;
```

### Synchronization Protocol
1. **Version Tracking**: Each membrane modification increments version
2. **Conflict Detection**: Compare versions and checksums
3. **Merge Strategies**: Last-writer-wins with version precedence
4. **Broadcast Updates**: Network-wide membrane state distribution

### Conflict Resolution
- **Same Version**: Compare checksums for integrity verification
- **Different Versions**: Higher version takes precedence
- **Data Conflicts**: Merge based on attention weighting
- **Split Resolution**: Create separate membrane instances

## Shell Commands

### Agent Commands
```bash
# Start agent discovery
agent-discover [port]           # Default port 9090

# Connect to remote agent
agent-connect <host:port>       # Direct peer connection

# Check load and trigger balancing
load-balance [load_factor]      # Optional load factor 0-100
```

### Pattern Sharing
```bash
# Share cognitive patterns
pattern-share <pattern>         # Broadcast pattern to network

# Synchronize attention states
attention-sync                  # Share current attention allocation
```

### Membrane Operations
```bash
# Allocate tensor membrane
membrane-alloc <prime1> <prime2> ... # Create membrane with prime factors

# Synchronize membrane
membrane-sync <membrane_id>     # Sync specific membrane across network
```

## Configuration

### Compile-Time Options
```c
#define ENABLE_DISTRIBUTED_PROTOCOLS 1  // Enable distributed features
#define ENABLE_COGNITIVE_GRAMMAR 1      // Required for cognitive features
```

### Runtime Configuration
- **Discovery Port**: Default 9090, configurable via agent-discover
- **Service Port**: Default 9091 for peer connections
- **Agent Capabilities**: Configurable capability bitmask
- **Heartbeat Interval**: 5 seconds for keep-alive messages

## Integration with Cognitive Grammar

### Attention Synchronization
- **Local Calculation**: ECAN attention allocation per agent
- **Global Synchronization**: Periodic sharing of attention states
- **Load Balancing**: Redistribute processing based on attention load

### Pattern Distribution
- **Local Discovery**: Pattern recognition within agent
- **Network Sharing**: Broadcast discovered patterns to peers
- **Collective Learning**: Aggregate patterns across agent network

### Distributed Inference
- **Query Distribution**: Route inference queries to capable agents
- **Result Aggregation**: Combine results from multiple agents
- **Truth Value Propagation**: Share PLN truth values across network

## Example Usage

### Basic Agent Setup
```bash
# Terminal 1: Start first agent
rc -c "agent-discover 9090; pattern-share 'agent-1-pattern'"

# Terminal 2: Start second agent and connect
rc -c "agent-discover 9091; agent-connect localhost:9090"
```

### Membrane Synchronization
```bash
# Create and sync tensor membrane
membrane-alloc 2 3 5 7          # Create membrane with prime factors
membrane-sync 1                 # Synchronize membrane ID 1

# Monitor synchronization
cognitive-status                # Check cognitive state
```

### Distributed Cognitive Processing
```bash
# Process with distributed attention
attention-allocate "complex distributed task"
attention-sync                  # Share attention state

# Transform and share patterns  
cognitive-transform "distributed-pattern" "input data"
pattern-share "transformation result"
```

## Security Considerations

### Current Implementation
- **Basic Discovery**: Unencrypted UDP broadcast
- **Simple Authentication**: Agent ID-based identification
- **Trust Model**: All discovered agents are trusted

### Future Enhancements
- **Encrypted Channels**: TLS/SSL for secure communication
- **Authentication**: Public key cryptography for agent verification
- **Authorization**: Role-based access to cognitive functions
- **Privacy**: Selective memory sharing policies

## Performance Characteristics

### Discovery Performance
- **Discovery Time**: < 1 second for local network
- **Scalability**: Supports up to 256 concurrent agents
- **Network Overhead**: Minimal UDP broadcast traffic

### Synchronization Performance
- **Membrane Sync**: O(n) where n = number of membranes
- **Attention Sync**: Constant time per agent
- **Load Balancing**: Adaptive based on current load

### Memory Usage
- **Agent Registry**: Fixed allocation for 256 agents
- **Membrane Storage**: Dynamic allocation per membrane
- **Message Buffers**: 64KB default buffer size

## Troubleshooting

### Common Issues
1. **Discovery Failures**: Check network connectivity and firewall settings
2. **Sync Conflicts**: Verify membrane IDs and version consistency
3. **Load Imbalance**: Monitor agent load factors and redistribution

### Debugging Commands
```bash
cognitive-status               # Check current cognitive state
agent-discover 0              # List discovered agents (when implemented)
membrane-sync -v <id>         # Verbose membrane synchronization
```

### Log Analysis
- **Discovery Messages**: UDP broadcast traffic on port 9090
- **Sync Status**: Membrane version and checksum verification
- **Load Metrics**: Agent processing load and redistribution events

## Future Enhancements

### Protocol Extensions
- **ZeroMQ Integration**: Replace UDP with ZeroMQ for robust messaging
- **gRPC Services**: Add structured RPC for complex operations
- **WebSocket Support**: Enable web-based agent interfaces

### Advanced Features
- **Consensus Protocols**: Raft-based agreement for distributed state
- **Fault Tolerance**: Agent failure detection and recovery
- **Dynamic Discovery**: DNS-based service discovery
- **Metric Collection**: Distributed monitoring and analytics

This implementation provides a solid foundation for distributed cognitive shell agents with room for future enhancements and optimizations.
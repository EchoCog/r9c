# GGUF Inference Engine Implementation

## Overview

This implementation provides a comprehensive GGUF inference engine with guile-llama-cpp / llama.cpp / ggml integration for the rc shell, featuring an "airchat" interface similar to aichat with llm-functions.

## Architecture Components

```mermaid
graph TB
    subgraph "GGUF Inference Engine Architecture"
        subgraph "User Interface Layer"
            SHELL[rc Shell Interface]
            AIRCHAT[AI Chat Interface]
            WS[WebSocket Interface]
        end
        
        subgraph "Orchestration Layer"
            OR[Orchestrating Agent<br/>(or.h, or.c)]
            NEURAL[Neural Tree Structure]
            PATTERN[Pattern Analysis]
            RESONANCE[Resonance Depth]
        end
        
        subgraph "Execution Layer"
            EE[Execution Engine<br/>(execution-engine.c)]
            WORKERS[Worker Threads]
            HYPER[Hyper-Pattern Analysis]
            SPATIAL[Spatial Transformation]
        end
        
        subgraph "AI Processing Layer"
            AIR[AI Chatbot Interface<br/>(air.h, air.c)]
            SESSION[Session Management]
            PERSIST[Persistence Propagation]
        end
        
        subgraph "Model Layer"
            GGUF[GGUF Format Support<br/>(gguf.h, gguf.c)]
            LLAMA[llama.cpp Integration]
            GUILE[Guile Scheme Integration]
        end
        
        subgraph "Grammar Layer"
            PARSER[Grammar Parsers<br/>(r.y, r.l)]
            GRAMMAR[Grammar Engine<br/>(grammar.c)]
            ASM[Assembly Optimizations<br/>(r.s)]
        end
    end
    
    SHELL --> OR
    AIRCHAT --> AIR
    WS --> EE
    
    OR --> NEURAL
    OR --> PATTERN
    OR --> RESONANCE
    
    OR --> EE
    EE --> WORKERS
    EE --> HYPER
    EE --> SPATIAL
    
    OR --> AIR
    AIR --> SESSION
    AIR --> PERSIST
    
    EE --> GGUF
    AIR --> GGUF
    GGUF --> LLAMA
    GGUF --> GUILE
    
    OR --> PARSER
    PARSER --> GRAMMAR
    GRAMMAR --> ASM
    
    classDef ui fill:#fff3e0,stroke:#f57c00,stroke-width:2px
    classDef orch fill:#e8f5e8,stroke:#2e7d32,stroke-width:2px
    classDef exec fill:#e1f5fe,stroke:#0277bd,stroke-width:2px
    classDef ai fill:#fce4ec,stroke:#c2185b,stroke-width:2px
    classDef model fill:#f3e5f5,stroke:#7b1fa2,stroke-width:2px
    classDef grammar fill:#e0f2f1,stroke:#00695c,stroke-width:2px
    
    class SHELL,AIRCHAT,WS ui
    class OR,NEURAL,PATTERN,RESONANCE orch
    class EE,WORKERS,HYPER,SPATIAL exec
    class AIR,SESSION,PERSIST ai
    class GGUF,LLAMA,GUILE model
    class PARSER,GRAMMAR,ASM grammar
```

### 1. Orchestrating Agent (`or.h`, `or.c`)
- **Orchestrator** class serving as the main coordination center
- Neural tree structure with recursive propagation algorithms
- Advanced pattern analysis including resonance depth, temporal coherence, and spatial distribution
- Integration hooks for guile-llama-cpp inference engine
- Multi-threaded execution with async task processing
- Real-time coordination with other language components

**Key Functions:**
- `orchestrator_create()` - Create new orchestrator instance
- `orchestrator_load_model()` - Load GGUF models
- `orchestrator_inference()` - Perform inference operations
- `neural_tree_propagate()` - Neural activation propagation
- `pattern_analysis_update()` - Real-time pattern analysis

### 2. Execution Engine (`execution-engine.c`)
- **ExecutionEngine** with advanced execution and inference capabilities
- WebSocket server for real-time inter-component communication
- Concurrent processing with configurable worker routines
- Command execution system with timeouts and priority handling
- Spatial transformation and supervisor synthesis capabilities
- Hyper-pattern analysis and cognitive service daemons

**Key Functions:**
- `execution_engine_create()` - Initialize execution engine
- `execution_engine_start_websocket()` - Start WebSocket server
- `hyper_pattern_analysis()` - Advanced pattern recognition
- `spatial_transformation()` - Spatial data processing
- `supervisor_synthesis()` - Cognitive synthesis operations

### 3. AI Chatbot Interface (`air.h`, `air.c`)
- Shell terminal interface with integrated AI
- Real-time chat sessions with persistence propagation
- Session management with state evolution tracking
- Spatial journey drawterm and comprehensive analysis
- WebSocket support for live interactions
- Multi-namespace session capabilities

**Key Functions:**
- `airchat_create_session()` - Create chat sessions
- `airchat_send_message()` - AI message processing
- `airchat_load_model()` - Load GGUF models for inference
- `websocket_create_server()` - WebSocket communication
- `airchat_save_session()` - Session persistence

### 4. GGUF Format Support (`gguf.h`, `gguf.c`)
- Complete GGUF file format implementation
- Model loading and metadata extraction
- Integration with llama.cpp tensor operations
- Memory-mapped file access for efficiency
- Support for all GGUF tensor types and value types

**Key Functions:**
- `gguf_init_from_file()` - Load GGUF files
- `gguf_load_model()` - Complete model loading
- `gguf_get_model_info()` - Extract model metadata
- `gguf_get_tensor_data()` - Access tensor data

### 5. Grammar Parsers/Lexers (`r.y`, `r.l`, `grammar.c`)
- **YaccGrammar** defining all system components
- Process monitoring, failure detection, and automatic restart capabilities
- Inter-component message routing via WebSocket and HTTP
- Comprehensive status reporting and health monitoring
- Unified API for creating integrated cognitive trees

**Key Functions:**
- `cognitive_grammar_parse()` - Parse cognitive commands
- `tokenize_line()` - Command tokenization
- `parse_cognitive_command()` - Command interpretation

### 6. Assembly Optimizations (`r.s`)
- Low-level optimizations for cognitive processing
- Fast neural tree propagation algorithms
- Optimized pattern analysis kernels
- SIMD-ready computation routines

## Shell Commands

### Orchestrator Commands
```bash
orchestrator-create <name>                    # Create orchestrator
orchestrator-status                           # Show orchestrator status
orchestrator-load-model <name> <model_path>   # Load GGUF model
orchestrator-inference <name> <prompt>        # Run inference
```

### AI Chat Commands
```bash
airchat-create <session_name> [model_path]    # Create chat session
airchat-load <session> <model_path>           # Load model into session
airchat-chat <message>                        # Send message to AI
airchat-list                                  # List all sessions
airchat-switch <session>                      # Switch active session
airchat-history [session]                     # View chat history
airchat-status                                # Show airchat status
airchat-websocket-start [port]                # Start WebSocket server
airchat-websocket-stop                        # Stop WebSocket server
```

### Execution Engine Commands
```bash
execution-engine-create <name> [workers]      # Create execution engine
execution-engine-start                        # Start engine
execution-engine-stop                         # Stop engine
execution-engine-status                       # Show engine status
execution-engine-websocket-start [port]       # Start WebSocket
hyper-pattern-analysis <input>               # Pattern analysis
spatial-transformation <input>               # Spatial transforms
supervisor-synthesis <input>                 # Cognitive synthesis
```

### GGUF Model Commands
```bash
gguf-load <model_path>                        # Load GGUF model
gguf-info <model_path>                        # Show model information
```

### Grammar and Integration Commands
```bash
grammar-parse <command>                       # Parse cognitive grammar
cognitive-status                              # Show cognitive framework status
```

## Integration Points

### 1. Existing Cognitive Framework
- Seamless integration with existing cognitive extensions
- Shared attention allocation and pattern recognition
- Compatible with tensor membrane operations
- Extends hypergraph encoding capabilities

### 2. Future llama.cpp Integration
The implementation provides ready integration points for:
- `llama_model_load_from_file()` - Direct GGUF loading
- `llama_new_context_with_model()` - Context creation
- `llama_eval()` - Token evaluation
- `llama_sampling_*()` - Sampling strategies

### 3. Guile Scheme Integration
Stubs provided for:
- `guile_llama_load_model()` - Scheme model loading
- `guile_llama_generate()` - Scheme text generation
- `scheme_eval_cognitive()` - Cognitive grammar evaluation

## Usage Examples

### Basic AI Chat Session
```bash
# Create and configure session
airchat-create my-session
airchat-load my-session /path/to/model.gguf
airchat-chat "Hello, explain quantum computing"

# Session management
airchat-list
airchat-history my-session
```

### Orchestrator with Neural Processing
```bash
# Create orchestrator and load model
orchestrator-create neural-agent
orchestrator-load-model neural-agent /path/to/model.gguf

# Run inference with neural tree processing
orchestrator-inference neural-agent "Analyze this cognitive pattern"
orchestrator-status
```

### Advanced Cognitive Analysis
```bash
# Create execution engine
execution-engine-create cognitive-processor 8
execution-engine-start

# Run advanced analysis
hyper-pattern-analysis "complex multidimensional data"
spatial-transformation "neural network topology"
supervisor-synthesis "distributed reasoning system"
```

### Grammar-Guided Operations
```bash
# Parse and execute cognitive commands
grammar-parse "orchestrator create advanced-agent"
grammar-parse "airchat create intelligent-session"
grammar-parse "model load /path/to/advanced-model.gguf"
```

## Configuration

Enable GGUF features in `config.h`:
```c
#define ENABLE_COGNITIVE_GRAMMAR 1
#define ENABLE_IPC_EXTENSIONS 1
#define ENABLE_SCHEME_INTEGRATION 1
#define ENABLE_TENSOR_OPERATIONS 1
#define ENABLE_GGUF_SUPPORT 1        // New flag
#define ENABLE_AIRCHAT_INTERFACE 1   // New flag
```

## Build Requirements

- pthread library for multi-threading
- Standard POSIX sockets for WebSocket support
- Memory mapping support for GGUF files
- Optional: Guile development libraries
- Optional: llama.cpp libraries

## Future Enhancements

1. **Full llama.cpp Integration**
   - Direct linking with llama.cpp libraries
   - Advanced sampling strategies
   - GPU acceleration support

2. **Enhanced GGUF Support**
   - Streaming model loading
   - Model quantization support
   - Multiple model management

3. **Advanced WebSocket Features**
   - Real-time collaborative AI sessions
   - Multi-client broadcasting
   - Session synchronization

4. **Guile Scheme Extensions**
   - Full Scheme interpreter integration
   - Cognitive DSL implementation
   - Advanced pattern matching

This implementation provides a solid foundation for GGUF-based AI inference within the rc shell environment, ready for extension with full llama.cpp capabilities when available.
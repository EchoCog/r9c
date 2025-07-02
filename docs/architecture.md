# Cognitive Architecture Overview

## Vision

This document outlines the architecture for integrating OzCog/r9's rc shell as the neural substrate for a distributed, agentic cognitive grammar network.

## Cognitive Flowchart

```mermaid
flowchart TD
    A[Distributed Agentic Network]:::core
    subgraph Shell Substrate
        B(rc Shell Kernel):::kernel
        C[IPC Layer<br/>(Sockets, Pipes)]:::comm
        D[Command Parsing & Dispatch]:::parse
    end
    subgraph Cognitive Grammar Layer
        E[Hypergraph Pattern Encoder<br/>(Scheme/PLN)]:::cog
        F[Symbolic-Subsymbolic Bridge<br/>(ggml customization)]:::bridge
        G[Attention Allocation (ECAN)]:::attention
        H[Emergent Cognitive Patterns]:::emerge
    end
    subgraph Distributed Cognition
        I[Node Agent<br/>(rc shell instance)]:::agent
        J[Tensor Membrane<br/>(Prime Factorization Shapes)]:::tensor
        K[Agent Communication<br/>(Distributed Protocols)]:::comm
    end

    A --> B
    B --> C
    C --> D
    D --> E
    E --> F
    F --> G
    G --> H
    H --> J
    J --> K
    K --> I
    I --> B

    classDef core fill:#222,stroke:#bbb,stroke-width:2px,color:#fff;
    classDef kernel fill:#3c5,stroke:#181,stroke-width:2px;
    classDef comm fill:#59c,stroke:#49a,stroke-width:2px;
    classDef parse fill:#5af,stroke:#15f,stroke-width:2px;
    classDef cog fill:#fb5,stroke:#da0,stroke-width:2px;
    classDef bridge fill:#ffb,stroke:#bb0,stroke-width:2px;
    classDef attention fill:#f66,stroke:#d11,stroke-width:2px;
    classDef emerge fill:#aaf,stroke:#66c,stroke-width:2px;
    classDef agent fill:#0ff,stroke:#088,stroke-width:2px;
    classDef tensor fill:#fff,stroke:#bbb,stroke-width:2px;
```

## Architecture Components

### A. Shell Substrate (OzCog/r9)
- Acts as the universal interface for agentic processes
- Provides extensible command parsing for distributed IPC
- Supports embedding of cognitive grammar kernels via hooks/adaptors

### B. Cognitive Grammar Layer
- Encodes agent knowledge and processes in hypergraph structures (Scheme)
- Implements symbolic-subsymbolic integration via ggml-tensor customizations
- Allocates cognitive resources dynamically (ECAN), optimizing synergy and emergence

### C. Distributed Cognition
- Each shell instance operates as a node agent, forming a peer network
- Communication via distributed protocols (e.g., ZeroMQ, gRPC, or custom IPC)
- Membranes (tensor structures) encapsulate and modularize agentic kernels
- Attention and activation are managed globally and locally, enabling adaptive focus

## Implementation Status

This is a foundational implementation providing extension points and basic infrastructure for the cognitive architecture. The current implementation includes:

- Documentation scaffolding
- Basic IPC extension hooks
- Cognitive grammar interface stubs
- Placeholder tensor membrane modules
- Basic distributed protocol stubs

## Future Development

The architecture is designed to be incrementally extended with:
- Full Scheme/C integration
- Complete ECAN attention allocation system
- Probabilistic Logic Networks (PLN)
- Advanced tensor membrane operations
- Production-ready distributed protocols
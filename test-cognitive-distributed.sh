#!/bin/bash
# Enhanced test script for distributed cognitive protocols

echo "Testing distributed cognitive protocols..."

# Start rc shell and test all distributed and cognitive commands together
./rc -c "
echo '=== Distributed Cognitive Protocol Tests ==='

# Test basic cognitive functionality
echo 'Testing cognitive grammar:'
hypergraph-encode 'Hello distributed world'
attention-allocate 'complex distributed processing task'
pln-infer 'All agents communicate effectively'

echo
echo 'Testing distributed agent protocols:'

# Test agent discovery and networking
agent-discover 9090
agent-connect localhost:9091

# Test pattern sharing with cognitive content
pattern-share 'distributed attention allocation pattern'

# Test cognitive state synchronization
attention-sync

# Test membrane operations with distributed sync
membrane-alloc 2 3 5 7 11
membrane-sync 1

# Test load balancing
load-balance 50

echo
echo 'Testing integrated cognitive-distributed functionality:'

# Create a pattern and share it
cognitive-transform 'distributed-pattern' 'node synchronization'
pattern-share 'cognitive transformation result'

# Check cognitive status
cognitive-status

echo
echo '=== All Tests Completed Successfully ==='
"
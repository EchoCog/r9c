#!/bin/bash
# Test script for enhanced cognitive kernel functionality

echo "Testing Cognitive Grammar Kernel..."

echo "=== Testing Hypergraph Encoding ==="
./rc -c "hypergraph-encode 'hello world'"
./rc -c "hypergraph-encode 'complex cognitive processing system'"

echo -e "\n=== Testing ECAN Attention Allocation ==="
./rc -c "attention-allocate 'simple'"
./rc -c "attention-allocate 'complex multi-word cognitive processing task with sophisticated reasoning'"

echo -e "\n=== Testing PLN Probabilistic Reasoning ==="
./rc -c "pln-infer 'All birds fly'"
./rc -c "pln-infer 'If cognitive agents reason then they exhibit intelligence'"

echo -e "\n=== Testing Cognitive Pattern Transformations ==="
./rc -c "cognitive-transform 'greeting' 'hello world'"
./rc -c "cognitive-transform 'command' 'ls -la /tmp'"
./rc -c "cognitive-transform 'question' 'what is the meaning of life'"

echo -e "\n=== Testing Scheme Integration ==="
./rc -c "scheme-eval '(+ 10 20)'"
./rc -c "scheme-eval '(* 6 9)'"
./rc -c "scheme-eval 'complex-expression'"

echo -e "\n=== Testing Combined Cognitive Pipeline ==="
echo "Loading cognitive modules and testing integrated functionality..."
./rc -c 'load-example-modules; cognitive-status'
./rc -c 'load-example-modules; test-pattern "hello"; hypergraph-encode "hello world"; attention-allocate "hello world"; pln-infer "greeting detected"'

echo -e "\nCognitive Grammar Kernel test completed!"
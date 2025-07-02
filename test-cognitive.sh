#!/bin/bash
# Test script for cognitive extensions

echo "Testing cognitive extensions..."

# Test basic cognitive status
echo "=== Testing cognitive-status ==="
echo 'cognitive-status' | ./rc

# Test example cognitive modules
echo -e "\n=== Testing cognitive modules ==="
./rc -c 'load-example-modules; cognitive-status'

echo -e "\n=== Testing pattern recognition ==="
./rc -c 'load-example-modules; test-pattern hello'
./rc -c 'load-example-modules; test-pattern goodbye'
./rc -c 'load-example-modules; test-pattern unknown'

echo -e "\n=== Testing attention allocation ==="
./rc -c 'load-example-modules; test-attention short'
./rc -c 'load-example-modules; test-attention "much longer and more complex input"'

# Test IPC commands (expect them to fail since they're stubs)
echo -e "\n=== Testing IPC commands ==="
echo 'ipc-listen /tmp/test' | ./rc 2>&1 | grep -q "failed to create listener" && echo "✓ ipc-listen command found"
echo 'ipc-connect /tmp/test' | ./rc 2>&1 | grep -q "failed to connect" && echo "✓ ipc-connect command found"

# Test that undefined commands are not found
echo -e "\n=== Testing disabled commands ==="
./rc -c 'scheme-eval test' 2>&1 | grep -q "cannot find" && echo "✓ scheme-eval properly disabled"
./rc -c 'tensor-create test' 2>&1 | grep -q "cannot find" && echo "✓ tensor-create properly disabled"

echo -e "\nCognitive extensions full test completed!"
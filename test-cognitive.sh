#!/bin/bash
# Test script for cognitive extensions

echo "Testing cognitive extensions..."

# Test basic cognitive status
echo "=== Testing cognitive-status ==="
echo 'cognitive-status' | ./rc

# Test IPC commands (expect them to fail since they're stubs)
echo -e "\n=== Testing IPC commands ==="
echo 'ipc-listen /tmp/test' | ./rc 2>&1 | grep -q "failed to create listener" && echo "✓ ipc-listen command found"
echo 'ipc-connect /tmp/test' | ./rc 2>&1 | grep -q "failed to connect" && echo "✓ ipc-connect command found"

# Test that undefined commands are not found
echo -e "\n=== Testing disabled commands ==="
./rc -c 'scheme-eval test' 2>&1 | grep -q "cannot find" && echo "✓ scheme-eval properly disabled"
./rc -c 'tensor-create test' 2>&1 | grep -q "cannot find" && echo "✓ tensor-create properly disabled"

echo -e "\nCognitive extensions basic test completed!"
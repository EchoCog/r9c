#!/bin/bash
# Test script for tensor membrane functionality

echo "=== Testing Tensor Membrane Customization ==="

echo "Testing prime factorization-based tensor shapes..."

echo "membrane-create [2,3,5]" | ./rc -p
echo
echo "membrane-create [7,11]" | ./rc -p
echo
echo "membrane-create [2,2,3]" | ./rc -p
echo

echo "=== Testing tensor membrane operations ==="

cat <<EOF | ./rc -p
membrane-create [2,3,5]
membrane-list
membrane-info 1
membrane-add-object 1 pattern_a
membrane-add-object 1 pattern_b
membrane-info 1
membrane-fill 1 3.14
membrane-set 1 0,1 2.71
membrane-get 1 0,1
EOF

echo

echo "=== Testing P-system membrane operations ==="

cat <<EOF | ./rc -p
membrane-create [2,3]
membrane-create [5,7] 
membrane-add-object 1 data_x
membrane-add-object 1 data_y
membrane-add-object 2 result_z
membrane-info 1
membrane-info 2
membrane-transfer 1 2 data_x
membrane-info 1
membrane-info 2
EOF

echo

echo "=== Testing membrane reshape operations ==="

cat <<EOF | ./rc -p
membrane-create [2,3,5]
membrane-info 1
membrane-reshape 1 [2,15]
membrane-info 1
EOF

echo

echo "=== Testing multiple membranes ==="

cat <<EOF | ./rc -p
membrane-create [2]
membrane-create [3] 
membrane-create [5]
membrane-create [7]
membrane-list
membrane-destroy 2
membrane-destroy 4
membrane-list
EOF

echo
echo "Tensor membrane customization tests completed!"
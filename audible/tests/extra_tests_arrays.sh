#!/bin/bash

violations=$(grep --exclude-dir=include --exclude-dir=tests -I -RF '[' | grep -v 'IMPORTANT' | grep -v '//')
if [ $? == 0 ]; then
    echo "Uses array brackets"
    echo "Violations:"
    echo "$violations"
    exit 1
fi

#!/bin/bash

violations=$(grep --exclude-dir=include --exclude-dir=tests -I -RF '<strings.h>' | grep -v '#error')
if [ $? == 0 ]; then
    echo "Uses <strings.h>"
    echo "Violations:"
    echo "$violations"
    exit 1
fi
violations=$(grep --exclude-dir=include --exclude-dir=tests -I -RF '<string.h>' | grep -v '#error')
if [ $? == 0 ]; then
    echo "Uses <string.h>"
    echo "Violations:"
    echo "$violations"
    exit 1
fi
violations=$(grep --exclude-dir=include --exclude-dir=tests -I -RF '<ctype.h>' | grep -v '#error')
if [ $? == 0 ]; then
    echo "Uses <ctype.h>"
    echo "Violations:"
    echo "$violations"
    exit 1
fi

#!/usr/bin/env sh

FAILING_FILES=`grep -r "^ " * | grep -E "^(\w|/)+\.(cpp|h):" | grep -E -v "^(src/(Swift/)?)?3rdParty" | grep -v "^.*moc_" | sed -e "s/:.*//" | uniq`

if [ "$FAILING_FILES" ]; then
	echo "ERROR: Found whitespace instead of tabs in the following files:"
	echo "$FAILING_FILES"
fi

#!/bin/sh

# This script assumes that it is run from the toplevel directory, that
# the 'configure' script has been called with '--enable-coverage'

SOURCE_DIR=src/Swift
SCRIPT_DIR=tools/coverage

RESULTS_DIR=tools/coverage/results
OUTPUT_DIR=$RESULTS_DIR/coverage-`git log --pretty=format:%ct-%h | head -n 1`

make -C $SOURCE_DIR
if [ ! -f $OUTPUT_DIR ]; then
  mkdir -p $OUTPUT_DIR
fi

# Reset counters
lcov --zerocounters --directory $SOURCE_DIR

# All tests
make -C $SOURCE_DIR test
lcov --capture --directory $SOURCE_DIR -b $SOURCE_DIR --output-file $OUTPUT_DIR/all.info --test-name all
$SCRIPT_DIR/FilterLCovData.py $OUTPUT_DIR/all.info

# Generate HTML
gendesc -o $OUTPUT_DIR/descriptions $SCRIPT_DIR/descriptions.txt
genhtml --title "Swift Coverage" --output-directory $OUTPUT_DIR $OUTPUT_DIR/all.info

# Generate summary
$SCRIPT_DIR/GenerateSummary.py $OUTPUT_DIR/all.info $OUTPUT_DIR/summary

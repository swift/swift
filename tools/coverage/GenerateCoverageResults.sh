#!/bin/sh

# This script assumes that it is run from the toplevel directory, that
# the 'configure' script has been called with '--enable-coverage'

if [ ! -f config.status ]; then
	echo "Please configure your build with --enable-coverage and rebuild."
	exit -1
fi
grep -q "\-\-enable-coverage" config.status
if [ "$?" != 0 ]; then
	echo "Please configure your build with --enable-coverage and rebuild."
	exit -1
fi

SOURCE_DIR=.
SCRIPT_DIR=tools/coverage
LCOVDIR=3rdParty/LCov

RESULTS_DIR=tools/coverage/results
OUTPUT_DIR=$RESULTS_DIR/coverage-`git log --pretty=format:%ct-%h | head -n 1`

make -C $SOURCE_DIR
if [ ! -f $OUTPUT_DIR ]; then
  mkdir -p $OUTPUT_DIR
fi

# Reset counters
$LCOVDIR/lcov --zerocounters --directory $SOURCE_DIR

# All tests
make -C $SOURCE_DIR test
$LCOVDIR/lcov --capture --directory $SOURCE_DIR -b $SOURCE_DIR --output-file $OUTPUT_DIR/all.info --test-name all
$SCRIPT_DIR/FilterLCovData.py $OUTPUT_DIR/all.info

# Generate HTML
$LCOVDIR/gendesc -o $OUTPUT_DIR/descriptions $SCRIPT_DIR/descriptions.txt
$LCOVDIR/genhtml --no-function-coverage --title "Swift Coverage" --output-directory $OUTPUT_DIR $OUTPUT_DIR/all.info

# Generate summary
$SCRIPT_DIR/GenerateSummary.py $OUTPUT_DIR/all.info $OUTPUT_DIR/summary

#!/bin/sh

# This script assumes that it is run from the toplevel directory

SOURCE_DIR=.
SCRIPT_DIR=BuildTools/Coverage
LCOVDIR=3rdParty/LCov

#RESULTS_DIR=BuildTools/Coverage/results
#OUTPUT_DIR=$RESULTS_DIR/coverage-`git log --pretty=format:%ct-%h | head -n 1`

OUTPUT_DIR=BuildTools/Coverage/coverage
rm -rf $OUTPUT_DIR

if [ ! -d $OUTPUT_DIR ]; then
	mkdir -p $OUTPUT_DIR
fi

# Reset counters
$LCOVDIR/lcov --zerocounters --directory $SOURCE_DIR

# Build & run all tests
./scons coverage=1 test=all $@

# Run SCons
$LCOVDIR/lcov --capture --directory $SOURCE_DIR -b $SOURCE_DIR --output-file $OUTPUT_DIR/all.info --test-name all
#cp $OUTPUT_DIR/all.info $OUTPUT_DIR/all.info.orig
$SCRIPT_DIR/FilterLCovData.py $OUTPUT_DIR/all.info

# Generate HTML
$LCOVDIR/gendesc -o $OUTPUT_DIR/descriptions $SCRIPT_DIR/descriptions.txt
$LCOVDIR/genhtml --prefix $PWD --no-function-coverage --title "Swift Coverage" --output-directory $OUTPUT_DIR $OUTPUT_DIR/all.info

# Generate summary
$SCRIPT_DIR/GenerateSummary.py $OUTPUT_DIR/all.info $OUTPUT_DIR/summary

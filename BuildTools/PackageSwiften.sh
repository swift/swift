#!/bin/sh

package=swiften-`date +%Y-%m-%d`

git archive --prefix=$package/ --format=tar HEAD 3rdParty BuildTools Documentation QA Swiften scons scons.bat SConstruct > $package.tar

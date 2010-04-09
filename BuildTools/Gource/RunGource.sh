#!/bin/sh

gource --user-scale 2.0 --file-filter 3rdParty --user-image-dir BuildTools/Gource/UserImages -s .5 --hide filenames $@

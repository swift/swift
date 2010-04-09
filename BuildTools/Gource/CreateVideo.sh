#!/bin/sh

# Can be 25, 30, or 60 (per Gource)
FRAMERATE=25
BITRATE=3000K

BuildTools/Gource/GetGravatars.py BuildTools/Gource/UserImages
BuildTools/Gource/RunGource.sh --disable-progress --stop-at-end -640x360 $@ --output-framerate $FRAMERATE --output-ppm-stream - | ffmpeg -y -b $BITRATE -r $FRAMERATE -f image2pipe -vcodec ppm -i - -vcodec libx264 -vpre default BuildTools/Gource/SwiftGource.mp4

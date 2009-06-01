#!/bin/sh

rm -f aclocal.m4
autoheader
aclocal -I autoconf
autoconf

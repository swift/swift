#!/usr/bin/env bash
curl -o src/unbound-1.4.20.tar.gz http://www.unbound.net/downloads/unbound-1.4.20.tar.gz
cd src
tar -xf unbound-1.4.20.tar.gz
mv unbound-1.4.20 unbound
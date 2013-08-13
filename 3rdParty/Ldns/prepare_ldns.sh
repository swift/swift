#!/usr/bin/env bash
curl -o src/ldns-1.6.16.tar.gz http://www.nlnetlabs.nl/downloads/ldns/ldns-1.6.16.tar.gz
cd src
tar -xf ldns-1.6.16.tar.gz
mv ldns-1.6.16 ldns
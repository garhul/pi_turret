#!/bin/bash

set -ex

mkdir -p .deployable

cp app.js .deployable/
cp package.json .deployable/
cp utils.js .deployable/
cp test.js .deployable/

scp .deployable/* crank@garlic.local:/home/crank/control
rm .deployable/*

ssh crank@garlic.local 'cd ~/control; npm instal;'


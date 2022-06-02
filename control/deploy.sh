#!/bin/bash

set -ex

cp app.js deployable/
cp package.json deployable/
cp utils.js deployable/
cp test.js deployable/

scp deployable/* crank@garlic.local:/home/crank/control
ssh crank@garlic.local 'cd ~/control; npm instal;'
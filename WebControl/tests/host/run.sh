#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
bin=$(mktemp /tmp/tank-host-test.XXXXXX)
trap 'rm -f "$bin"' EXIT
c++ -std=c++17 -Itests/host/fakes -Iinclude tests/host/hardware_test.cpp src/TankDrive.cpp src/Button.cpp src/RgbLed.cpp -o "$bin"
"$bin"

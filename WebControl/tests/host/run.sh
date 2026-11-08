#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
bin=$(mktemp /tmp/tank-host-test.XXXXXX)
trap 'rm -f "$bin"' EXIT
c++ -std=c++17 -Itests/host/fakes -Iinclude tests/host/hardware_test.cpp src/TankDrive.cpp src/Button.cpp src/RgbLed.cpp src/Turret.cpp -o "$bin"
"$bin"

c++ -std=c++17 -Itests/host/fakes -Iinclude tests/host/motion_test.cpp src/MotionSensor.cpp -o "$bin"
"$bin"

c++ -std=c++17 -Itests/host/fakes -Iinclude tests/host/heading_test.cpp src/Heading.cpp -o "$bin"
"$bin"

c++ -std=c++17 -Itests/host/fakes -Iinclude tests/host/navigation_test.cpp src/Navigation.cpp src/Heading.cpp src/TankDrive.cpp -o "$bin"
"$bin"

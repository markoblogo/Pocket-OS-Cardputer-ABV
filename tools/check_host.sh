#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
for test in tests/*_test.py; do python3 "$test"; done
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
for test in persistence inbox_recovery; do
  c++ -std=c++17 -Wall -Wextra -I main "tests/${test}_test.cpp" main/persistence.cpp -o "$work/$test"
  "$work/$test"
done
c++ -std=c++17 -Wall -Wextra -DABVX_HOST_TEST -I main tests/gnss_test.cpp main/gnss_service.cpp -o "$work/gnss"
"$work/gnss"

c++ -std=c++17 -Wall -Wextra -I main tests/journey_test.cpp main/journey_service.cpp -o "$work/journey"
"$work/journey"

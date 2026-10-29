#!/usr/bin/env bash

set -e

runner_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

echo "NORMAL SMOKE TESTS"
python3 "$runner_dir/run.py" "$@"

echo "ERROR-DETECTION TEST: deliberately forcing ROOT to fail"
log=$(mktemp)
trap 'rm -f "$log"' EXIT
if ! false_command=$(type -P false) || [ ! -x "$false_command" ]; then
  echo "FAIL: external false executable not found on PATH" >&2
  exit 1
fi
rc=0
python3 "$runner_dir/run.py" "$@" --root "$false_command" >"$log" 2>&1 || rc=$?

expected="FAIL: ROOT write / calibration: exit 1; expected marker 'LFHCAL_SMOKE_OK: write'"
if [ "$rc" -eq 1 ] && grep -Fqx "$expected" "$log"; then
  echo "PASS: expected error detected"
else
  echo "FAIL: expected ROOT-write rejection with exit 1; got exit $rc" >&2
  cat "$log" >&2
  exit 1
fi

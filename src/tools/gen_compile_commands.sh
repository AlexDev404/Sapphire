#!/usr/bin/env bash
# Regenerates compile_commands.json for clangd from this Makefile's own
# CXXFLAGS/source layout, so include paths never need to be hardcoded
# per-machine in .clangd. Run from anywhere; re-run after adding new
# source directories or changing CXXFLAGS in the Makefile.
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/.."
ROOT="$(pwd -W 2>/dev/null || pwd)"

CXXFLAGS=(
  -ffreestanding -Wall -Wextra -fno-exceptions
  -fno-rtti -m32 -fno-pic -mno-sse -mno-sse2 -mno-mmx -mno-80387 -g
  -fno-pie -fno-inline -I./ -I./include -I./arch/noarch/include
)

sources=$(find kernel arch/x86 driver -type f -name '*.cpp' | sort)

{
  echo "["
  first=1
  while IFS= read -r file; do
    [ -z "$file" ] && continue
    if [ "$first" -eq 0 ]; then echo ","; fi
    first=0
    printf '  {\n'
    printf '    "directory": "%s",\n' "$ROOT"
    printf '    "file": "%s/%s",\n' "$ROOT" "$file"
    printf '    "arguments": ['
    printf '"g++"'
    for f in "${CXXFLAGS[@]}"; do printf ', "%s"' "$f"; done
    printf ', "-c", "%s"' "$file"
    printf ']\n'
    printf '  }'
  done <<< "$sources"
  echo
  echo "]"
} > compile_commands.json

echo "Wrote $(pwd)/compile_commands.json ($(echo "$sources" | grep -c .) entries)"

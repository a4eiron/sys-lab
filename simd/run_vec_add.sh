#!/bin/bash

IN_FILE="vec_add"
OUT_FILE="result_vec_add.txt"

> "$OUT_FILE"

OPTIMIZATIONS=("-O0" "-O3")

for opt in "${OPTIMIZATIONS[@]}"; do
    safe_name="${opt#-}"

    if ! gcc "$opt" -mavx2 "$IN_FILE.c" -lm -o "${IN_FILE}_${safe_name}"; then
        echo "error: compilation failed for $opt" >> "$OUT_FILE"
        continue
    fi

    {
        echo "---- optimization level: $opt ----"
        for i in {1..5}; do
            "./${IN_FILE}_${safe_name}"
            echo ""
        done
        echo ""
    } >> "$OUT_FILE"
done

echo "Done."
cat "$OUT_FILE"

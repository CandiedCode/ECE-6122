#!/bin/bash

# Ray tracing report generation script
# Runs all combinations of render modes, thread counts, and ray counts
# Automatically detects "Sample limit reached" and kills process

set -e

# Configuration
RAY_COUNTS=(3600 10800 36000 108000)
RENDER_MODES=("Single-Threaded" "StdThread" "OpenMP")
SAMPLE_COUNT=99

# Color codes for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get thread counts for a given render mode
get_thread_counts() {
    local mode=$1
    if [[ "$mode" == "Single-Threaded" ]]; then
        echo "1"
    else
        echo "2 4 8 16 32"
    fi
}

# Run a single test configuration
run_test() {
    local mode=$1
    local threads=$2
    local rays=$3

    echo -e "${BLUE}Running: --mode $mode --num-threads $threads --num-rays $rays${NC} --csv $SAMPLE_COUNT"

    # Run the command and capture output, detecting "Sample limit reached"
    # /Users/jennifercwagenberg/Code/gaTech_v2/ECE-6122/build/output/bin/Hw2
    /home/hice1/jcwagenberg6/code/ECE-6122/build/output/bin/Hw2 --mode "$mode" --num-threads "$threads" --num-rays "$rays" --csv "$SAMPLE_COUNT" 2>&1 | while IFS= read -r line; do
        echo "$line"
        if [[ "$line" == *"Sample limit reached"* ]]; then
            echo -e "${GREEN}✓ Sample limit reached - test complete${NC}"
            # Kill the parent process (Hw2)
            pkill -P $$ Hw2 2>/dev/null || true
            exit 0
        fi
    done
}

# Main loop
echo -e "${YELLOW}Starting report generation...${NC}"
echo "Ray counts: ${RAY_COUNTS[*]}"
echo "Render modes: ${RENDER_MODES[*]}"
echo "Sample count per configuration: $SAMPLE_COUNT"
echo ""

for mode in "${RENDER_MODES[@]}"; do
    echo -e "${YELLOW}=== Render Mode: $mode ===${NC}"

    thread_counts=$(get_thread_counts "$mode")

    for threads in $thread_counts; do
        echo -e "${YELLOW}--- Thread Count: $threads ---${NC}"

        for rays in "${RAY_COUNTS[@]}"; do
            run_test "$mode" "$threads" "$rays"
        done
    done
done

echo -e "${GREEN}All report generation complete!${NC}"

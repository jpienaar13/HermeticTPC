#!/bin/bash

# Usage:
# ./submit_hermeticjobs.sh <num_jobs> <events_per_job> <macro_file> [start_index] [--retry-post|--retry-failed]

NUM_JOBS=$1
NEVENTS=$2
MACROFILE=$3
START_INDEX=${4:-1}  #Option to allow starting any strating index on job numbers

# If start index is numeric, shift it; otherwise assume it's an option flag
if [[ "$4" =~ ^[0-9]+$ ]]; then
    shift 4
else
    shift 3
fi

#Rerunning failed job numbers option for completeness
RETRY_POST_ONLY=0
RETRY_FAILED=0

while [[ $# -gt 0 ]]; do
    case "$1" in
        --retry-post)
            RETRY_POST_ONLY=1
            shift
            ;;
        --retry-failed)
            RETRY_FAILED=1
            shift
            ;;
        *)
            echo "Unknown option $1"
            exit 1
            ;;
    esac
done

if [ -z "$NUM_JOBS" ] || [ -z "$NEVENTS" ] || [ -z "$MACROFILE" ]; then
    echo "Usage: ./submit_hermeticjobs.sh <num_jobs> <events_per_job> <macro_file> [start_index] [--retry-post|--retry-failed]"
    exit 1
fi

# Load singularity module
echo "Activating LCG environment..."
source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc12-opt/setup.sh

# Define container path and directories
SCRATCH_DIR="/storage/xenon/jacquesp/hermeticTPC"
TEMPLATE=run_hermeticTPCsingle.sh
LOGDIR="$SCRATCH_DIR/logs"

mkdir -p "$LOGDIR"

# Macro name parsing
MACRO_BASENAME=$(basename "$MACROFILE")
MACRO_BASENAME=${MACRO_BASENAME%.*}
MACRO_BASENAME_NO_PREFIX=${MACRO_BASENAME#run_}

# Temp dir for job scripts
TMPDIR=$(mktemp -d)

# Compute end index
END_INDEX=$((START_INDEX + NUM_JOBS - 1))

for ((i=START_INDEX; i<=END_INDEX; i++)); do
    JOBID=$(printf "%04d" $i)
    OUTFILE="${MACRO_BASENAME_NO_PREFIX}_${JOBID}.root"
    BASENAME="${MACRO_BASENAME_NO_PREFIX}_${JOBID}"
    ROOTFILE="${SCRATCH_DIR}/${OUTFILE}"
    POSTFILE="${SCRATCH_DIR}/${BASENAME}" 
    STEP="full"

    if [ $RETRY_POST_ONLY -eq 1 ]; then
        if [ -f "$POSTFILE" ]; then
            echo "[SKIP] Post-processing exists for $BASENAME"
            continue
        fi
        STEP="post"

    elif [ $RETRY_FAILED -eq 1 ]; then
        STEP="full"
        NEEDS_RETRY=0

        if [ ! -s "$ROOTFILE" ]; then
            echo "[RETRY FULL] ROOT file missing or empty for $BASENAME — resubmitting"
            NEEDS_RETRY=1
        else
            echo "[DEBUG] Checking if 'events/events' exists in $OUTFILE..."

            if ! singularity exec --bind "$SCRATCH_DIR:$SCRATCH_DIR" "$CONTAINER_PATH" \
                python -c "import uproot; f = uproot.open('$ROOTFILE'); print(any(k.startswith('events/events') for k in f.keys()))" \
                | grep -q True; then
                echo "[RETRY FULL] '$ROOTFILE' missing 'events/events' — resubmitting"
                NEEDS_RETRY=1
            elif [ ! -s "${POSTFILE}_5mm.npy" ]; then
                echo "[RETRY POST] Post-processing output missing for $BASENAME — resubmitting post only"
                STEP="post"
                NEEDS_RETRY=1
            else
                echo "[SKIP] All outputs valid for $BASENAME"
            fi
        fi

        if [ $NEEDS_RETRY -eq 0 ]; then
            continue
        fi
    fi

    JOBSCRIPT="${TMPDIR}/hermeticsub_${JOBID}.sh"

    sed \
        -e "s|{{NEVENTS}}|$NEVENTS|g" \
        -e "s|{{OUTFILE}}|$OUTFILE|g" \
        -e "s|{{BASENAME}}|$BASENAME|g" \
        -e "s|{{MACROFILE}}|$MACROFILE|g" \
        -e "s|{{STEP}}|$STEP|g" \
        "$TEMPLATE" > "$JOBSCRIPT"

    chmod +x "$JOBSCRIPT"
    echo "[SUBMIT] Submitting job for $BASENAME (step: $STEP)"
    qsub "$JOBSCRIPT"
done

rm -r "$TMPDIR"


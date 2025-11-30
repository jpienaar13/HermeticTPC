#!/bin/bash
###########################################
# -----------  GLOBAL DEFAULTS -----------
###########################################

SCALE=10
START_INDEX=0
RETRY_POST_ONLY=0
RETRY_FAILED=0
CONTAINER_PATH="/cvmfs/singularity.opensciencegrid.org/your/container" 
SCRATCH_DIR="/storage/xenon/$USER/hermeticTPC"
TEMPLATE="run_hermeticTPCsingle.sh"
LOGDIR="/srv01/xenon/$USER/HermeticTPC/logs"

############################################
# ---------------  COLORS ----------------
###########################################
C_RESET="\e[0m"
C_RED="\e[31m"
C_GREEN="\e[32m"
C_YELLOW="\e[33m"
C_BLUE="\e[34m"

msg() { echo -e "${C_BLUE}[*]${C_RESET} $*"; }
ok()  { echo -e "${C_GREEN}[OK]${C_RESET} $*"; }
warn(){ echo -e "${C_YELLOW}[WARN]${C_RESET} $*"; }
err() { echo -e "${C_RED}[ERR]${C_RESET} $*" >&2; }

###########################################
# ---------------  USAGE -----------------
###########################################
usage() {
    echo -e "${C_GREEN}Usage:${C_RESET}"
    echo "  $0 <num_jobs> <events_per_job> <macro_file> [start_index] [--scale N] [--post_proc_only] [--retry-failed]"
    exit 1
}

###########################################
# -----------  PARSE ARGUMENTS -----------
###########################################
if [ $# -lt 3 ]; then usage; fi

NUM_JOBS=$1
NEVENTS=$2
MACROFILE=$3
shift 3

# Optional positional start_index
if [[ $# -gt 0 && "$1" =~ ^[0-9]+$ ]]; then
    START_INDEX=$1
    shift
fi

# Optional flags
while [[ $# -gt 0 ]]; do
    case "$1" in
        --scale)
            SCALE=$2
            shift 2
            ;;
        --post_proc_only)
            POST_PROC_ONLY=1
            shift
            ;;
        --retry-failed)
            RETRY_FAILED=1
            shift
            ;;
        *)
            err "Unknown option: $1"
            usage
            ;;
    esac
done

###########################################
# ----------- ENVIRONMENT SETUP ----------
###########################################
setup_environment() {
    msg "Setting up environment..."
    mkdir -p "$LOGDIR"
}

###########################################
# ---------- CHECK IF RETRY NEEDED -------
###########################################
should_retry() {
    local basename="$1"
    local rootfile="${SCRATCH_DIR}/${basename}.root"
    local postfile="${SCRATCH_DIR}/${basename}"

    # ------------------------------------------------------
    # Post-processing ONLY mode (always redo post)
    # ------------------------------------------------------
    if (( POST_PROC_ONLY == 1 )); then
        STEP="post"
        return 0
    fi

    # ------------------------------------------------------
    # Retry only failed jobs
    # ------------------------------------------------------
    if (( RETRY_FAILED == 1 )); then

        # ROOT file missing or empty → redo full
        if [ ! -s "$rootfile" ]; then
            warn "ROOT file missing for $basename"
            STEP="full"
            return 0
        fi

        # Check existence of events/events
        msg "Checking 'events/events' in $rootfile..."
        PYTHON_CMD="python3 -"
        if [[ -n "$CONTAINER_PATH" ]]; then
            PYTHON_CMD="singularity exec --bind \"$SCRATCH_DIR:$SCRATCH_DIR\" \"$CONTAINER_PATH\" python3 -"
        fi

        python_output=$( $PYTHON_CMD <<EOF
import uproot
try:
    f = uproot.open("$rootfile")
    print(any(k.startswith("events/events") for k in f.keys()))
except Exception:
    print(False)
EOF
        )

        if [[ "$python_output" != "True" ]]; then
            warn "'events/events' missing for $basename"
            STEP="full"
            return 0
        fi

        # All outputs valid → skip
        ok "All outputs valid for $basename — skipping"
        return 1
    fi

    # Default: full simulation
    STEP="full"
    return 0
}

###########################################
# ------------ BUILD JOBSCRIPT -----------
###########################################
build_jobscript() {
    local jobscript="$1"
    local nevents="$2"
    local outfile="$3"
    local basename="$4"
    local macro="$5"
    local scale="$6"
    local step="$7"

    sed \
        -e "s|{{NEVENTS}}|$nevents|g" \
        -e "s|{{OUTFILE}}|$outfile|g" \
        -e "s|{{BASENAME}}|$basename|g" \
        -e "s|{{MACROFILE}}|$macro|g" \
        -e "s|{{STEP}}|$step|g" \
        -e "s|{{SCALE}}|$scale|g" \
        -e "s|{{USER}}|$USER|g" \
        "$TEMPLATE" > "$jobscript"

    chmod +x "$jobscript"
}

###########################################
# --------------- SUBMIT JOB -------------
###########################################
submit_job() {
    local jobscript="$1"
    local basename="$2"
    local step="$3"

    msg "Submitting job $basename (step: $step)"
    qsub "$jobscript"
}

###########################################
# ---------------- MAIN LOOP -------------
###########################################
setup_environment

TMPDIR=$(mktemp -d)
END_INDEX=$((START_INDEX + NUM_JOBS - 1))
MACRO_BASENAME=$(basename "$MACROFILE")
MACRO_TAG="${MACRO_BASENAME%.*}"
MACRO_TAG="${MACRO_TAG#run_}"

for ((i = START_INDEX; i <= END_INDEX; i++)); do
    JOBID=$(printf "%04d" $i)
    BASENAME="${MACRO_TAG}_${JOBID}"
    OUTFILE="${BASENAME}.root"

    STEP=""   # reset per job

    if ! should_retry "$BASENAME"; then
        continue
    fi

    # Safety: if STEP wasn't set, assume full simulation
    if [[ -z "$STEP" ]]; then
        STEP="full"
    fi

    JOBSCRIPT="$TMPDIR/hermeticsub_${JOBID}.sh"
    build_jobscript "$JOBSCRIPT" "$NEVENTS" "$OUTFILE" "$BASENAME" "$MACROFILE" "$SCALE" "$STEP"
    submit_job "$JOBSCRIPT" "$BASENAME" "$STEP"
done

rm -r "$TMPDIR"
ok "All jobs processed."

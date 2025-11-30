#!/bin/bash
#PBS -N hermeticTPC_job
#PBS -q N
#PBS -m n
#PBS -l walltime=12:00:00
#PBS -l select=1:ncpus=1:mem=2500mb
#PBS -l io=1
#PBS -j oe
#PBS -o /srv01/xenon/{{USER}}/HermeticTPC/logs/{{BASENAME}}.log

# Load environment
echo "Activating LCG environment..."
source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc12-opt/setup.sh

if [ "{{STEP}}" != "post" ]; then
    echo "Running GEANT4 simulation..."    
    cd /srv01/xenon/{{USER}}/HermeticTPC
    ./build/bin/hermeticTPC -f macros/{{MACROFILE}} -n {{NEVENTS}} -o /storage/xenon/{{USER}}/hermeticTPC/{{OUTFILE}}

fi

# Post-process
echo "Running post-processing..."
python /srv01/xenon/{{USER}}/HermeticTPC/analysis/proc_root_reduced.py /storage/xenon/{{USER}}/hermeticTPC/{{OUTFILE}} --outfile /storage/xenon/{{USER}}/hermeticTPC/{{BASENAME}}.parquet --scale {{SCALE}}

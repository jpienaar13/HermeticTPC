#!/bin/bash
#PBS -N hermeticTPC_job
#PBS -q N
#PBS -m n
#PBS -l walltime=02:00:00
#PBS -l select=1:ncpus=1:mem=2500mb
#PBS -l io=1
#PBS -j oe
#PBS -o /srv01/xenon/nadavh/HermeticTPC/logs/{{BASENAME}}.log

# Load environment
echo "Activating LCG environment..."
source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc12-opt/setup.sh

if [ "{{STEP}}" != "post" ]; then
    echo "Running GEANT4 simulation..."    
    cd /srv01/xenon/nadavh/HermeticTPC
    ./build/bin/hermeticTPC -f macros/{{MACROFILE}} -n {{NEVENTS}} -o /storage/xenon/nadavh/hermeticTPC/{{OUTFILE}}

fi

# Post-process
echo "Running post-processing..."
# python /home/jpienaar/HermeticTPC/analysis/process_backgrounds.py /scratch/midway2/jpienaar/hermeticTPC/{{OUTFILE}} --outfile /scratch/midway2/jpienaar/hermeticTPC/{{BASENAME}}

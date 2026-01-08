#!/bin/bash
#PBS -N hermeticTPC_job
#PBS -q N
#PBS -m n
#PBS -l walltime=12:00:00
#PBS -l select=1:ncpus=1:mem=2500mb
#PBS -l io=1
#PBS -j oe
#PBS -o /storage/xenon/{{USER}}/hermeticTPC/BigStats_01/log/{{BASENAME}}.log

COMPONENT=$"Sapphire"
ISOTOPE=$"Co60"

# Load environment
echo "Activating LCG environment..."
source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc12-opt/setup.sh

if [ "{{STEP}}" != "post" ]; then
    echo "Running GEANT4 simulation..."    
    cd /srv01/xenon/{{USER}}/HermeticTPC
    ./build/bin/hermeticTPC -f macros/{{MACROFILE}} -n {{NEVENTS}} -o /storage/xenon/{{USER}}/hermeticTPC/BigStats_01/${COMPONENT}/${ISOTOPE}/{{OUTFILE}}

fi

# Wait for root file to show up...
sleep 2

# Post-process
echo "Running post-processing: scale=10mm"
python /srv01/xenon/{{USER}}/HermeticTPC/analysis/proc_root_reduced.py /storage/xenon/{{USER}}/hermeticTPC/BigStats_01/${COMPONENT}/${ISOTOPE}/{{OUTFILE}} --outfile /storage/xenon/{{USER}}/hermeticTPC/BigStats_01/${COMPONENT}/${ISOTOPE}/{{BASENAME}}_s10mm.parquet --scale 10

echo "Running post-processing: scale=1mm"
python /srv01/xenon/{{USER}}/HermeticTPC/analysis/proc_root_reduced.py /storage/xenon/{{USER}}/hermeticTPC/BigStats_01/${COMPONENT}/${ISOTOPE}/{{OUTFILE}} --outfile /storage/xenon/{{USER}}/hermeticTPC/BigStats_01/${COMPONENT}/${ISOTOPE}/{{BASENAME}}_s1mm.parquet --scale 1

# Wait for files to stabilise
sleep 1

echo "Discarding root file"
rm /storage/xenon/{{USER}}/hermeticTPC/BigStats_01/${COMPONENT}/${ISOTOPE}/{{BASENAME}}.root

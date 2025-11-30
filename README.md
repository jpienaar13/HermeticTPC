# Hermetic TPC GEANT4 Simulation

## Supported GEANT4 Versions (Installation Prerequisites) ##
Current main branch supported:

GEANT4.11.02
(The Capsule-cryostats branch was developed on GEANT4.10.7 before switching. The PMT packing algorithm in this branch is currently outdated.)
The job submission scripts provided are optimsied for use on the Weizmann Atlas cluster. Change as needed.

## Installation Instructions on Atlas cluster##
1. Clone the repository and ensure that ROOT and GEANT4 are properly set up:
    ```
    git clone [https://<username>:<password>@github.com/jpienaar13/mc.git](https://github.com/jpienaar13/HermeticTPC.git)
    ```

2. Source the following CVMFS resource
   ```
   source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc12-opt/setup.sh
   ```

3. Compile the code
    ```
    cd mc
    cmake -S . -B build -DMAKE_STYLE=OFF && cmake --build build -j 4
    ```
4. After successful compilation, for visualtiona you can run as
   ```
    ./build/bin/hermeticTPC -f macros/run_Sapphire_U238.mac -i
   ```
   There are currently no implemented preinit options so the -p option (inherited from XENON) does nothing. The number of primaries to be simulated is specified with the -n option and the output file name with -o.


## Post processing
Post processing is done with the proc_root_reduced.py script. Currently the scale factor, which specifies when energy depositions are separated into clusters, defaults to 10mm, but it can be specified with --scale option. 


CONTAINER_PATH="/cloud/wwu1/p_agweinheimer/AGW_LowRad/software/container/xenonnt_mc_ms_20260112.sif"
CONTAINER_BIND="--bind /cloud/wwu1/p_agweinheimer/AGW_LowRad:/cloud/wwu1/p_agweinheimer/AGW_LowRad --bind /scratch/tmp/$USER:/SCRATCH"

SIMULATION_PATH="/cloud/wwu1/p_agweinheimer/AGW_LowRad/home/l_alth03/simulations/software"
SIMULATION_VERSION="20251205_HermeticTPC"

echo "Using container: $CONTAINER_PATH"
echo "Using simulation: $SIMULATION_VERSION"

# Clean previous build
rm -rf $SIMULATION_PATH/$SIMULATION_VERSION/build

# Load container and compile
ml Singularity
singularity exec $CONTAINER_BIND $CONTAINER_PATH /bin/bash -c "cd $SIMULATION_PATH/$SIMULATION_VERSION && cmake -S . -B build -DMAKE_STYLE=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON && cmake --build build -j4"

echo "Compilation finished."

# Echo $G4WORKDIR in signularity and store to variable
#G4WORKDIR=$(singularity exec $CONTAINER_BIND $CONTAINER_PATH /bin/bash -c "echo \$G4WORKDIR")

# On success move binary to shared folder
#if [ $? -eq 0 ]; then
#    echo "Compilation successful. Moving binary to shared folder."
#    cp $SIMULATION_PATH/$SIMULATION_VERSION/XeSim /AGW_LowRad/software/XeSim/$SIMULATION_VERSION/XeSim
#fi
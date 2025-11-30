#Hermetic TPC GEANT4 Simulation

## Supported GEANT4 Versions (Installation Prerequisites) ##
Current main branch supported:

GEANT4.11.02
(The Capsule-cryostats branch was developed on GEANT4.10.7 before switching. The PMT packing algorithm in this branch is currently outdated.)
The job submission scripts provided are optimsied for use on the Weizmann Atlas cluster. Change as needed.

Run like this:

cmake -S .. -B build -DMAKE_STYLE=OFF && cmake --build build -j --target install


If doesnt work:
# 1) Start clean to avoid cached flags
rm -rf build
mkdir build && cd build

# 2) Configure with C++17 (and optional GUI flags if you want Qt/OpenGL)
cmake -S .. -B . \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=17 \
  -DCMAKE_CXX_STANDARD_REQUIRED=ON \
  -DGEANT4_USE_QT=ON \
  -DGEANT4_USE_OPENGL_X11=ON

# 3) Build + install
cmake --build . -j --target install

# 4) Then go back to repo root dir and run
cd ..
QT_QPA_PLATFORM=xcb ./build/bin/hermeticTPC -f macros/run_Sapphire_U238.mac -i


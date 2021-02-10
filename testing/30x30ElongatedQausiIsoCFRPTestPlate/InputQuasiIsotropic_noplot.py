## Input data for RSG quasi-isotropic model
# modified from InputQuasiIsotropic.m
import numpy as np
from scipy import signal
import math

## Provide min and max phase velocities
maxPhaseVel = 10476.7; # m/s at 0 deg, 100 kHz via Disperse

## Calculate or provide spatial discretization
dx = 0.25e-3;
dy = 0.25e-3;
dz = 0.04e-3;

## Calculate time discretization

# dt based on CFL condition
C_MAX = 0.85; # CFL Number (less than 1 for stability)
dt = C_MAX * np.min([dx,dy,dz]) / maxPhaseVel; # Stability criterion for RSG with margin
rho = 1570.0

## Simulation time information
# Total time steps
simTime = 150e-6; # seconds
numTimeSteps = np.round(simTime/dt);

# Sampling rate
outputEvery = 50;

## Geometry information

# Plate model lengths
len_1 = 30e-3; # meters
len_2 = 30e-3;
len_3 = 0.96e-3;

# Solid cells
N1 = int(np.rint(len_1/dx)+2);
N2 = int(np.rint(len_2/dy)+2);
N3 = int(np.rint(len_3/dz)+2);

# geometry matrix;
Geom = np.zeros((N1,N2,N3), dtype=np.uint8);
Geom[1:-1,1:-1,1:-1] = 1

planeCounts = np.sum(np.sum(Geom, axis=2), axis=1).astype(dtype=np.uint64)

## Layup
z_rotation = np.array([0, 45, -45, 90, 90, -45, 45, 0]);
z_rotation = z_rotation * np.pi/180;
numPlies = len(z_rotation);

pliesPerLam = int(np.rint((N3-2)/numPlies));

Rot = np.zeros((N1-2,N2-2,N3-2, 3), dtype=np.float32);



# distribute z_rotation to dense matrix
idx = 0
for z_rot in z_rotation:
    for pl_idx in range(pliesPerLam):
        Rot[:,:,idx, 0] = z_rot
        idx+=1
        pass
    pass
            
## Excitation

# Force position in meters
fLoc1 = 15e-3; # x-position
fLoc2 = 15e-3; # y-position
fLoc3 = 0;     # z-position

#Force Position in cells
F1 = int(np.rint(fLoc1/dx));
F2 = int(np.rint(fLoc2/dy));
F3 = int(np.rint(fLoc3/dz));

# Transducer size
tSize = 5e-3; # Excitation radius in meters

# Force normal
forceDirection = [1, 0, 0];
forceNormal = forceDirection/np.linalg.norm(forceDirection);

# Force orientation
# Not implemented

## Excitation signal

domFreq = 200e3;   # Hz

numCycles = 6.5;
pulseLen = (numCycles/domFreq); # Pulse length (seconds) given # of cycles
pulseSteps = int(np.ceil(pulseLen/dt)); # # of pulse time steps 

print('pulseSteps')
print(pulseSteps)

amp = 10**6;

t = np.arange(0, (pulseSteps-1)*dt, dt);
excit = (amp*np.sin(t*domFreq*2*np.pi)).astype(np.double) 
window = signal.hann(pulseSteps)
excit = np.multiply(excit,window);

geomName = 'Geometry';
fname = "%s.raw" % geomName
pcfname = "%s_planeCounts.raw" % geomName
Geom.tofile(fname, sep='')
planeCounts.tofile(pcfname, sep='')
Rot.tofile('Rotations.bin', sep='')
excit.tofile('Excitation.bin', sep='')


# ## Write PlaneCounts to File

# fileID = fopen([geomName '_planeCounts.raw'],'w');
# fwrite(fileID, planeCounts,'uint64');
# fclose(fileID);

# ## Write Excitation to File

# fileID = fopen('Excitation.bin','w');
# fwrite(fileID, excit, 'single');
# fclose(fileID);

# ## Write Geometry to File

# fileID = fopen([geomName '.raw'],'w');
# fwrite(fileID, Geom, 'uint8');
# fclose(fileID);

# ## Write Rotations to File

# fileID = fopen('Rotations.bin','w');
# fwrite(fileID, Rot, 'single');
# fclose(fileID);


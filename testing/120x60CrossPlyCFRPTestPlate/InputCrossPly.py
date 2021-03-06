## Input data for RSG quasi-isotropic model
# modified from InputQuasiIsotropic.m
import numpy as np
from scipy import signal
import math

plotting = False

## Provide min and max phase velocities
maxPhaseVel = 10476.7; # m/s at 0 deg, 100 kHz via Disperse

## Calculate or provide spatial discretization
dx = 0.125e-3;
dy = 0.125e-3;
dz = 0.02875e-3;

## Calculate time discretization

# dt based on CFL condition
C_MAX = 0.85; # CFL Number (less than 1 for stability)
#dt = C_MAX * np.min([dx,dy,dz]) / maxPhaseVel; # Stability criterion for RSG with margin
dt = 2.3e-9
rho = 1570.0

## Simulation time information
# Total time steps
simTime = 60e-6; # seconds
numTimeSteps = np.round(simTime/dt);

# Sampling rate
outputEvery = 1;

## Geometry information

# Plate model lengths
len_1 = 120e-3; # meters
len_2 = 60e-3;
len_3 = 0.92e-3;

# Solid cells
NX = int(np.rint(len_1/dx));
NY = int(np.rint(len_2/dy));
NZ = int(np.rint(len_3/dz));

# geometry matrix;
Geom = np.ones((NX,NY,NZ), dtype=np.uint8)
planeCounts = np.sum(np.sum(Geom, axis=2), axis=1).astype(dtype=np.uint64)


## Layup
z_rotation = np.array([0, 0, 90, 90, 90, 90, 0, 0]);
z_rotation = z_rotation * np.pi/180;
numPlies = len(z_rotation);
pliesPerLam = int(np.rint((NZ)/numPlies));
Rot = np.zeros((NX,NY,NZ, 3), dtype=np.float32);

# distribute z_rotation to dense matrix
idx = 0
for z_rot in z_rotation:
    for pl_idx in range(pliesPerLam):
        Rot[:,:,idx, 0] = z_rot
        idx+=1
        pass
    pass

geomName = 'Geometry';
fname = "%s.raw" % geomName
pcfname = "%s_planeCounts.raw" % geomName
Geom.tofile(fname, sep='')
planeCounts.tofile(pcfname, sep='')
Rot.tofile('Rotations.bin', sep='')            
## Excitation

# Force position in meters
fLoc1 = 90e-3; # x-position
fLoc2 = 90e-3; # y-position
fLoc3 = 0;     # z-position

#Force Position in cells
F1 = int(np.rint(fLoc1/dx));
F2 = int(np.rint(fLoc2/dy));
F3 = int(np.rint(fLoc3/dz));

# Transducer size
tSize = 19.5e-3; # Excitation radius in meters

# Force normal
forceDirection = [1, 0, 0];
forceNormal = forceDirection/np.linalg.norm(forceDirection);

# Force orientation
# Not implemented

## Excitation signal

domFreq = 300e3;   # Hz
numCycles = 3;
pulseLen = (numCycles/domFreq); # Pulse length (seconds) given # of cycles
pulseSteps = int(np.ceil(pulseLen/dt)); # # of pulse time steps 

print('pulseSteps = %d' % pulseSteps)

amp = 10**6;
t = np.arange(0, (pulseSteps)*dt, dt)
excit = (amp*np.sin(t*domFreq*2*np.pi)).astype(np.double) 
window = signal.hann(pulseSteps)
excit = np.multiply(excit, window) 


# delamination
d_cx = 28e-3 # m
d_cy = 32e-3 # m
d_wx = 20e-3 
d_wy = 20e-3

Geom[int(d_cx/dx-d_wx/(2*dx)):int(d_cx/dx+d_wx/(2*dx)),int(d_cy/dy-d_wy/(2*dy)):int(d_cy/dy+d_wy/(2*dy)),24] = 0
planeCounts = np.sum(np.sum(Geom, axis=2), axis=1).astype(dtype=np.uint64)


rot_array = np.zeros(int(np.sum(planeCounts)*3), dtype=np.float32)
rot_i = 0
for i in range(NX):
    for j in range(NY):
        for k in range(NZ):
            if Geom[i,j,k] > 0:
                for c in range(3):
                    rot_array[rot_i*3+c] = Rot[i, j, k, c]
                rot_i += 1 
                pass
            pass
        pass
    pass


if(plotting):
    import matplotlib.pyplot as plt
    plt.figure(1)
    plt.imshow(Geom[:, :, 23], interpolation='none')
    plt.figure(2)
    plt.imshow(Geom[int(d_cx/dx), int(d_cy/dy-d_wy/dy-10):int(d_cy/dy+d_wy/dy+10), :], interpolation='none')
    plt.figure(3)
    plt.imshow(Rot[10, 10:30, :, 0], interpolation='none')
    plt.figure(4)
    plt.plot(t,excit);
    plt.show()
    pass


geomName = 'Geometry_delam';
fname = "%s.raw" % geomName
pcfname = "%s_planeCounts.raw" % geomName
Geom.tofile(fname, sep='')
planeCounts.tofile(pcfname, sep='')
rot_array.tofile('Rotations_delam.bin', sep='')
excit.tofile('Excitation.bin', sep='')


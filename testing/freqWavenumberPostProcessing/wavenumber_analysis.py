import numpy as np
import scipy.interpolate as spinterp
import scipy.optimize as spopt
import vtk
import recon

#user supplied data
save_path = "20200923_180x180ElongatedQausiIsoCFRPTestPlate"
data_path = "%s"%save_path
file_root = "Output_at"
data_to_process = "V2"
t_index_start = 4400
number_of_sequential_datasets = 100
target_frequency = 300e3
z_target_slice = 0.1e-3

# note, if files are indexed 0,1,2,..., 
#   - this requires a time_step_multiplier of 1
#   - this requires timestep_seconds to be the time interval between files
# if files are indexed e.g. 0,50,100,150..., 
#   - this requires a time_step_multiplier of 50
#   - this requires a timestep_seconds to be the simulation time step 
#         (e.g., the time interval between files is 50x timestep_seconds)
# timestep_seconds = 0.1/target_frequency
timestep_seconds = 2.3e-09
time_step_multiplier = 50

#post processing
rc = recon.reconstructor("%s/%s"%(data_path, file_root))
Result, kx, ky, Acc = rc.generateWavenumberDataAtFrequency(number_of_sequential_datasets, t_index_start,
                                                      target_frequency, timestep_seconds,
                                                      z_target_slice, data_name=data_to_process,
                                                      time_step_multiplier=time_step_multiplier)


FVzM = np.abs(Result)
FVzM = FVzM/np.max(np.max(FVzM))

FV = spinterp.RectBivariateSpline(kx,ky,FVzM.T)
theta = np.linspace(0,90,91)*np.pi/180.
kr_max = np.zeros(theta.size)

mid = np.array([int(kx.size/2),int(ky.size/2)])
idx_maxx = mid[1]+np.argmax(FVzM[mid[1],mid[0]:])
idx_maxy = mid[0]+np.argmax(FVzM[mid[1]:,mid[0]])
guess = 0.5*(kx[idx_maxx]+ky[idx_maxy])

def f(kr, theta):
    return FV.ev(kr*np.cos(theta), kr*np.sin(theta))

for k in range(theta.size):
    kr_max[k] = spopt.fmin(lambda x: -f(x, theta[k]), guess)

#saving
np.savez("%s/wavenumber_results.npz"%save_path, Result=Result, kx=kx, ky=ky, theta=theta, kr=kr_max)

csv_data = np.array([theta,kr_max]).T

np.savetxt("%s/wavenumber_results.csv"%save_path, csv_data, delimiter=",")


import numpy as np
import scipy.interpolate as spinterp
import scipy.optimize as spopt
import vtk
import recon

#user supplied data
#save_path="./20200922_120x60CrossPlyCFRPTestPlate"
save_path = "./20201019_120x60CrossPlyCFRPTestPlate_FBH"
data_path = "%s" % save_path
file_root = "Output_at"
data_to_process = "V2"
t_index_start = 100
number_of_sequential_datasets = 261
dz = 0.02875e-3
z_target_slice = 5*dz

print("Saving %s slice at z=%f" %(data_to_process, z_target_slice))

# note, if files are indexed 0,1,2,..., 
#   - this requires a time_step_multiplier of 1
#   - this requires timestep_seconds to be the time interval between files
# if files are indexed e.g. 0,50,100,150..., 
#   - this requires a time_step_multiplier of 50
#   - this requires a timestep_seconds to be the simulation time step 
#         (e.g., the time interval between files is 50x timestep_seconds)
time_step_multiplier = 100

#post processing
rc = recon.reconstructor("%s/%s"%(data_path, file_root))
Result, dims, ds = rc.generateZSurfaceChunk(number_of_sequential_datasets, t_index_start, z_target_slice,
                                        data_name=data_to_process, time_step_multiplier=time_step_multiplier)


#saving
ofile = "%s/Top_Z_Slice_results.npz"%save_path
print("Saving %s." % ofile)
np.savez(ofile, Result=Result, dims=dims, ds=ds)



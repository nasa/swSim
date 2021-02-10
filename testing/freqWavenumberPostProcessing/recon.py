#

import vtk
import scipy.fft as spfft
import scipy.signal as spsig
import numpy as np

class reconstructor:
  def __init__(self,fname_base=""):
    self.setFilenameBase(fname_base)
    return

  def setFilenameBase(self,fname_base):
    self._fname_root=fname_base
    return

  def generate3DFFT(self,Nt,tstart,dt,z=0.,data_name="Vz",time_step_multiplier=1):
    tukey_time=spsig.tukey(Nt,alpha=0.5)
    for kt in range(Nt):
      tidx=tstart+kt*time_step_multiplier
      data,dims,ds=self._loadTimeStepDataSlice(tidx,z,data_name=data_name)
      if kt==0:
        result=np.zeros([data.shape[0],data.shape[1],Nt],dtype=np.cdouble)
        tukey_space=self._makeTukeySpatialWindow(dims,alpha=0.5)
      result[:,:,kt]=data*tukey_space*tukey_time[kt]
    wavenum_x,wavenum_y=self._makeWavenumbers(dims,ds)
    frequency=spfft.fftshift(spfft.fftfreq(Nt,dt))
    result=spfft.fftshift(spfft.fftn(result))
    return result,wavenum_x,wavenum_y,frequency

  def generateWavenumberDataAtFrequency(self,Nt,tstart,freq,dt,z=0.,
                                        data_name="Vz",time_step_multiplier=1):
    tukey_time=spsig.tukey(Nt,alpha=0.5)
    for kt in range(Nt):
      tidx=tstart+kt*time_step_multiplier
      data,dims,ds=self._loadTimeStepDataSlice(tidx,z,data_name=data_name)
      if kt==0:
        Acc=np.zeros(data.shape,dtype=np.cdouble)
      Acc+=data*np.exp(-1j*2*np.pi*freq*tidx*dt)*dt*tukey_time[kt]
    wavenum_x,wavenum_y=self._makeWavenumbers(dims,ds)
    tukey_space=self._makeTukeySpatialWindow(dims,alpha=0.5)
    result=spfft.fftshift(spfft.fftn(Acc*tukey_space))
    return result,wavenum_x,wavenum_y,Acc
  
  def generateWavenumberDataAtFrequency_data(self, data, dims, ds, freq, dt):
    Nt = data.shape[2]
    tukey_time=spsig.tukey(Nt,alpha=0.5)
    Acc=np.zeros((data.shape[0],data.shape[1]),dtype=np.cdouble)
    for kt in range(Nt):
      tidx=tstart+kt*time_step_multiplier
      Acc+=data[:,:,kt]*np.exp(-1j*2*np.pi*freq*tidx*dt)*dt*tukey_time[kt]
    wavenum_x,wavenum_y=self._makeWavenumbers(dims,ds)
    tukey_space=self._makeTukeySpatialWindow(dims,alpha=0.5)
    result=spfft.fftshift(spfft.fftn(Acc*tukey_space))
    return result,wavenum_x,wavenum_y,Acc

  def generateZSurfaceChunk(self,Nt,tstart,z=0.,data_name="Vz", time_step_multiplier=1):
    for kt in range(Nt):
      tidx=tstart+kt*time_step_multiplier
      data,dims,ds=self._loadTimeStepDataSlice(tidx,z,data_name=data_name)
      if kt==0:
        result=np.zeros([data.shape[0],data.shape[1],Nt],dtype=np.double)  
      result[:, :, kt]=data
    return result, dims, ds

  # def writeSegment(self,filename,segment,timestep,z=0.):
  #   dat=self._readSegmentAtTimestep(segment,timestep,z)
  #   writer=vtk.vtkXMLImageDataWriter()
  #   writer.SetFileName(filename)
  #   writer.SetInputData(dat)
  #   writer.Update()
  #   return
  
  # def writeSlices(self,Nt,write_root,read_root,z=0.):
  #   for kt in range(Nt):
  #     wname="%s_%i.vti"%(write_root,kt)
  #     rname="%s_%i.pvtu"%(read_root,kt)
  #     self.writeSlice(wname,rname,z)
  #   return

  # def writeSlice(self,filename,readname,z=0.):
  #   dat=self._readSliceFromVTUFile(readname,z)
  #   img=self._makeVTKImageFromPolydata(dat)
  #   writer=vtk.vtkXMLImageDataWriter()
  #   writer.SetFileName(filename)
  #   writer.SetInputData(img)
  #   writer.Write()
  #   return

## private stuff
  def _makeWavenumbers(self,dims,spacing):
    wavenum_x=spfft.fftshift(spfft.fftfreq(dims[0],spacing[0]))
    wavenum_y=spfft.fftshift(spfft.fftfreq(dims[1],spacing[1]))
    return wavenum_x,wavenum_y

  def _makeTukeySpatialWindow(self,dims,alpha=0.5):
    tukey_win_x=spsig.tukey(dims[0],alpha=alpha)
    tukey_win_y=spsig.tukey(dims[1],alpha=alpha)
    tukey_space=np.outer(tukey_win_y,tukey_win_x)
    return tukey_space

  def _loadTimeStepDataSlice(self,tidx,z,data_name="Vz"):
    print("load timestep: %i"%tidx)
    pdat=self._readSegmentAtTimestep(tidx,z)
    img_data=self._makeVTKImageFromPolydata(pdat)
    data=self._makeNumpySliceFromImg(img_data,data_name=data_name)
    dims=img_data.GetDimensions()
    spacing=img_data.GetSpacing()
    return data,dims,spacing

  def _makeNumpySliceFromImg(self,img_data,data_name="Vz"):
    dims=img_data.GetDimensions()
    Result=np.zeros(dims[0:2]).T
    Result_raw=img_data.GetPointData().GetAbstractArray(data_name)
    for kx in range(dims[0]): 
      for ky in range(dims[1]): 
        Result[ky,kx]=Result_raw.GetTuple1(kx+ky*dims[0])
    return Result

  def _readSliceFromVTUFile(self,filename,z=0.):
    # Read the source file.
    reader=vtk.vtkXMLPUnstructuredGridReader()#vtk.vtkXMLUnstructuredGridReader()
    reader.SetFileName(filename)
    reader.Update()  # Needed because of GetScalarRange    
    pt2cell=vtk.vtkPointDataToCellData()
    pt2cell.SetInputConnection(reader.GetOutputPort())
    pt2cell.Update()
    plane=vtk.vtkPlane()
    plane.SetNormal(0.,0.,1.)
    plane.SetOrigin(0.,0.,z)
    slicer=vtk.vtkCutter()#vtk.vtkPlaneCutter()
    slicer.SetInputConnection(pt2cell.GetOutputPort())
    slicer.SetCutFunction(plane)#SetPlane(plane)#
    slicer.Update()
    output=slicer.GetOutput()
    return output

  def _makeVTKImageFromPolydata(self,dat):
    bounds=dat.GetBounds()
    dims=np.array([bounds[1]-bounds[0],bounds[3]-bounds[2]])
    maxdim=np.max(dims)
    cell_aabb=dat.GetCell(0).GetBounds()
    dx=cell_aabb[1]-cell_aabb[0]
    dy=cell_aabb[3]-cell_aabb[2]
    resampler=vtk.vtkResampleToImage()
    resampler.SetInputDataObject(dat)
    resampler.SetSamplingDimensions(int(dims[0]/dx),int(dims[1]/dy),1)
    resampler.Update()
    output=resampler.GetOutput()
    return output

  def _readSegmentAtTimestep(self,timestep,z=0.):
    filename="%s_%i.pvtu"%(self._fname_root,timestep)
    output=self._readSliceFromVTUFile(filename,z)
    print("Reading %s." % filename)
    return output

# swSim

Project Name: Solid Wave Sim (swSim)
Version 1.0
Authors: Erik Frankforter, Elizabeth Gregory, Bill Schneck
Description:
Solid Wave Simulation (swSim) is software that solves heterogeneous, anisotropic elastodynamic equations for ultrasonic NDE simulation. A time-explicit staggered grid finite difference (FD) scheme is employed and solved on graphics processing units (GPUs). Parallelism via the Message Passing Interface (MPI) allows for deployment on a cluster, or on a single computer with one or more GPUs. Wavefield data is output using Visualization Toolkit (VTK) file formats for 3D rendering capabilities via open source tools, and a kernel composition module allows high-level registration of sequences of matrix operations, providing extensibility in equations and physics solved.

# License: 
Copyright 2020 United States Government as represented by the Administrator of the National 
Aeronautics and Space Administration. No copyright is claimed in the United States under 
Title 17, U.S. Code. All Other Rights Reserved. See Appendix A for 3rd party licenses.

The Solid-Wave Sim (swSIM) platform is licensed under the Apache License, Version 2.0 (the 
"License"); you may not use this file except in compliance with the License. You may obtain 
a copy of the License at http://www.apache.org/licenses/LICENSE-2.0. 

Unless required by applicable law or agreed to in writing, software distributed under the 
License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

# Notices:

Googletest is a product of Google Inc. and is subject to the following:
 
Copyright 2008, Google Inc. All rights reserved.
           
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of Google Inc. nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

# Documentation

Available at https://nasa.github.io/swSim/

# Building swSim
1. Download latest release
2. Ensure correct dependencies are installed, and are in the search path
  - MPI
  - VTK >= 9.0 (built and linked with the same MPI being used for PanNDE)
  - gcc >= 7.3
  - cmake >= 3.13.5
  - libxml2 >-2.9.10
  - cuda >= 10.1
3. Ensure Python3 is installed for some included post processing utilities
  - this is optional, if the utilities are not desired
4. cd to `swSimroot/directory`
5. generate compilation instructions using `cmake -B build -S .`
6. compile with `make -C build`Create build files using CMAKE 3.13 or higher


# Creating a Case

The testing directory contains four test cases:
* 30x30ElongatedQausiIsoCFRPTestPlate <- necessary for Code Testing
* 120x60CrossPlyCFRPTestPlate
* 120x60CrossPlyCFRPTestPlate_course 
* 180x180ElongatedQausiIsoCFRPTestPlate

Each of these subdirectories contain a .py file that can be ran to generate the required input files. We recommend creating scripts to generate the input files. 


# Running a Case

mpiexec -np <number_of_processes> ./bin/rsg_sim Path/to/Inputs.xml

# Input File Requirements
 * Envelope size in voxels (x, y, & z).
 * Size of voxels in meters (x, y, & z).
 * Root name of the geometry file (example: 'Geometry' for Geometry.raw and Geometry_planCounts.raw)
 * Name of the rotations file (example: 'rotations.bin')
 * Total number of materials 
 * Materials with number corresponding to the values in the geometry file:
    - density
    - 21 elements or the stiffness Matrix
 * Total number of time steps to be simulated.
 * Size of the time step in seconds
 * Total number of excitations
 * Excitations with unique ID number:
    - radius of the excitation
    - location of the excitation in meters
    - initial time step the excitation is active
    - the duration of the excitation in time steps 
    - the orientation of the excitation (x, y, & z)
    - a scaling vector of the excitation (x, y, & z)






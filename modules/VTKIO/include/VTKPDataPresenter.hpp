/*! \headerfile VTKPDataPresenter.hpp "modules/VTKIO/include/VTKPDataPresenter.hpp"
* "VTKPDataPresenter.hpp" contains the presenter implementation for writing a parallel mesh
* and it's mesh-assigned data to vtk files.
*
*/

// Copyright 2020 United States Government as represented by the Administrator of the National 
// Aeronautics and Space Administration. No copyright is claimed in the United States under 
// Title 17, U.S. Code. All Other Rights Reserved. See Appendix A for 3rd party licenses.
//
// The Solid-Wave Sim (swSIM) platform is licensed under the Apache License, Version 2.0 (the 
// "License"); you may not use this file except in compliance with the License. You may obtain 
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0. 
// 
// Unless required by applicable law or agreed to in writing, software distributed under the 
// License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
// either express or implied. See the License for the specific language governing permissions 
// and limitations under the License.

#ifndef VTKPDATAPRESENTER_HPP
#define VTKPDATAPRESENTER_HPP

#include <unordered_map>
#include <string>
#include <memory>

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkHexahedron.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLPUnstructuredGridWriter.h>
//#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkMPIController.h>
#include <vtkMPICommunicator.h>
#include <vtkProcessGroup.h>

#include "DataPresenter.hpp"
#include "InspectionTarget.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Field.hpp"

#include <mpi.h>

namespace VTKIO {
  /*! \class VTKPDataPresenter VTKPDataPresenter.hpp "imodules/VTKIO/include/VTKPDataPresenter.hpp"
  *
  * Defines the implementation through which the main software can write parallel mesh and mesh assigned
  * data to  the vtk file. This interfaces provides for setting the mesh, the data on mesh, and committing 
  * that mesh and data to file.
  *
  */
  class VTKPDataPresenter : public Controller::DataPresenter {
    public:
      /*!
      * Constructor that builds the mesh communication using MPI and a user provided communicator
      * \param comm MPI communicator, defaults to MPI_COMM_WORLD
      */
      VTKPDataPresenter(MPI_Comm comm=MPI_COMM_WORLD){
        MPI_Comm_rank(MPI_COMM_WORLD, &rank[0]); //Assign a rank to every process
        MPI_Comm_size(MPI_COMM_WORLD, &Nparts[0]);  /* get number of nodes */
        MPI_Comm_rank(comm, &rank[1]); //Assign a rank to every process
        MPI_Comm_size(comm, &Nparts[1]);  /* get number of nodes */
        std::vector<int> localcomm_globalIds;
        localcomm_globalIds.resize(Nparts[1]);
        MPI_Allgather(&rank[0],1,MPI_INT,localcomm_globalIds.data(),1,MPI_INT,comm);

        vtkgroup->Initialize(vtkcomm->GetWorldCommunicator());
        vtkgroup->RemoveAllProcessIds();
        for(auto it=localcomm_globalIds.begin();it!=localcomm_globalIds.end();it++){
          vtkgroup->AddProcessId(*it);
        };
        vtkcomm->Initialize(vtkgroup);
        ctrlr->Initialize(0,0,1);
        ctrlr->SetCommunicator(vtkcomm);
        writer->SetController(ctrlr);
      };

      /*!
      * register a mesh with the Presenter for eventual file write.
      * \param target is a ModelUT structure containing the mesh and the material. The material 
      * shared pointer can be a nullptr, as it is unused. 
      */
      void setInspectedTarget(std::shared_ptr<ModelUT::InspectionTarget>& target) override {
        mesh=target->mesh;
        material=target->material;
        setVTKPoints();
        setVTKCells();
      };
      
      /*!
      * register a data field with the Presenter for eventual file write.
      * \param field is a ModelUT structure containing the data-on-mesh to be written.
      * \param name is the std::string name of the field by which it will be called in the 
      * eventually written file.
      */
      void addOverlayedDataFieldToTarget(std::shared_ptr<ModelUT::Field> field,std::string name) override {
        if(nullptr==mesh){
          auto tgt=std::make_shared<ModelUT::InspectionTarget>(ModelUT::InspectionTarget());
          tgt->mesh=field->mesh();
          setInspectedTarget(tgt);
        };
        outputfields.emplace(name,field);
        if(ModelUT::Field::CELL==field->type() || ModelUT::Field::CONSTANT==field->type()){
          addCellFieldDataToTarget(field,name);
        };
        if(ModelUT::Field::NODE==field->type() || ModelUT::Field::CONSTANT==field->type()){
          addNodeFieldDataToTarget(field,name);
        };
      };
      
      /*!
      * Instruct the presenter to invoke the file writer.
      * \param filename is the name of the file to be written.
      */
      void commitToFile(std::string filename) override {
        writer->SetFileName(filename.c_str());
        writer->SetInputData(uGridVTK);
        writer->SetNumberOfPieces(Nparts[1]);
        writer->SetStartPiece(rank[1]);
        writer->SetEndPiece(rank[1]);
        writer->Update();
        writer->Write();
      };

      /*!
      * adjust the number of partitions, if necessary
      */
      void setPartitionCount(int Npartitions){
        (this->Nparts)[1]=Npartitions;
      };

      /*!
      * destructor to clean up VTK/MPI artifacts
      */
      ~VTKPDataPresenter(){/*finalize the vtk mpi stuff*/};

    private:
      void addNodeFieldDataToTarget(std::shared_ptr<ModelUT::Field> field,std::string name){
        //vtkSmartPointer<vtkPointData> nodedata=vtkSmartPointer<vtkPointData>::New();
        vtkSmartPointer<vtkDoubleArray> data=vtkSmartPointer<vtkDoubleArray>::New();
        data->SetNumberOfComponents(1);
        data->SetName(name.c_str());
        for(int k=0;k<mesh->nodeCount();k++){
          data->InsertNextValue(field->at(k));
        };
        //nodedata.AddArray(data);
        uGridVTK->GetPointData()->AddArray(data);
      };
      void addCellFieldDataToTarget(std::shared_ptr<ModelUT::Field> field,std::string name){
        //vtkSmartPointer<vtkCellData> celldata=vtkSmartPointer<vtkCellData>::New();
        vtkSmartPointer<vtkDoubleArray> data=vtkSmartPointer<vtkDoubleArray>::New();
        data->SetNumberOfComponents(1);
        data->SetName(name.c_str());
        for(int k=0;k<mesh->cellCount();k++){
          data->InsertNextValue(field->at(k));
        };
        //celldata.AddArray(data);
        uGridVTK->GetCellData()->AddArray(data);
      };


      void setVTKPoints(){
        double coords[3];
        for(int32_t kn=0;kn<mesh->nodeCount();kn++){
          mesh->nodeCoordinate(kn,coords);
          nodeLocations->InsertNextPoint(coords);
        };
        uGridVTK->SetPoints(nodeLocations);
      };
      void setVTKCells(){
        int32_t cell_nodes[8];
        for(int32_t kc=0;kc<mesh->cellCount();kc++){
          mesh->cell(kc,cell_nodes);
          for(int k=0;k<8;k++){
            hexCell->GetPointIds()->SetId(k,cell_nodes[k]);
          };
          uGridVTK->InsertNextCell(hexCell->GetCellType(),
                                   hexCell->GetPointIds());
        };
        uGridVTK->BuildLinks();
      };

      int Nparts[2],rank[2];

      std::shared_ptr<ModelUT::Mesh> mesh=nullptr;
      std::shared_ptr<ModelUT::Material> material=nullptr;
      std::unordered_map<std::string,std::shared_ptr<ModelUT::Field>> outputfields;

      vtkSmartPointer<vtkPoints> nodeLocations=
                      vtkSmartPointer<vtkPoints>::New();
      vtkSmartPointer<vtkHexahedron> hexCell =
                      vtkSmartPointer<vtkHexahedron>::New();
      vtkSmartPointer<vtkUnstructuredGrid> uGridVTK =
                      vtkSmartPointer<vtkUnstructuredGrid>::New();

      vtkSmartPointer<vtkMPICommunicator> vtkcomm =
                      vtkSmartPointer<vtkMPICommunicator>::New();
      vtkSmartPointer<vtkProcessGroup> vtkgroup =
                      vtkSmartPointer<vtkProcessGroup>::New();
      vtkSmartPointer<vtkMPIController> ctrlr =
                      vtkSmartPointer<vtkMPIController>::New();
      vtkSmartPointer<vtkXMLPUnstructuredGridWriter> writer =
                      vtkSmartPointer<vtkXMLPUnstructuredGridWriter>::New();
  };
};

#endif /* VTKPDATAPRESENTER_HPP */
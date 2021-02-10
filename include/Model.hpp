/*! \headerfile Model.hpp "include/Model.hpp"
* "Model.hpp" contains the class definition encapsulating the 
* data structure interface for a Model in swSim.
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

#ifndef MODEL_HPP
#define MODEL_HPP

#include <mpi.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <stddef.h>

#include "Material.hpp"
#include "Excitation.hpp"
#include "Geometry.hpp"
#include "ParsableObject.hpp"

namespace swSim{
    /*! \class Model Model.hpp "include/Model.hpp"
    *
    * Defines data structures for Model in swSim
    */
    class Model: public ParsableObject {
    public:
        struct model_s {
            uint32_t numTimeSteps; /*!< Number of time steps in the simulation */
            double dt; /*!< Size of the time step in seconds */
            uint8_t numMaterials; /*!< Number of matrials in the model */
            uint8_t numExcitations; /*!< Number of excitations in the model */
            char inputFile[50]; /*!< Input file name */
            char inputPath[150]; /*!< Path to input file directory */
        };
        model_s modS; /*!< The Model struct for MPI passing */
        Geometry modGeometry; /*!< The geometry associated with the model */
        std::vector<Material> modMaterialsList; /*!< Vector of materials */
        std::vector<Excitation> modExcitationsList; /*!< Vector of Excitations */


        MPI_Datatype mpi_model; /*!< MPI data type for passing the Model struct*/
       // std::vector<std::string> parseNames; /*!< Vector of parse names to look for when reading inputs*/

        /*!
        * Create the Model
        */
    	Model();
        /*!
        * Destroy the Model
        */
    	~Model();
        /*!
        * Initilize data values
        */
        void Init(uint32_t TimeSteps, double dt, uint8_t numMat, uint8_t numExc);
        /*!
        * Initilize data values and file names
        */
        void Init(uint32_t TimeSteps, double dt, uint8_t numMat, uint8_t numExc,  const char *inFname, const char *inPname);
        /*!
        * Copy values of one model to another.
        */
        void Copy(Model *other);
        /*!
        * Create the MPI datatype for the model.
        */
        MPI_Datatype MPICreate();
        /*!
        * Provide parsing instructions for reading inputs associated with the model
        */
        void ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber) override;
        /*!
        * Takes input from a file and disperse it across all process
        */
        void Input(char *inputFileName, MPI_Comm m_mpi_comm);
        /*!
        * Prints all the elements of the Model
        */
        void Print(int rank); 
        /*!
        * Read the Input from the file
        */
        int readInput(char *inputFileName);
        /*!
        * Send the Input to other processes
        */
        void sendInput(MPI_Comm l_mpi_comm);
        /*!
        * Recieve the Input from another processes
        */
        void recieveInput(MPI_Comm l_mpi_comm);
        /*!
        * Return the path only
        */
        std::string parsePath(char *inputFileName);
        /*!
        * Return the filename only
        */
        std::string parseFile(char *inputFileName);


    };
};

#endif /*SWSIM_MODEL_H*/
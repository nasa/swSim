/*! \headerfile Excitation.hpp "include/Excitation.hpp"
* "Excitation.hpp" contains the class definition encapsulating the 
* data structure interface for a Excitation in a swSim model.
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

#ifndef EXCITATION_HPP
#define EXCITATION_HPP

#include <mpi.h>
#include <stdint.h>
#include <string>
#include <string.h>
#include <math.h>
#include <vector>
#include <stddef.h>

#include "ParsableObject.hpp"


namespace swSim{
    /*! \class Excitation Excitation.hpp "include/Excitation.hpp"
    *
    * Defines data structures for Excitation in swSim
    */
    class Excitation: public ParsableObject {   
    public:
    	struct excitation_s 
        {
            uint32_t excitationID; /*!< Unique identification number for this excitation */
            double location[3]; /*!< Location (in meters) of the Center of this Excitation */
            double radius; /*!< Radius (in meters) of this Excitation */

            bool driven; /*!< A drive function fo this transducer exsists, i.e. it is not a reciever */

            uint32_t initTimeStep; /*!< The timestep on which the excitation begins */
            uint32_t excitationNumTimeSteps; /*!< The length, in time steps of the excitation. */
            char waveformFile[50]; /*!< The Waveform file name. */

            float scalingVector[3]; /*!< The 3D scaling vector for x, y, z dimensions. */
            float orientNormal[3]; /*!< The Orientation of the transducer */

            int excitationProcess; //*!< the process on which the center element of the transducer resides.*/
            uint64_t excitationCenterElement; //*!< the center element on which the transducer resides.*/
            uint64_t excitationEdgeElement; /*!< and edge element on which element on which the transducer resides on the same process as the center element.*/      
        };

    	excitation_s excS; /*!< Excitation struct */
    	MPI_Datatype mpi_excitation;  /*!<  A MPI_Datatype for the opject. */
        /*!
        * Create the Excitation
        */
    	Excitation(); 
        /*!
        * Destroy the Excitation
        */
        ~Excitation();
        /*!
        * Initilize data values
        */
    	void Init(uint32_t id, double *pos, double rad, bool drvn, uint32_t tinit, uint32_t drlen, const char *fname, float *sF, float *orient);
        /*!
        * Copy values of one Excitation to another.
        */
        void Copy(Excitation *other);
        /*!
        * Create the MPI datatype for the Excitation.
        */ 
    	MPI_Datatype MPICreate();
        /*!
        * Provide parsing instructions for reading inputs associated with the Excitation
        */
        void ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber) override;
        /////// TO DO:  Remove Location Check //////
        bool LocationCheck(std::vector<double> &nCoords); 

    private:
        

    };
};

#endif

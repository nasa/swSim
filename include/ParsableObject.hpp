/*! \headerfile ParsableObject.hpp "include/ParsableObject.hpp"
* "ParsableObject.hpp" contains the class definition encapsulating the 
* data structure interface for a creating all objects from inputs in a swSim Model.
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

#include <stdint.h>
#include <fstream>  
#include <vector>
#include <string>

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>


#ifndef PARSABLEOBJECT_HPP
#define PARSABLEOBJECT_HPP

namespace swSim{
    /*! \class ParsableObject ParsableObject.hpp "include/ParsableObject.hpp"
    *
    * Defines data structures for ParsableObject in swSim
    */
    class ParsableObject {
    public:
        /*!
        * Function to help parse the input
        */
        virtual void ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber);
        /*!
        * Parse the specific object
        */
        void Parse(xmlDocPtr doc, xmlNodePtr cur); 
        std::vector<std::string> parseNames; /*!< Vector of parse names to look for when reading inputs*/
        /*!
        * Search to the xml element corespondng to each parsable variable.
        */
        xmlNodePtr ParseSearch(xmlDocPtr doc, xmlNodePtr cur, const char *value); 
        /*!
        * Parse a double object
        */
        void Do_parse(xmlDocPtr doc, xmlNodePtr cur, double *value);
        /*!
        * Parse a float object
        */
        void Do_parse(xmlDocPtr doc, xmlNodePtr cur, float *value);
        /*!
        * Parse a uint_8 object
        */
        void Do_parse(xmlDocPtr doc, xmlNodePtr cur, uint8_t *value);
        /*!
        * Parse a uint_32 object
        */
        void Do_parse(xmlDocPtr doc, xmlNodePtr cur, uint32_t *value);
        /*!
        * Parse a uint_64 object
        */
        void Do_parse(xmlDocPtr doc, xmlNodePtr cur, uint64_t *value);
        /*!
        * Parse a boolean object
        */
        void Do_parse(xmlDocPtr doc, xmlNodePtr cur, bool *value);
        /*!
        * Parse a char array object
        */
        void Do_parse(xmlDocPtr doc, xmlNodePtr cur, char *value);
    };
};

#endif
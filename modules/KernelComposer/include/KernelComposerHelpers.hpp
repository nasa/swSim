/*! \headerfile KernelComposerHelpers.hpp "include/KernelComposerHelpers.hpp"
* "KernelComposerHelpers.hpp" contains the error information structures and
* parsers, along with any other miscellaneous helper functions for which utility
* can be found and deemed worth implementing.
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

#ifndef SWSIM_KERNEL_COMPOSER_HELPERS_H
#define SWSIM_KERNEL_COMPOSER_HELPERS_H

namespace swSim{
    /*!
    * enum type returning error/warning states 
    */
    enum KernelComposerError_t{
        KC_OK=0,/*!< no warnings or errors */
        WARN_UNCOMMITTED,/*!< warning flag indicating device has not been committed when the calling function expects the device to have already been committed */
        WARN_COMMITTED,/*!< warning flag indicating device has been committed when the calling function expects the device to have not already been committed */
        WARN_INVALID_KEY,/*!< Invalid key provided to operation construction */
        ERR_INVALID_KEY,/*!< Invalid key found during validation [not currently implemented] */
        ERR_INVALID_OP,/*!< Invalid operation case called during execution */
        ERR_UNKNOWN /*!< unknown error encountered. */
    };
    
};

#endif /*SWSIM_KERNEL_COMPOSER_HELPERS_H*/
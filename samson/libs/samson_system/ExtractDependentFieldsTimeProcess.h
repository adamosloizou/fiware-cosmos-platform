/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef _H_SAMSON_system_EXTRACT_DEPENDENT_FIELDS_TIME_PROCESS
#define _H_SAMSON_system_EXTRACT_DEPENDENT_FIELDS_TIME_PROCESS

#include "samson_system/ProcessComponent.h"

namespace samson{ namespace system{

    class ExtractDependentFieldsTimeProcess : public ProcessComponent
    {
        std::vector < std::string> dependent_concepts_;
        std::string independent_concept_;
        std::string out_app_name_;
        std::string out_def_name_;

    public:

        ExtractDependentFieldsTimeProcess( ): ProcessComponent("default")
        {
            out_app_name_ = "def_out";
            out_def_name_ = "def_def";
        };

        ExtractDependentFieldsTimeProcess( std::string name, std::vector <std::string> dependent_concepts, std::string independent_concept, std::string out_app, std::string out_def ): ProcessComponent(name)
        {
            dependent_concepts_ = dependent_concepts;
            independent_concept_ = independent_concept;
            out_app_name_ = out_app;
            out_def_name_ = out_def;
        }

        std::string out_app_name() { return out_app_name_; }
        std::string out_def_name() { return out_def_name_; }

        // Update this state based on input values ( return true if this state has been updated with this component )
        bool update( Value* key ,
                            Value * state ,
                            Value ** values ,
                            size_t num_values ,
                            samson::KVWriter *writer );
    };

} } // End of namespace

#endif

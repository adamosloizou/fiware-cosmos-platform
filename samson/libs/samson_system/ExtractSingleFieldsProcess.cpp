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

#include "samson_system/ValueContainer.h"

#include "ExtractSingleFieldsProcess.h" // Own interface


namespace samson{ namespace system{

bool ExtractSingleFieldsProcess::update( Value* key ,Value * state ,Value ** values ,size_t num_values ,samson::KVWriter *writer )
{
    Value new_key;
    Value new_value;

    if (key->checkMapValue("app", name().c_str()))
    {
        std::vector<std::string> keys = key->get_keys_from_map();
        for (size_t i = 0; (i < keys.size()); i++)
        {
            for (size_t j = 0; (j < concepts_.size()); j++)
            {
                if (keys[i] == concepts_[j])
                {
                    //new_key.set_as_void();
                    new_key.set_as_map();
                    new_key.add_value_to_map("app")->set_string(out_app_name().c_str());
                    new_key.add_value_to_map("concept")->set_string(keys[i].c_str());
                    Value *p_value = key->get_value_from_map(keys[i].c_str());
                    if (p_value->isVector())
                    {
                        for (size_t k = 0; (k < p_value->get_vector_size()); k++)
                        {
                            new_value.set_string(p_value->get_string());
                            for (size_t j = 0; (j < num_values); j++)
                            {
                                emit_output(&new_key, &new_value, writer);
                            }
                        }
                    }
                    else if (p_value->isMap())
                    {
                        LM_E(("Error, don't know how to distribute concept '%s' being a map", keys[i].c_str()));
                        continue;
                    }
                    else
                    {
                        new_value.set_string(p_value->get_string());
                        for (size_t j = 0; (j < num_values); j++)
                        {
                            emit_output(&new_key, &new_value, writer);
                        }
                    }
                }
            }
        }
        key->set_string_for_map("app", out_def_name().c_str());
        for (size_t j = 0; (j < num_values); j++)
        {
            emit_feedback(key, values[j], writer);
        }
        return true;
    }
    else
    {
        return false;
    }
}



}} // End of namespace

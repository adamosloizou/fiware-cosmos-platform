
#include "samson_system/ValueContainer.h"

#include "HitCountProcess.h" // Own interface

#include <vector>
#include <algorithm>

namespace samson{ namespace system{

double HitCountProcess::update_count(double old_count, size_t old_time, size_t new_time)
{
    int diff = new_time - old_time;
    return (old_count * pow(forgetting_factor_, diff ));
}

struct st_hit
{
        std::string item;
        double count;
        double time;
};
static bool sort_hit_by_count(struct st_hit h1, struct st_hit h2)
{
    return h1.count > h2.count;
}

bool HitCountProcess::update( Value* key ,Value * state ,Value ** values ,size_t num_values ,samson::KVWriter *writer )
{

    Value *p_hit;

    std::string input_item;
    std::string item;

    size_t newest_timestamp = 0;

    if (key->checkMapValue("app", name().c_str()))
    {
        LM_M(("Detected app:'%s'", name().c_str()));
        if (state->isVoid())
        {
            LM_M(("Init state for key app:'%s', concept:'%s'", name().c_str(), key->get_string_from_map("concept")));
            state->set_as_vector();
        }
        else
        {
            LM_M(("Existing state for key app:'%s', concept:'%s', with %lu items", name().c_str(), key->get_string_from_map("concept"), state->get_vector_size()));
        }

        for (size_t i = 0; (i < num_values); i++)
        {
            input_item = values[i]->get_string_from_map("item");
            size_t timestamp = values[i]->get_double_from_map("time");
            if (timestamp == 0)
            {
                LM_W(("Warning, for concept:'%s', item:'%s' timestamp==0 ", input_item.c_str(), key->get_string_from_map("concept")));
            }
            if (timestamp > newest_timestamp)
            {
                newest_timestamp = timestamp;
            }
            Value *p_hit = NULL;
            bool found_hit = false;
            // When logs arrive in block from push, values are grouped,
            // so more efficient to look backwards.
            for (ssize_t j = state->get_vector_size()-1; (j >= 0); j--)
            {
                p_hit = state->get_value_from_vector(j);
                if (input_item == p_hit->get_string_from_map("item"))
                {
                    double old_count = p_hit->get_double_from_map("count");
                    size_t old_timestamp = p_hit->get_double_from_map("time");
                    double new_count = 1 + update_count(old_count, old_timestamp, timestamp);
                    p_hit->set_double_for_map("time", timestamp);
                    p_hit->set_double_for_map("count", new_count);
                    //LM_M(("Particular update count for item:'%s'(%lu of %lu), old_count:%lf, old_time:%lu, new_count:%lf, new_time:%lu",  input_item.c_str(), j, state->get_vector_size(), old_count, old_timestamp, new_count, timestamp));

                    found_hit = true;
                    break;
                }
            }
            if (found_hit == false)
            {
                Value *new_hit = state->add_value_to_vector(state->get_vector_size());
                new_hit->set_as_map();
                new_hit->add_value_to_map("item")->set_string(input_item);
                new_hit->add_value_to_map("time")->set_double(static_cast<double>(timestamp));
                new_hit->add_value_to_map("count")->set_double(1.0);
                //LM_M(("Added to state item:'%s'(count:%lf, time:%lf), now size:%lu", input_item.c_str(), new_hit->get_double_from_map("count", 0.0), new_hit->get_double_from_map("time", 0.0), state->get_vector_size()));
            }
        }

        if (newest_timestamp == 0)
        {
            LM_W(("Warning, for concept:'%s', newest_timestamp==0 with %lu values", key->get_string_from_map("concept"), num_values));
        }
        if (num_values > 0)
        {
            // We want only to update state count if we had had input values, and thus a newest_timestamp
            for (size_t j = 0; (j < state->get_vector_size()); j++)
            {
                p_hit = state->get_value_from_vector(j);
                double old_count = p_hit->get_double_from_map("count");
                size_t old_timestamp = p_hit->get_double_from_map("time");
                double new_count = update_count(old_count, old_timestamp, newest_timestamp);
                p_hit->set_double_for_map("time", newest_timestamp);
                p_hit->set_double_for_map("count", new_count);
                //LM_M(("General update count for item:'%s'(%lu of %lu), old_count:%lf, old_time:%lu, new_count:%lf, new_time:%lu",  p_hit->get_string_from_map("item"), j, state->get_vector_size(), old_count, old_timestamp, new_count, newest_timestamp));
            }
        }


        // If we have no so many elements in the value, we can work directly with it.
        // If not, we prefer to sort a standard vector, to prune it and to convert it to value
        if (state->get_vector_size() < 1000)
        {

            LM_M(("Start value sort and prune phase for %lu items", state->get_vector_size()));

            // simple bubble sort implementation. Difficult to use sort() with Value

            for (ssize_t i = (state->get_vector_size()-1); (i > 0); i--)
            {
                for (ssize_t j = 1; (j <= i); j++)
                {
                    if (state->get_value_from_vector(j-1)->get_double_from_map("count") <
                            state->get_value_from_vector(j)->get_double_from_map("count"))
                    {
                        //LM_M(("Swapping item j(%lu,'%s',%lf) with (%lu,'%s',%lf)", j, state->get_value_from_vector(j)->get_string_from_map("item"), state->get_value_from_vector(j)->get_double_from_map("count"), j+1, state->get_value_from_vector(j+1)->get_string_from_map("item"), state->get_value_from_vector(j+1)->get_double_from_map("count")));
                        state->swap_vector_components(j-1, j);
                    }
                }
            }
            // Pruning the least relevant items (keeping a large margin)
            while (state->get_vector_size() > n_top_items_)
            {
                //LM_M(("Pruning items from size:%lu to %lu", state->get_vector_size(), n_top_items_));
                state->pop_back_from_vector();
            }

            LM_M(("End value sort and prune phase for %lu items", state->get_vector_size()));

        }
        else
        {
            LM_M(("Start vector sort and prune phase for %lu items", state->get_vector_size()));

            std::vector<struct st_hit> temp_vector(state->get_vector_size());
            for (size_t i = 0; (i < state->get_vector_size()); i++)
            {
                temp_vector[i].item = state->get_value_from_vector(i)->get_string_from_map("item");
                temp_vector[i].count = state->get_value_from_vector(i)->get_double_from_map("count");
                temp_vector[i].time = state->get_value_from_vector(i)->get_double_from_map("time");
            }

            LM_M(("End copy for %lu items", state->get_vector_size()));

            std::sort(temp_vector.begin(), temp_vector.end(), sort_hit_by_count);

            LM_M(("End sort for %lu items", state->get_vector_size()));

            state->clear();

            LM_M(("End sort for %lu items", state->get_vector_size()));


            for (size_t i = 0; (i < n_top_items_); i++)
            {
                Value *new_hit = state->add_value_to_vector(state->get_vector_size());
                new_hit->set_as_map();
                new_hit->add_value_to_map("item")->set_string(temp_vector[i].item);
                new_hit->add_value_to_map("time")->set_double(temp_vector[i].time);
                new_hit->add_value_to_map("count")->set_double(temp_vector[i].count);
                LM_M(("Recovered back to state item:'%s'(count:%lf, time:%lf), at pos:%lu", new_hit->get_string_from_map("item"), new_hit->get_double_from_map("count", 0.0), new_hit->get_double_from_map("time", 0.0), i));
            }
            temp_vector.clear();

            LM_M(("End vector sort and prune phase for %lu items", state->get_vector_size()));

        }



        for (size_t i = 0; (i < state->get_vector_size()); i++)
        {
            LM_M(("Pruned state for concept:'%s,  item i(%lu,'%s',%lf) of %lu", key->get_string_from_map("concept"), i, state->get_value_from_vector(i)->get_string_from_map("item"), state->get_value_from_vector(i)->get_double_from_map("count"), state->get_vector_size()));
        }

        Value new_key;
        new_key.set_as_map();
        new_key.add_value_to_map("app")->set_string(out_app_name().c_str());
        new_key.add_value_to_map("concept")->set_string(key->get_string_from_map("concept"));

        LM_M(("Before emiting output  for concept:'%s' with %lu items", new_key.get_string_from_map("concept"), state->get_vector_size()));

        emit_output(&new_key, state, writer);

        for (size_t i = 0; (i < state->get_vector_size()); i++)
        {
            LM_M(("Emiting state for concept:'%s,  item i(%lu,'%s',%lf) of %lu", key->get_string_from_map("concept"), i, state->get_value_from_vector(i)->get_string_from_map("item"), state->get_value_from_vector(i)->get_double_from_map("count"), state->get_vector_size()));
        }

        LM_M(("Before emiting state for concept:'%s' with %lu items", key->get_string_from_map("concept"), state->get_vector_size()));

        emit_state(key, state, writer);

        for (size_t i = 0; (i < state->get_vector_size()); i++)
        {
            LM_M(("Emiting state for concept:'%s,  item i(%lu,'%s',%lf) of %lu", key->get_string_from_map("concept"), i, state->get_value_from_vector(i)->get_string_from_map("item"), state->get_value_from_vector(i)->get_double_from_map("count"), state->get_vector_size()));
        }

        LM_M(("After emiting state for concept:'%s' with %lu items", key->get_string_from_map("concept"), state->get_vector_size()));


        key->set_string_for_map("app", out_def_name().c_str());
        for (size_t j = 0; (j < num_values); j++)
        {
            emit_feedback(key, values[j], writer);
        }
        return true;
    }
    else
    {
        LM_M(("key app:'%s' different from expected", key->get_string_from_map("app"), name().c_str()));
        return false;
    }
}



}} // End of namespace

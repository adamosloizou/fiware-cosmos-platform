
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_level_update_valTimed_graph
#define _H_SAMSON_level_update_valTimed_graph


#include <samson/module/samson.h>
#include <samson/modules/level/GraphLine.h>
#include <samson/modules/level/ValTimed.h>
#include <samson/modules/system/String.h>

#include "logMsg/logMsg.h"

namespace samson{
namespace level{


class update_valTimed_graph : public samson::Reduce
{

    samson::system::String key;
    samson::level::ValTimed value;
    samson::level::GraphLine curve;

    public:


    //  INFO_MODULE
    // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
    // Please, do not remove this comments, as it will be used to check consistency on module declaration
    //
    //  input: system.String level.ValTimed
    //  input: system.String level.GraphLine
    //  output: system.String level.GraphLine
    //
    //  END_INFO_MODULE

    void init( samson::KVWriter *writer )
    {
    }

    void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
    {
        if (inputs[1].num_kvs == 0)
        {
            key.parse(inputs[0].kvs[0]->key);
            curve.Init();
            LM_M(("Curve created for item:'%s'", key.value.c_str()));
        }
        else
        {
            key.parse(inputs[1].kvs[0]->key);
            // Previous state
            curve.parse(inputs[1].kvs[0]->value); // We only consider one state
            //LM_M(("Already existing curve for item:'%s'", key.value.c_str()));

            if (inputs[1].num_kvs > 1)
            {
                LM_W(("Multiple curves(%lu) for item:'%s'", inputs[1].num_kvs, key.value.c_str()));
            }
        }

        int pos_hours = curve.NUMBER_OF_SECONDS.value + curve.NUMBER_OF_MINUTES.value;
        int pos_minutes = curve.NUMBER_OF_SECONDS.value;

        //LM_M(("pos_minutes:%d, pos_hours:%d", pos_minutes, pos_hours));


        //LM_M(("For item:'%s', %lu values, with p_coord_seconds:%p", key.value.c_str(), inputs[0].num_kvs, curve.coord));
        for (uint64_t i = 0; i < inputs[0].num_kvs; i++)
        {
            value.parse(inputs[0].kvs[i]->value);

            if (value.t.value < curve.prev_timestamp.value)
            {
                LM_W(("Value %d (at %d) out of order for item:'%s', hit:%s, prev:%s", value.val.value, i, key.value.c_str(), value.t.str().c_str(), curve.prev_timestamp.str().c_str()));
                continue;
            }
            else
            {
                //LM_M(("Value %d (at %d) in right order for item:'%s', hit:%s, prev:%s", value.val.value, i, key.value.c_str(), value.t.str().c_str(), curve.prev_timestamp.str().c_str()));
            }

            if (curve.prev_timestamp.value != 0)
            {
                uint64_t inc_time = value.t.value - curve.prev_timestamp.value;
                curve.nsecs_since_shift.value += inc_time;
                //LM_M(("Updated nsecs_since_shift:%d (inc_time:%lu) with p_coord_seconds:%p (pos_minutes:%d, pos_hours:%d)", curve.nsecs_since_shift.value, inc_time, curve.coord, pos_minutes, pos_hours));
                while (curve.nsecs_since_shift.value >= curve.NUMBER_OF_SECONDS.value)
                {
                    curve.nmins_since_shift.value++;
                    if (curve.nmins_since_shift.value >= curve.NUMBER_OF_MINUTES.value)
                    {
                        curve.shift_hours(1);
                        //LM_M(("Shift hours for key:'%s'", key.value.c_str()));
                        curve.coord[pos_hours].x[1].value = curve.average(&(curve.coord[pos_minutes]), curve.NUMBER_OF_MINUTES.value);
                        //LM_M(("For key:'%s', initial hour assigned to coord[%d]:%d", key.value.c_str(), pos_hours, curve.coord[pos_hours].x[1].value));
                        curve.nmins_since_shift.value -= curve.NUMBER_OF_MINUTES.value;
                    }
                    //LM_M(("pos_minutes:%d, pos_hours:%d", pos_minutes, pos_hours));
                    curve.shift_minutes(1);
                    //LM_M(("pos_minutes:%d, pos_hours:%d", pos_minutes, pos_hours));
                    //LM_M(("Shift minutes for key:'%s'", key.value.c_str()));
                    curve.coord[pos_minutes].x[1].value = curve.average(&(curve.coord[0]), curve.NUMBER_OF_SECONDS.value);
                    //LM_M(("pos_minutes:%d, pos_hours:%d", pos_minutes, pos_hours));
                    //LM_M(("For key:'%s', initial minute assigned to coord[%d]:%d", key.value.c_str(), pos_minutes, curve.coord[pos_minutes].x[1].value));
                    curve.nsecs_since_shift.value -= curve.NUMBER_OF_SECONDS.value;
                }
                if (inc_time > static_cast<uint64_t>(curve.NUMBER_OF_SECONDS.value))
                {
                    //LM_M(("Clipping inc_time:%d", inc_time));
                    inc_time = curve.NUMBER_OF_SECONDS.value - 1;
                    curve.nsecs_since_shift.value = 0;
                }
                curve.shift_seconds(inc_time);
                //LM_M(("Shift seconds for key:'%s'", key.value.c_str()));
            }
            curve.coord[0].x[1].value = value.val.value;
            //LM_M(("For key:'%s', initial second assigned to coord[%d]:%d", key.value.c_str(), 0, curve.coord[0].x[1].value));
            curve.prev_timestamp.value = value.t.value;
        }

        //LM_M(("Emit curve for item:'%s'", key.value.c_str()));
        writer->emit(0, &key, &curve);
    }

    void finish( samson::KVWriter *writer )
    {
    }



};


} // end of namespace level
} // end of namespace samson

#endif

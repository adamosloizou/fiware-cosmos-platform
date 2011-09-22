
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_borrar_vector_client_out
#define _H_SAMSON_mob2_borrar_vector_client_out


#include <stdint.h>
#include <samson/module/samson.h>
#include <samson/modules/mob2/ClusterVector.h>
#include <samson/modules/mob2/Node_Bts.h>



namespace samson{
namespace mob2{


	class borrar_vector_client_out : public samson::ParserOut
	{
        //Inputs
        Node_Bts nodbts;
        ClusterVector clusvect;

	public:

#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif
#define MAX_STR_LEN 1024
                        char output[MAX_STR_LEN];


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: mob2.Node_Bts mob2.ClusterVector  

extendedHelp: 		Parse out of client vectors

#endif // de INFO_COMMENT

		void init(TXTWriter *writer )
		{
		}

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{

        for(uint64_t i=0; i<inputs[0].num_kvs; i++)
        {
        size_t total = 0;
                nodbts.parse(inputs[0].kvs[i]->key);
                clusvect.parse(inputs[0].kvs[i]->value);
                total += snprintf( output+total, MAX_STR_LEN-total,"%lu",nodbts.phone.value);
                for(int j=0; j<clusvect.coms_length; j++)
                {
                        total += snprintf( output+total, MAX_STR_LEN-total,"|%f",clusvect.coms[j].value);
                }
                total += snprintf( output+total, MAX_STR_LEN-total,"\n");
		writer->emit(output);

        }

		}

		void finish(TXTWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif

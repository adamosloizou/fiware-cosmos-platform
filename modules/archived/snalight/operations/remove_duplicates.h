
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_snalight_remove_duplicates
#define _H_SAMSON_snalight_remove_duplicates


#include <samson/module/samson.h>


namespace samson{
namespace snalight{


	class remove_duplicates : public samson::Reduce
	{
		  samson::system::UInt key;
		  CDR cdr;
		  CDR cdrPrev;
	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		  if( inputs[0].num_kvs == 0)
		    return;

			//Parse and emit the first one...
		  key.parse(inputs[0].kvs[0]->key );
		  cdrPrev.parse( inputs[0].kvs[0]->value );
		  writer->emit(0, &key, &cdrPrev);

		  for (size_t i=1 ; i < inputs[0].num_kvs ; ++i) {

			  cdr.parse(inputs[0].kvs[i]->value);

			  if (cdrPrev.isEqual(&cdr) == false)
			  {
				  cdrPrev = cdr;
				  writer->emit(0, &key, &cdrPrev);
			  }
		  }
		}


	};


} // end of namespace samson
} // end of namespace snalight

#endif

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_prime_filter_primes
#define _H_SAMSON_prime_filter_primes


#include <samson/module/samson.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Void.h>


namespace samson{
namespace prime{


	class filter_primes : public samson::Reduce
	{

	   samson::system::UInt key;
	   samson::system::Void value;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt system.Void  
output: system.UInt system.Void

helpLine: Only emit at the output input keys that appear only once... prime numbers
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{

		   if( inputs[0].num_kvs == 1 )
		   {
              key.parse( inputs[0].kvs[0]->key );
			  writer->emit( 0 , &key , &value );
		   }
		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace prime
} // end of namespace samson

#endif
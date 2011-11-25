
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_Vector_UInt
#define _H_SAMSON_system_Vector_UInt

#include "logMsg/logMsg.h"
#include <samson/modules/system/Vector_UInt_base.h>


namespace samson{
namespace system{


	class Vector_UInt : public Vector_UInt_base
	{
public:
		int valuesAddSorted(UInt _value){
			valuesSetLength( values_length + 1 );
			for (int i = 0; (i < (values_length-1)); i++)
			{
				if (_value.value <= values[i].value)
				{
					for (int j = (values_length-1); (j > i); j--)
					{
						values[j].value = values[j-1].value;
					}
					values[i] = _value.value;
					return i;
				}
			}
			values[values_length-1] = _value;
			return (values_length-1);
		}

		int valuesAddAtIndex(UInt _value, int index){
			if (index > values_length)
			{
				return (-1);
			}
			valuesSetLength( values_length + 1 );

			for (int j = (values_length-1); (j > index); j--)
			{
				values[j].value = values[j-1].value;
			}
			values[index].value = _value.value;
			return index;
		}
	};


} // end of namespace samson
} // end of namespace system

#endif
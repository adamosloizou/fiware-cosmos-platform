
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_test_compareFunctions_User_BASE
#define _H_SAMSON_test_compareFunctions_User_BASE


#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt8.h>
#include <samson/modules/test_compareFunctions/Address.h>


namespace samson{
namespace test_compareFunctions{


	class User_base : public samson::DataInstance{

	public:
	::samson::system::UInt8 __filledOptFields__;
	::samson::system::UInt id;
	::samson::system::String surname;
	::samson::test_compareFunctions::Address address;
	::samson::system::UInt8 age;
	::samson::system::UInt companyId;
	::samson::system::UInt expense;

	User_base() : samson::DataInstance(){
		__filledOptFields__ = 0;
	}

	~User_base() {
	}

	void unassignAllOptionals(){
		__filledOptFields__ = 0;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing __filledOptFields__
			offset += __filledOptFields__.parse(data+offset);
		}

		{ //Parsing id
			offset += id.parse(data+offset);
		}

		{ //Parsing surname
			offset += surname.parse(data+offset);
		}

		if (__filledOptFields__.value & 0x1)
		{ //Parsing address
			offset += address.parse(data+offset);
		}

		if (__filledOptFields__.value & 0x2)
		{ //Parsing age
			offset += age.parse(data+offset);
		}

		{ //Parsing companyId
			offset += companyId.parse(data+offset);
		}

		if (__filledOptFields__.value & 0x4)
		{ //Parsing expense
			offset += expense.parse(data+offset);
		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serializing __filledOptFields__
			offset += __filledOptFields__.serialize(data+offset);
		}

		{ //Serializing id
			offset += id.serialize(data+offset);
		}

		{ //Serializing surname
			offset += surname.serialize(data+offset);
		}

		if (__filledOptFields__.value & 0x1)
		{ //Serializing address
			offset += address.serialize(data+offset);
		}

		if (__filledOptFields__.value & 0x2)
		{ //Serializing age
			offset += age.serialize(data+offset);
		}

		{ //Serializing companyId
			offset += companyId.serialize(data+offset);
		}

		if (__filledOptFields__.value & 0x4)
		{ //Serializing expense
			offset += expense.serialize(data+offset);
		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		::samson::system::UInt8 local__filledOptFields__;
		{ //Parsing __filledOptFields__
			offset += local__filledOptFields__.parse(data+offset);
		}

		{ //Sizing id
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing surname
			offset += ::samson::system::String::size(data+offset);
		}

		if (local__filledOptFields__.value & 0x1)
		{ //Sizing address
			offset += ::samson::test_compareFunctions::Address::size(data+offset);
		}

		if (local__filledOptFields__.value & 0x2)
		{ //Sizing age
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing companyId
			offset += ::samson::system::UInt::size(data+offset);
		}

		if (local__filledOptFields__.value & 0x4)
		{ //Sizing expense
			offset += ::samson::system::UInt::size(data+offset);
		}

		return offset;
	}

	int hash(int max_num_partitions){
		{ //Partitioning id
			return id.hash(max_num_partitions);
		}

	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		::samson::system::UInt8 local__filledOptFields__1;
		(*offset1) += local__filledOptFields__1.parse(data1+*offset1);
		::samson::system::UInt8 local__filledOptFields__2;
		(*offset2) += local__filledOptFields__2.parse(data2+*offset2);
		{ // comparing id
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  id compared 
		{ // comparing surname
			int tmp = ::samson::system::String::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  surname compared 
		{ // comparing address
			if (((local__filledOptFields__1.value & 0x1) != 0) && ((local__filledOptFields__2.value & 0x1) == 0))
			{
				return 1;
			}
			else if (((local__filledOptFields__1.value & 0x1) == 0) && ((local__filledOptFields__2.value & 0x1) != 0))
			{
				return -1;
			}
			else if ((local__filledOptFields__1.value & 0x1) && (local__filledOptFields__2.value & 0x1))
			{
				int tmp = ::samson::test_compareFunctions::Address::compare(data1,data2,offset1 , offset2);
				if( tmp != 0) return tmp;
			}
		}   //  address compared 
		{ // comparing age
			if (((local__filledOptFields__1.value & 0x2) != 0) && ((local__filledOptFields__2.value & 0x2) == 0))
			{
				return 1;
			}
			else if (((local__filledOptFields__1.value & 0x2) == 0) && ((local__filledOptFields__2.value & 0x2) != 0))
			{
				return -1;
			}
			else if ((local__filledOptFields__1.value & 0x2) && (local__filledOptFields__2.value & 0x2))
			{
				int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
				if( tmp != 0) return tmp;
			}
		}   //  age compared 
		{ // comparing companyId
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  companyId compared 
		{ // comparing expense
			if (((local__filledOptFields__1.value & 0x4) != 0) && ((local__filledOptFields__2.value & 0x4) == 0))
			{
				return 1;
			}
			else if (((local__filledOptFields__1.value & 0x4) == 0) && ((local__filledOptFields__2.value & 0x4) != 0))
			{
				return -1;
			}
			else if ((local__filledOptFields__1.value & 0x4) && (local__filledOptFields__2.value & 0x4))
			{
				int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
				if( tmp != 0) return tmp;
			}
		}   //  expense compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}




	void address_SetAssigned(bool _assign)
	{
		if (_assign)
		{
			__filledOptFields__.value |= 0x1;
		}
		else
		{
			__filledOptFields__.value &= ~0x1;
		}
	}
	bool address_GetAssigned(void)
	{
		return (__filledOptFields__.value & 0x1);
	}

	void age_SetAssigned(bool _assign)
	{
		if (_assign)
		{
			__filledOptFields__.value |= 0x2;
		}
		else
		{
			__filledOptFields__.value &= ~0x2;
		}
	}
	bool age_GetAssigned(void)
	{
		return (__filledOptFields__.value & 0x2);
	}


	void expense_SetAssigned(bool _assign)
	{
		if (_assign)
		{
			__filledOptFields__.value |= 0x4;
		}
		else
		{
			__filledOptFields__.value &= ~0x4;
		}
	}
	bool expense_GetAssigned(void)
	{
		return (__filledOptFields__.value & 0x4);
	}


	static int *getDataPath(const std::string &dataPathString){
		const char *dataPathCharP = dataPathString.c_str();
		int nlevels = 1;
		int *dataPathIntP;

		const char *p_sep = dataPathCharP;
		while ((p_sep = strchr(p_sep, '.')) != NULL)
		{
			nlevels++;
			p_sep++;
		}

		if ((dataPathIntP = (int *)malloc((nlevels + 1)*sizeof(int))) == NULL)
		{
			return ((int *)NULL);
		}

		int retError = getDataPath(dataPathCharP, dataPathIntP);

		if (retError)
		{
			free(dataPathIntP);
			dataPathIntP = NULL;
		}

		return  (dataPathIntP);
	}

	static int getDataPath(const char * dataPathCharP, int *dataPathIntP){
		if (strcmp(dataPathCharP, "User") == 0)
		{
			*dataPathIntP = -1;
			return (0);
		}

		if (strncmp(dataPathCharP, "User.", strlen("User.")) == 0)
		{
			return (getDataPath(dataPathCharP+strlen("User."), dataPathIntP));
		}
		{ //Scanning id, terminal and non-terminal
			if (strcmp(dataPathCharP, "id") == 0)
			{
				*dataPathIntP = 0;
				*(dataPathIntP+1) = -1;
				return (0);
			}
			if (strncmp(dataPathCharP, "id.", strlen("id.")) == 0)
			{
				*dataPathIntP = 0;
				*(dataPathIntP+1) = -1;
				return (0);
			}
		}
		{ //Scanning surname, terminal and non-terminal
			if (strcmp(dataPathCharP, "surname") == 0)
			{
				*dataPathIntP = 1;
				*(dataPathIntP+1) = -1;
				return (0);
			}
			if (strncmp(dataPathCharP, "surname.", strlen("surname.")) == 0)
			{
				*dataPathIntP = 1;
				*(dataPathIntP+1) = -1;
				return (0);
			}
		}
		{ //Scanning address, terminal and non-terminal
			if (strcmp(dataPathCharP, "address") == 0)
			{
				*dataPathIntP = 2;
				*(dataPathIntP+1) = -1;
				return (0);
			}
			if (strncmp(dataPathCharP, "address.", strlen("address.")) == 0)
			{
				*dataPathIntP = 2;
				*(dataPathIntP+1) = -1;
				return (0);
			}
		}
		{ //Scanning age, terminal and non-terminal
			if (strcmp(dataPathCharP, "age") == 0)
			{
				*dataPathIntP = 3;
				*(dataPathIntP+1) = -1;
				return (0);
			}
			if (strncmp(dataPathCharP, "age.", strlen("age.")) == 0)
			{
				*dataPathIntP = 3;
				*(dataPathIntP+1) = -1;
				return (0);
			}
		}
		{ //Scanning companyId, terminal and non-terminal
			if (strcmp(dataPathCharP, "companyId") == 0)
			{
				*dataPathIntP = 4;
				*(dataPathIntP+1) = -1;
				return (0);
			}
			if (strncmp(dataPathCharP, "companyId.", strlen("companyId.")) == 0)
			{
				*dataPathIntP = 4;
				*(dataPathIntP+1) = -1;
				return (0);
			}
		}
		{ //Scanning expense, terminal and non-terminal
			if (strcmp(dataPathCharP, "expense") == 0)
			{
				*dataPathIntP = 5;
				*(dataPathIntP+1) = -1;
				return (0);
			}
			if (strncmp(dataPathCharP, "expense.", strlen("expense.")) == 0)
			{
				*dataPathIntP = 5;
				*(dataPathIntP+1) = -1;
				return (0);
			}
		}
		return -1;
	}

	static std::string getType(const int *dataPathIntP){
		switch(*dataPathIntP)
		{
			case -1:
				return ("User");
				break;
			case 0:
				return(system::UInt::getType(dataPathIntP+1));
				break;
			case 1:
				return(system::String::getType(dataPathIntP+1));
				break;
			case 2:
				return(test_compareFunctions::Address::getType(dataPathIntP+1));
				break;
			case 3:
				return(system::UInt8::getType(dataPathIntP+1));
				break;
			case 4:
				return(system::UInt::getType(dataPathIntP+1));
				break;
			case 5:
				return(system::UInt::getType(dataPathIntP+1));
				break;
			default:
				return ("_Unknown_");
				break;
		};
	}

	DataInstance * getInstance(const int *dataPathIntP){
		switch(*dataPathIntP)
		{
			case -1:
				return(this);
				break;
			case 0:
				return(id.getInstance(dataPathIntP+1));
				break;
			case 1:
				return(surname.getInstance(dataPathIntP+1));
				break;
			case 2:
				if (__filledOptFields__.value & 0x1)
					return(address.getInstance(dataPathIntP+1));
				else
					return (NULL);
				break;
			case 3:
				if (__filledOptFields__.value & 0x2)
					return(age.getInstance(dataPathIntP+1));
				else
					return (NULL);
				break;
			case 4:
				return(companyId.getInstance(dataPathIntP+1));
				break;
			case 5:
				if (__filledOptFields__.value & 0x4)
					return(expense.getInstance(dataPathIntP+1));
				else
					return (NULL);
				break;
			default:
				return (NULL);
				break;
		};
	}

	void copyFrom( User_base *other ){
		{ //Copying __filledOptFields__
			__filledOptFields__.copyFrom(&other->__filledOptFields__);
		}

		{ //Copying id
			id.copyFrom(&other->id);
		}

		{ //Copying surname
			surname.copyFrom(&other->surname);
		}

		if (__filledOptFields__.value & 0x1)
		{ //Copying address
			address.copyFrom(&other->address);
		}

		if (__filledOptFields__.value & 0x2)
		{ //Copying age
			age.copyFrom(&other->age);
		}

		{ //Copying companyId
			companyId.copyFrom(&other->companyId);
		}

		if (__filledOptFields__.value & 0x4)
		{ //Copying expense
			expense.copyFrom(&other->expense);
		}

	};

	std::string str(){
		std::ostringstream o;
				{ //Texting __filledOptFields__
			o << __filledOptFields__.str();
		}

		o<<" ";
				{ //Texting id
			o << id.str();
		}

		o<<" ";
				{ //Texting surname
			o << surname.str();
		}

		o<<" ";
		if (__filledOptFields__.value & 0x1)
				{ //Texting address
			o << address.str();
		}

		o<<" ";
		if (__filledOptFields__.value & 0x2)
				{ //Texting age
			o << age.str();
		}

		o<<" ";
				{ //Texting companyId
			o << companyId.str();
		}

		o<<" ";
		if (__filledOptFields__.value & 0x4)
				{ //Texting expense
			o << expense.str();
		}

		o<<" ";
		return o.str();
	}

	}; //class User_base

} // end of namespace samson
} // end of namespace test_compareFunctions

#endif

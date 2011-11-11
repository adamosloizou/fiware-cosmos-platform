
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_passive_location_mongo_location_export
#define _H_SAMSON_passive_location_mongo_location_export


#include <samson/module/samson.h>
#include <samson/modules/passive_location/Record.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/mobility/Record.h>

#include "mongo/client/dbclient.h"
#include "mongo/client/dbclientcursor.h"


using namespace mongo;

namespace samson
{
namespace passive_location
{


class mongo_location_export : public samson::Map
{
	std::string          mongo_ip;
	std::string          mongo_db;
	std::string          mongo_collection;

	int                  mongo_history;
	int                  mongo_bulksize;

	std::string          mongo_db_path;
	DBClientConnection*  mdbConnection;

	std::vector<mongo::BSONObj>  dataVec;
	int                          inserts;

	int                          insertsAcc;
	int                          records;
	int                          runs;

public:



/* ****************************************************************************
*
* init - 
*/
void init(samson::KVWriter* writer)
{
	std::string bulksize;
	std::string history;

	mdbConnection            = NULL;

	bulksize                 = environment->get("mongo.bulksize",           "30000");
	history                  = environment->get("mongo.history",            "1");

	mongo_ip                 = environment->get("mongo.ip",                 "no-mongo-ip");
	mongo_db                 = environment->get("mongo.db",                 "no-mongo-db");
	mongo_collection         = environment->get("mongo.collection",         "no-mongo-collection");

	
	mongo_bulksize           = atoi(bulksize.c_str());
	mongo_history            = atoi(history.c_str());

	if (mongo_ip == "no-mongo-ip")
	{
		tracer->setUserError("No mongo ip specified. Please specify mongo ip with 'mongo.ip' environment variable");		
		return;
	}

	if (mongo_db == "no-mongo-db")
	{
		tracer->setUserError("No db specified. Please specify mongo database db name with 'mongo.db' environment variable");
		return;
	}

	if (mongo_collection == "no-mongo-collection")
	{
		tracer->setUserError("No collection specified. Please specify mongo database collection name with 'mongo.collection' environment variable");
		return;
	}

	mdbConnection = new DBClientConnection();
	OLM_M(("Connecting to mongo at '%s'", mongo_ip.c_str()));

	try
	{
		mdbConnection->connect(mongo_ip);
	}
	catch(mongo::ConnectException &e)
	{
		OLM_E(("Error connecting to mongo at '%s'", mongo_ip.c_str()));
		delete mdbConnection;
		mdbConnection = NULL;
		tracer->setUserError("error connecting to MongDB at" + mongo_ip);
		return;
	}

	mongo_db_path          = mongo_db         + "." + mongo_collection;

	inserts    = 0;

	insertsAcc = 0;
	records    = 0;
	runs       = 0;
}



/* ****************************************************************************
*
* run - 
*/
void run(samson::KVSetStruct* inputs, samson::KVWriter* writer)
{
	if (inputs[0].num_kvs == 0)
	{
		tracer->setUserError("Zero input key values ...");
		return;
	}
	
	if (mdbConnection == NULL)
	{
		tracer->setUserError("Not connected to MongoDB - please check the parameters to this operation");
		return;
	}

	++runs;

	records += inputs[0].num_kvs;

	for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
	{
		samson::system::UInt      key;
		samson::mobility::Record  value;
		auto_ptr<DBClientCursor>  result;

		key.parse(inputs[0].kvs[i]->key);
		value.parse(inputs[0].kvs[i]->value);

		if (mongo_history == 1)
		{
			mongo::BSONObj  record = BSON("I" << (long long int) value.userId.value      <<
										  "T" << (long long int) value.timestamp.value   <<
										  "C" << (long long int) value.cellId.value      <<
										  "X" << (float) value.position.latitude.value   <<
										  "Y" << (float) value.position.longitude.value);

			dataVec.push_back(record);

			++inserts;
			++insertsAcc;

			if ((inserts % mongo_bulksize) == 0)
			{
				// OLM_M(("Inserting bulk of %d records (bulksize: %d)", inserts, mongo_bulksize));
				mdbConnection->insert(mongo_db_path, dataVec);
				dataVec.clear();
				inserts = 0;
			}
		}
		else
		{
			mongo::BSONObj  query;
			mongo::BSONObj  record = BSON("_id" << (long long int) value.userId.value       <<
										  "T"   << (long long int) value.timestamp.value    <<
										  "C"   << (long long int) value.cellId.value       <<
										  "X"   << (float) value.position.latitude.value    <<
										  "Y"   << (float) value.position.longitude.value);

			// db.LastKnownLocation.update( { _id:3, T : { $lt: 3 }  }, { _id:3, T:3, C:1, X:1, Y:1 }, true  )
			query  = BSON("_id" << (long long int) value.userId.value << "T" << BSON("$lt" << (long long int) value.timestamp.value));

			mdbConnection->update(mongo_db_path, query, record, true);
		}
	}

	if (mongo_history == 1)
	{
		if ((inserts % mongo_bulksize) == 0)
		{
			mdbConnection->insert(mongo_db_path, dataVec);
			dataVec.clear();
			inserts = 0;
		}
	}

	// mdbConnection->ensureIndex(mongo_db_path, fromjson("{I:1}"));
}



/* ****************************************************************************
*
* finish - 
*/
void finish(samson::KVWriter* writer)
{
	if ((mongo_history == 1) && (inserts != 0))
	{
		mdbConnection->insert(mongo_db_path, dataVec);
		dataVec.clear();
	}

	OLM_M(("%d inserts, %d records inserted, %d calls to run", insertsAcc, records, runs));
	delete mdbConnection;
}

};


} // end of namespace passive_location
} // end of namespace samson

#endif

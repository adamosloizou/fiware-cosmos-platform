
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_mongo_bulk_load
#define _H_SAMSON_txt_mongo_bulk_load


#include <samson/module/samson.h>
#include "mongo/client/dbclient.h"

using namespace mongo;


namespace samson
{

namespace txt_md
{


class mongo_bulk_load : public samson::Map
{
public:
	std::string            mongo_ip;
	std::string            mongo_db;
	std::string            mongo_collection;
	std::string            mongo_db_path;
	DBClientConnection*    mdbConnection;



void init(samson::KVWriter* writer)
{
	mongo_ip           = environment->get("mongo.ip",    "no-mongo-ip");
	mongo_db           = environment->get("mongo.db",    "no-mongo-db");
	mongo_collection   = environment->get("mongo.collection", "no-mongo-collection");

	mongo_db_path = mongo_db + "." + mongo_collection;

	mdbConnection = new DBClientConnection();
	mdbConnection->connect(mongo_ip);
}



void run(samson::KVSetStruct* inputs, samson::KVWriter* writer)
{
	if (inputs[0].num_kvs == 0)
		return;

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

	std::vector<mongo::BSONObj> bulk_data;
	for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
	{
		samson::system::UInt      key;
		samson::txt_md::BulkData  value;

        key.parse(inputs[0].kvs[i]->key);
        value.parse(inputs[0].kvs[i]->value);

		mongo::BSONObj record = BSON("I" << (long long int) key.value << "P" << (long long int) value.position.value << "T" << (long long int) value.timestamp.value);
		bulk_data.push_back(record);
	}

	mdbConnection->insert(mongo_db_path, bulk_data);
	bulk_data.clear();
}



void finish(samson::KVWriter* writer)
{
	delete mdbConnection;
}



};


} // end of namespace txt
} // end of namespace samson

#endif

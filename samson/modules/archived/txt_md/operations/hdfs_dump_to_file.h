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
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_hdfs_dump_to_file
#define _H_SAMSON_txt_hdfs_dump_to_file


#include <samson/module/samson.h>
#include "hdfs.h"


namespace samson
{

namespace txt_md
{


class hdfs_dump_to_file : public samson::Map
{
	std::string  hdfs_ip;
	std::string  hdfs_path;
	hdfsFS       hdfs_fs;
	hdfsFile     hdfs_fd;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.String system.UInt  

helpLine: Dump KV data to a HDFS file in server 'hdfs.ip' and with path 'hdfs.path'
#endif // de INFO_COMMENT



/* ****************************************************************************
*
* init - 
*/
void init(samson::KVWriter* writer)
{
	hdfs_ip     = environment->get("hdfs.ip",    "default");
	hdfs_path   = environment->get("hdfs.path",  "no-hdfs-path");

	if (hdfs_path == "no-hdfs-path")
	{
		tracer->setUserError("No path specified. Please specify HDFS path with 'hdfs.path' environment variable");
		return;
	}

	hdfs_fs     = hdfsConnect(hdfs_ip.c_str(), 0);
	hdfs_fd     = hdfsOpenFile(hdfs_fs, hdfs_path.c_str(), O_WRONLY|O_CREAT, 0, 0, 0);
}



/* ****************************************************************************
*
* run - 
*/
void run(samson::KVSetStruct* inputs, samson::KVWriter* writer)
{
	if (inputs[0].num_kvs == 0)
		return;

	if (hdfs_path == "no-hdfs-path")
	{
		tracer->setUserError("No path specified. Please specify HDFS path with 'hdfs.path' environment variable");
		return;
	}

	for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
	{
		std::string             line;
		samson::system::String  key;
		samson::system::UInt    value;
		char                    cval[64];

		key.parse(inputs[0].kvs[i]->key);
		value.parse(inputs[0].kvs[i]->value);

		sprintf(cval, "%lu", value.value);
		line = std::string("K:") + key.value.c_str() + std::string("::V:") + cval + std::string("\n");
		hdfsWrite(hdfs_fs, hdfs_fd, (void*) line.c_str(), strlen(line.c_str()));
	}

	hdfsFlush(hdfs_fs, hdfs_fd);
}



/* ****************************************************************************
*
* finish - 
*/
void finish(samson::KVWriter* writer)
{
	hdfsCloseFile(hdfs_fs, hdfs_fd);
}

};


} // end of namespace txt_md
} // end of namespace samson

#endif

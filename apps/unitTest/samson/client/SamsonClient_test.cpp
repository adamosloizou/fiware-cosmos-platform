
#include "gtest/gtest.h"

#include "au/ThreadManager.h"

#include "engine/MemoryManager.h"
#include "engine/MemoryRequest.h"

#include "xmlparser/xmlParser.h"

#include "samson/client/SamsonClient.h"

TEST(samson_client, test_1 )
{
   size_t total_memory = 64*1024*1024; // Use 64Mb for this test
   samson::SamsonClient::general_init( total_memory );

   // Create client connection
   samson::SamsonClient* samson_client = new samson::SamsonClient("SamsonClientTest");

   // Connect to samsonWorker
   au::ErrorManager error;   
   samson_client->initConnection( &error , "localhost" , 1324 , "anonymous" , "anonymous" );

   EXPECT_EQ( error.isActivated() , false) << "Error connecting samsonClient to samsonWorker";

   samson_client->waitUntilFinish();

   // Disconnect from worker
   samson_client->disconnect();

   // Delete client
   delete samson_client;

   // Close everything initiatied in general_init
   samson::SamsonClient::general_close( );

}    

TEST(samson_client, test_2 )
{
   size_t total_memory = 64*1024*1024; // Use 64Mb for this test
   samson::SamsonClient::general_init( total_memory );

   // Create client connection
   samson::SamsonClient* samson_client = new samson::SamsonClient("SamsonClientTest");

   // Connect to samsonWorker
   au::ErrorManager error;
   samson_client->initConnection( &error , "localhost" , 1324 , "anonymous" , "anonymous" );

   ASSERT_TRUE(samson_client->connection_ready()) << "Connection not ready";

   // Disconnect from worker
   samson_client->disconnect();

   // Delete client
   delete samson_client;

   // Close everything initiatied in general_init
   samson::SamsonClient::general_close( );
}



#ifndef _H_SAMSONPUSHLOGS_CONNECTIONS_MANAGER
#define _H_SAMSONPUSHLOGS_CONNECTIONS_MANAGER

#include "au/mutex/TokenTaker.h"

#include "samson/client/SamsonClient.h"
#include "samson/client/SamsonPushBuffer.h"

#include "LogsDataSet.h"

// Function to run the background thread
void* run_SamsonPushLogsConnection( void* p);

class SamsonPushLogsConnection
{

    samson::SamsonPushBuffer *pushBuffer_;
    LogsDataSet *dataset_;
    char *queue_name_;
    float ntimes_real_time_;
    char *buffer_;
    bool thread_running_;

public:

    SamsonPushLogsConnection( LogsDataSet *dataset, const char *queue_name, float ntimes_real_time, samson::SamsonClient *samson_client );
    ~SamsonPushLogsConnection();

    void Run();

    bool IsFinished()
    {
        return !thread_running_;
    }

    const char *GetQueueName() {return queue_name_;}
};

class SamsonPushLogsConnectionsManager
{
    au::Token token_;
    std::set<SamsonPushLogsConnection*> connections_;

public:

    SamsonPushLogsConnectionsManager() : token_("SamsonPushConnectionsManager")
    {

    }

    virtual void NewLogDataset(SamsonPushLogsConnection * log_connection )
    {
        // Mutex protection
        au::TokenTaker tt(&token_);

        //SamsonPushLogsConnection * connection = new SamsonPushLogsConnection( log_connection );
        connections_.insert(log_connection);
    }

    void ReviewDatasets()
    {
        // Mutex protection
        au::TokenTaker tt(&token_);

        std::set<SamsonPushLogsConnection*>::iterator it_connections;
        for( it_connections = connections_.begin() ; it_connections != connections_.end() ; ++it_connections )
        {
            SamsonPushLogsConnection* connection = *it_connections;
            if ( connection->IsFinished() )
            {
                LM_M(("Dataset:%s finished", connection->GetQueueName()));
                connections_.erase( it_connections );
            }
        }
    }

    size_t GetNumDataSets()
    {
        // Mutex protection
        au::TokenTaker tt(&token_);

        return connections_.size();
    }

};

#endif

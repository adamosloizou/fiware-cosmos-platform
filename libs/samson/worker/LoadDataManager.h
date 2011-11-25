#ifndef _H_LOAD_DATA_MANAGER
#define _H_LOAD_DATA_MANAGER

#include <string>

#include "samson/common/samson.pb.h"					// samson::network:...
#include "au/map.h"						// au::map
#include "au/Lock.h"						// au::Lock
#include "samson/common/samsonDirectories.h"			// SAMSON_DATA_DIRECTORY
#include "samson/common/SamsonSetup.h"				// samson::SamsonSetup

namespace samson {

	class Buffer;
	class SamsonWorker;
	class LoadDataManager;
    class DiskOperation;
	
	class LoadDataManagerItem
	{
		
	public:

		size_t id;								// Identifier inside LoadDataManager

        size_t load_id;                         // Identifier at the controller ( only in upload at the moment )
        
		int fromIdentifier;						// Dalilah identifier to send responses		
		engine::Buffer *buffer;					// Buffer allocated for this task		
		LoadDataManager *dataManager;			// Pointer to the data manager		
		
		LoadDataManagerItem( size_t _id, size_t _load_id, int _fromIdentifier , LoadDataManager *dataManager );		
		virtual ~LoadDataManagerItem(){}
		virtual std::string getStatus( )=0;

	};
	
	
	class UploadItem : public LoadDataManagerItem 
	{

	public:
		
		network::UploadDataFile *upload_data_file;	// Information about the file-request to upload
		size_t sender_id;							// id of the sender
		
		std::string fileName;						// Filename of the new upload
		size_t size;								// Size of the uploaded file
				
		UploadItem( size_t _id, int _fromIdentifier , LoadDataManager *dataManager, const network::UploadDataFile &_uploadDataFile , size_t sender_id,  engine::Buffer * buffer );
		~UploadItem();
		
		void submitToFileManager();
		void sendResponse( bool error , std::string error_message );
		
		// Function to get the run-time status of this object
		std::string getStatus( );
		
	};	
	
	/**
	 Item requested to be downloaded
	 */
	
	class DownloadItem : public LoadDataManagerItem 
	{
		
	public:
		
		
		network::DownloadDataFile *download_data_file;	// Information about the request
		size_t sender_id;								// id of the sender
		
		
		DownloadItem(size_t _id, int _fromIdentifier, LoadDataManager *dataManager, const network::DownloadDataFile &_download_data_file, size_t sender_id);		
		~DownloadItem();

		void submitToFileManager();
		void sendResponse( bool error , std::string error_message );

		std::string getStatus( )
		{
			std::ostringstream output;
			output << "<Down:?>";
			return output.str();
		}
	};
	
	
	class LoadDataManager : public engine::Object
	{
		friend class UploadItem;
		friend class DownloadItem;
		
		
		SamsonWorker *worker;

		au::Lock lock;
		
		au::map<size_t,UploadItem> uploadItem;		// Items to be uploded from dalilahs
		au::map<size_t,DownloadItem> downloadItem;	// Items to be downloaded to dalilahs

		size_t id;	// Auto-incrising counter to identify elements
		
	public:

		size_t upload_size;			// Simple information about the size of upload
		
		LoadDataManager( SamsonWorker *_worker );
        ~LoadDataManager();
		
		// Add item to upload data
		void addUploadItem( int fromIdentifier, const network::UploadDataFile &uploadDataFile ,size_t sender_id, engine::Buffer * buffer  );
		
		// add item to download data
		void addDownloadItem( int fromIdentifier, const network::DownloadDataFile &downloadDataFile, size_t sender_id );
		
		// Notifications
        void notify( engine::Notification* notification );
        void setNotificationCommonEnvironment( engine::Notification* notification );

		
	};

}

#endif
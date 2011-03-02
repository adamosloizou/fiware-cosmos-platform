
#ifndef _H_FILE_MANAGER_ITEM
#define _H_FILE_MANAGER_ITEM


#include "Lock.h"		// au::Locod
#include "Buffer.h"		// ss::Buffer
#include "au_map.h"		// au::map
#include <list>			// std::list
#include <set>			// std::set
#include <vector>		// std::vector
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate
#include "DiskStatistics.h"			// ss::DiskStatistics
#include "Status.h"					// au::Status

namespace ss {
	
	/**
	 Generic read/write item that contains a pointer to the delegate
	 */
	
	class FileManagerItem 
	{
	protected:
		
		FileManagerDelegate *delegate;		// Delegate that should be notified
		
		size_t fm_id;						// File Manager id
		
		friend class FileManager;
		
		//For statistics
		
		DiskStatistics::Operation operation;
		
		size_t size;
		
		timeval begin;
		timeval end;

	public:
		
		int component;
		size_t tag;
		size_t sub_tag;
		
	public:
		
		bool error;
		std::string error_message;
		
	public:
		
		typedef enum
		{
			write,
			read,
			remove
		} Type;
		
		Type type;
		
		FileManagerItem( FileManagerDelegate* _delegate ,  Type _type , size_t _size );
		
		size_t getId();
		
		void addStatistics(  DiskStatistics * statistics );
		
		
		virtual void notifyToDelegate()=0;
				
		virtual void freeResources()=0;
		
		void setError( std::string _error_message );
		
	private:
		void setId(size_t _id);
		
		
	};	
	
	
	/**
	 Request of a file to be removed
	 */
	
	class FileManagerRemoveItem : public FileManagerItem
	{
		
	public:
		
		std::string fileName;				// Name of the file
		
		FileManagerRemoveItem( std::string _fileName , FileManagerDelegate* delegate ) : FileManagerItem( delegate ,  FileManagerItem::remove  , 0 )
		{
			fileName = _fileName;
		}
		
		void notifyToDelegate()
		{
			if( delegate )
				delegate->notifyFinishRemoveItem(this);
			else
				delete this;	// Auto-remove to myself since no-one will handle me
		}
		
		void freeResources()
		{
			// nothing to do here
		}

		
	};
	
}


#endif

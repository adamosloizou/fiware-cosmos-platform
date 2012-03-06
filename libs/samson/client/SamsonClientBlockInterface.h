#ifndef _H_SamsonClientBlockInterface_SamsonClient
#define _H_SamsonClientBlockInterface_SamsonClient

namespace  samson 
{
    
    // Class to work with blocks
    
    class SamsonClientBlockInterface
    {
        
    public:
        
        virtual ~SamsonClientBlockInterface(){}
        
        // Get header information about the block
        virtual KVHeader getHeader()=0;
        
        // Get total size of the block
        virtual size_t getBufferSize()=0;
        virtual size_t getTXTBufferSize()=0;

        // Get a pointer to the txt block if this is the case
        virtual char* getTXTBufferPointer()=0;
        
        // Get content
        virtual std::string get_header_content()=0;
        virtual std::string get_content(int max_kvs, const char *format="plain")=0;
        
    };
}

#endif
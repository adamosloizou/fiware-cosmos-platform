#include "au/string.h"

#include "CommitCommand.h" // Own interface

namespace samson
{
  CommitCommandItem::CommitCommandItem( const std::string& command
                                       , const std::string& queue
                                       , size_t block_id
                                       , const KVFormat& format
                                       , const KVRange& range
                                       , const KVInfo& info )
  : command_(command)
  , queue_(queue)
  , block_id_(block_id)
  , format_(format)
  , range_(range)
  , info_( info )
  {
  }
  
  
  CommitCommandItem* CommitCommandItem::create_item( const std::string& command , au::ErrorManager* error )
  {
    std::vector<std::string> components = au::split( command , ':' );
    if( components.size() != 9 )
    {
      error->set(au::str("Wrong number of components (%lu!=9) in commit command component: '%s'"
                         , components.size()
                         , command.c_str() ));
      return NULL;
    }
    
    std::string sub_command = components[0];
    
    if( ( sub_command != "add" ) && ( sub_command != "rm" ) )
    {
      error->set(au::str("Wrong command (%s) in commit command component: '%s'"
                         , sub_command.c_str()
                         , command.c_str() ));
      return NULL;
    }
    
    size_t block_id = atoll( components[2].c_str() );
    if( block_id == 0 )
    {
      error->set(au::str("Wrong block_id (%lu) in commit command component: '%s'"
                         , block_id
                         , command.c_str() ));
      return NULL;
    }
    
    KVFormat format( components[3] , components[4] );
    
    KVRange range( atoi( components[5].c_str() ) , atoi( components[6].c_str() ) );
    if( !range.isValid() )
    {
      error->set(au::str("Wrong range (%s) in commit command component: '%s'"
                         , range.str().c_str()
                         , command.c_str() ));
      return NULL;
    }
    
    KVInfo info( atoll( components[7].c_str() ) , atoll( components[8].c_str() ) );
    
    return new CommitCommandItem( sub_command , components[1] , block_id , format , range , info );
    
  }
  
  
  std::string CommitCommandItem::command() const
  {
    return command_;
  }
  
  std::string CommitCommandItem::queue() const
  {
    return queue_;
  }
  
  size_t CommitCommandItem::block_id() const
  {
    return block_id_;
  }
  
  KVFormat CommitCommandItem::format() const
  {
    return format_;
  }
  
  KVRange CommitCommandItem::range() const
  {
    return range_;
  };
  
  KVInfo CommitCommandItem::info() const
  {
    return  info_;
  }
  
  std::string CommitCommandItem::str() const
  {
    std::ostringstream output;
    output << command_        << ":";
    output << queue_          << ":";
    output << block_id_       << ":";
    output << format_.keyFormat     << ":";
    output << format_.valueFormat   << ":";
    output << range_.hg_begin << ":";
    output << range_.hg_end   << ":";
    output << info_.size      << ":";
    output << info_.kvs       << ":";
    return  output.str();
  }
  
  
  CommitCommand::CommitCommand()
  {
  }
  
  CommitCommand::~CommitCommand()
  {
    items_.clearVector();
  }
  
  
  
  void CommitCommand::ParseCommitCommand( const std::string& command , au::ErrorManager * error )
  {
    au::CommandLine cmdLine( command );
    int num_arguments = cmdLine.get_num_arguments();
    
    if( num_arguments == 0 )
    {
      error->set("No command provided");
      return;
    }
    
    if( cmdLine.get_argument(0) != "block" )
    {
      error->set( au::str("Command %s not valid. Only block accepted" , cmdLine.get_argument(0).c_str() ));
      return;
    }
    
    if( num_arguments == 1 )
    {
      error->set("No command provided after code-word block");
      return;
    }
    
    
    for ( int i = 1 ; i < num_arguments ; i++ )
    {
      au::ErrorManager tmp_error;
      CommitCommandItem* item =  CommitCommandItem::create_item( cmdLine.get_argument(i) , &tmp_error );
      if( tmp_error.IsActivated() )
      {
        error->set(au::str(" Error parsing sub command %s: %s"
                           , cmdLine.get_argument(i).c_str()
                           , tmp_error.GetMessage().c_str() ));
        return;
      }
      
      // Insert in the list of items
      items_.push_back(item);
      
    }
    
  }
  
  void CommitCommand::AddBlock( const std::string& queue
                               , size_t block_id
                               , const KVFormat& format
                               , const KVRange& range
                               , const KVInfo& info)
  {
    items_.push_back( new CommitCommandItem("add" , queue , block_id , format , range, info ) );
  }
  
  void CommitCommand::RemoveBlock( const std::string& queue
                                  , size_t block_id
                                  , const KVFormat& format
                                  , const KVRange& range
                                  , const KVInfo& info)
  {
    items_.push_back( new CommitCommandItem("rm" , queue , block_id , format , range, info ));
  }
  
  std::string CommitCommand::GetCommitCommand() const
  {
    std::ostringstream output;
    output << "block "; // Main command of the commit
    for ( size_t i = 0 ; i < items_.size() ; i++ )
      output << items_[i]->str() << " ";
    
    return output.str();
  }
  
  
  const au::vector<CommitCommandItem>& CommitCommand::items()
  {
    return items_;
  }
  
}

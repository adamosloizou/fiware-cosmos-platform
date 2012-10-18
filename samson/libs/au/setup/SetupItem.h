


#ifndef _H_AU_SETUP_ITEM
#define _H_AU_SETUP_ITEM

#include <string>

#include "au/containers/map.h"

namespace au {
  
  typedef enum {
    SetupItem_uint64,
    SetupItem_string
  } SamsonAdaptorType;
  
  class SetupItem {
    
  public:
    
    SetupItem( const std::string& name
              , const std::string& default_value
              , const std::string& description
              , SamsonAdaptorType type );
    
    bool set_value(const std::string& value);
    
    std::string name() const;
    std::string value() const;
    std::string default_value() const;
    std::string description() const;
    std::string category() const;
    
    bool CheckValidValue(const std::string& _value) const;
    
    void ResetToDefaultValue();
    
  private:
    
    std::string name_;
    std::string default_value_;
    std::string value_;
    std::string description_;
    SamsonAdaptorType type_;
    
  };
  
}

#endif
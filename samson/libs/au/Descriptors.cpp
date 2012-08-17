
#include "Descriptors.h"        // Own interface

namespace au {
  
  DescriptorsCounter::DescriptorsCounter( const std::string& description)
  : description_( description )
  {
    counter_ = 0;
  }
  
  void DescriptorsCounter::Increase()
  {
    counter_++;
  }
  
  std::ostream& operator<<( std::ostream& o , const DescriptorsCounter& descriptor_counter )
  {
    o << descriptor_counter.counter_ << " X " << descriptor_counter.description_; 
    return o;
  }
  
  Descriptors::Descriptors()
  {
    
  }
  
  Descriptors::~Descriptors()
  {
    // Delete is called for each conunter
    concepts_.clearMap();
  }
  
  size_t Descriptors::size()
  {
    return concepts_.size();
  }
  
  void Descriptors::Add( const std::string& txt )
  {
    DescriptorsCounter* counter = concepts_.findInMap( txt );
    
    if( !counter )
    {
      counter = new DescriptorsCounter( txt );
      concepts_.insertInMap( txt, counter );
    }
    
    counter->Increase();
  }
  
  std::string Descriptors::str()
  {
    
    std::ostringstream output;
    return output.str();
  }
 
  std::ostream& operator<<( std::ostream& o , const Descriptors& descriptors )
  {    
    au::map< std::string , DescriptorsCounter>::const_iterator it;
    for ( it = descriptors.concepts_.begin() ; it != descriptors.concepts_.end() ; )
    {
      o << it->second; 
      it++;
      if( it != descriptors.concepts_.end() )
        o << " ";
    }
    return o;
  }

  
}

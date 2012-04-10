
#include "Uint64Vector.h"
namespace au
{
    Uint64Vector::Uint64Vector()
    {
    }
    
    Uint64Vector::Uint64Vector(  std::vector<size_t> v )
    {
        copyFrom(v);
    }
    
    size_t Uint64Vector::getNumberOfTimes( size_t t )
    {
        size_t num = 0;
        for ( size_t i = 0 ; i < size() ; i++)
            if( (*this)[i] == t )
                num++;
        return num;
    }
    
    std::set<size_t> Uint64Vector::getSet()
    {
        std::set<size_t> set;
        for ( size_t i = 0 ; i < size() ; i++)
            set.insert( (*this)[i] );
        return set;
    }
    
    void Uint64Vector::copyFrom(  std::vector<size_t> &v)
    {
        std::set<size_t> set;
        for ( size_t i = 0 ; i < v.size() ; i++)
            push_back( v[i] );
    }   
}
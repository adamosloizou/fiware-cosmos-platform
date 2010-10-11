#ifdef _H_VAR_INT
#define _H_VAR_INT

/**
    Variable length integer definition
*/

inline int staticVarIntParse( char *data , size_t* value)
{
  unsigned char *p = (unsigned char *)data;
  size_t base = 1;
  
  int offset = 0;
  size_t tmp_value = 0;
  
  do
    {
      tmp_value += (size_t)(p[offset]&127)*base; 
      base = base*128;
    }
  while( p[offset++] & 128 );
  
  *value = tmp_value;
  return offset;
}

inline int staticVarIntSerialize( char *data , size_t value)
{
  unsigned char *p = (unsigned char *)data;
  
  size_t __value = value;
  
  //Special case
  if (__value==0)
    {
      p[0] = 0;
      return 1;
    }
  
  int offset = 0;
  unsigned char tmp;
  
  while( __value > 0 )
    {
      //cerr << "Parsign value "<< __value << "\n";
      
      tmp  = __value - (__value/128)*128;
      __value = __value/128;
      
      if( __value >0)
	tmp |= 128;//Add flag
      
      //Push to the buffer
      p[offset++]=tmp;
    }
  return offset;
}


#endif

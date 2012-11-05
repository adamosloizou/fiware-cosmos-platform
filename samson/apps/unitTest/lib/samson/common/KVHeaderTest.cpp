/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */


#include "samson/common/KVHeader.h"
#include "samson/common/coding.h"

#include "gtest/gtest.h"

// Test  KVInfo;
TEST(samson_common_KVHeader, basic) {
  
  samson::KVHeader header;
  header.Init( samson::KVFormat("a" , "b") , samson::KVInfo( 100 , 10 ) );
  
  EXPECT_EQ( "a"  , header.GetKVFormat().keyFormat );
  EXPECT_EQ( "b"  , header.GetKVFormat().valueFormat );
  EXPECT_EQ( 100, header.info.size );
  EXPECT_EQ( 10 , header.info.kvs);
  EXPECT_FALSE( header.IsTxt() );
  EXPECT_FALSE( header.IsModule() );
  EXPECT_TRUE( header.Check() );
  

  samson::KVHeader header_txt;
  header_txt.InitForTxt(1000);
  EXPECT_STREQ( samson::KVFormat::KVFormatForTxt().str().c_str() , header_txt.GetKVFormat().str().c_str() );
  EXPECT_EQ( 0 , header_txt.info.kvs );
  EXPECT_EQ( 1000 , header_txt.info.size );
  EXPECT_TRUE( header_txt.IsTxt() );
  EXPECT_FALSE( header_txt.IsModule() );
  EXPECT_TRUE( header_txt.Check() );

  samson::KVHeader header_module;
  header_module.InitForModule(2000);
  EXPECT_STREQ( samson::KVFormat::KVFormatForModule().str().c_str() , header_module.GetKVFormat().str().c_str() );
  EXPECT_EQ( 2000 , header_module.info.size );
  EXPECT_EQ( 0 , header_module.info.kvs );
  EXPECT_FALSE( header_module.IsTxt() );
  EXPECT_TRUE( header_module.IsModule() );
  EXPECT_TRUE( header_module.Check() );

}

TEST(samson_common_KVHeader, check_total_size ) {
  
  samson::KVHeader header;
  header.Init( samson::KVFormat("a" , "b") , samson::KVInfo( 100 , 10 ) );
  size_t total_size = sizeof(samson::KVHeader) + 100;
  EXPECT_TRUE( header.CheckTotalSize( total_size ) );
  
}

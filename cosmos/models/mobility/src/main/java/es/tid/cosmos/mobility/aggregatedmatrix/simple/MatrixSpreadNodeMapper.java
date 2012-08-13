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

package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, ItinTime>
 * Output: <Long, ItinTime>
 *
 * @author dmicol
 */
class MatrixSpreadNodeMapper extends Mapper<ProtobufWritable<TwoInt>,
        TypedProtobufWritable<ItinTime>, LongWritable, TypedProtobufWritable<ItinTime>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            TypedProtobufWritable<ItinTime> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodbts = key.get();
        context.write(new LongWritable(nodbts.getNum1()), value);
    }
}
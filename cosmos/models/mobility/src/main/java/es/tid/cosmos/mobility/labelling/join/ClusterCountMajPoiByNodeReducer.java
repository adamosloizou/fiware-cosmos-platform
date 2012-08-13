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

package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;


/**
 * Input: <TwoInt, Null>
 * Output: <Long, TwoInt>
 *
 * @author dmicol
 */
class ClusterCountMajPoiByNodeReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>, LongWritable,
        TypedProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Null>> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (TypedProtobufWritable<Null> value : values) {
            valueCount++;
        }
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        context.write(new LongWritable(twoInt.getNum1()),
                      new TypedProtobufWritable<TwoInt>(
                              TwoIntUtil.create(twoInt.getNum2(), valueCount)));
    }
}

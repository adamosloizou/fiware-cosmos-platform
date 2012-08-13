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

package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, TwoInt>
 *
 * @author dmicol
 */
class AdjPutMaxIdReducer extends Reducer<LongWritable,
        TypedProtobufWritable<TwoInt>, LongWritable, TypedProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        TwoInt pairPois = null;
        long max = Long.MIN_VALUE;
        for (TypedProtobufWritable<TwoInt> value : values) {
            pairPois = value.get();
            if (pairPois.getNum2() > max) {
                max = pairPois.getNum2();
            }
        }
        context.write(key, new TypedProtobufWritable<TwoInt>(
                TwoIntUtil.create(pairPois.getNum1(), max)));
    }
}
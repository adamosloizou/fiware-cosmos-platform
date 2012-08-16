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
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, TwoInt>
 * Output: <Long, TwoInt>
 *
 * @author dmicol
 */
class AdjJoinPairbtsAdjbtsReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<InputIdRecord>,
        LongWritable, TypedProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<InputIdRecord>> values, Context context)
            throws IOException, InterruptedException {
        List<TwoInt> pairPoisList = new LinkedList<TwoInt>();
        boolean hasAdjacentBts = false;
        for (TypedProtobufWritable<InputIdRecord> value : values) {
            final InputIdRecord record = value.get();
            switch (record.getInputId()) {
                case 0:
                    final TwoInt twoInt = TwoInt.parseFrom(
                            record.getMessageBytes());
                    pairPoisList.add(twoInt);
                    break;
                case 1:
                    hasAdjacentBts = true;
                    break;
                default:
                    throw new IllegalStateException("Unexpected Input ID: "
                            + record.getInputId());
            }
        }

        if (hasAdjacentBts) {
            for (TwoInt pairPois : pairPoisList) {
                context.write(new LongWritable(pairPois.getNum1()),
                              new TypedProtobufWritable<TwoInt>(pairPois));
            }
        }
    }
}

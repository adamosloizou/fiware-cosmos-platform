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

package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, ItinTime|Poi>
 * Output: <Long, ItinTime>
 *
 * @author dmicol
 */
class ItinFilterPoisReducer extends Reducer<ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Message>, LongWritable, TypedProtobufWritable<ItinTime>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedList = TypedProtobufWritable.groupByClass(
                values, ItinTime.class, Poi.class);
        List<ItinTime> itinTimeList = dividedList.get(ItinTime.class);
        List<Poi> poiList = dividedList.get(Poi.class);
        key.setConverter(TwoInt.class);
        final TwoInt nodeBts = key.get();
        for (Poi poi : poiList) {
            for (ItinTime itinTime : itinTimeList) {
                ItinTime.Builder itinTimeOut = ItinTime.newBuilder(itinTime);
                itinTimeOut.setBts(poi.getId());
                context.write(new LongWritable(nodeBts.getNum1()),
                              new TypedProtobufWritable<ItinTime>(itinTimeOut.build()));
            }
        }
    }
}

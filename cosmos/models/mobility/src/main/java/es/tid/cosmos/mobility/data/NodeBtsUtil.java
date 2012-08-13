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

package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 *
 * @author sortega
 */
public final class NodeBtsUtil {

    private NodeBtsUtil() {}

    public static NodeBts create(long userId, long bts,
                                 int weekday, int range) {
        return NodeBts.newBuilder()
                .setUserId(userId)
                .setBts(bts)
                .setWeekday(weekday)
                .setRange(range)
                .build();
    }

    public static ProtobufWritable<NodeBts> wrap(NodeBts obj) {
        ProtobufWritable<NodeBts> wrapper =
                ProtobufWritable.newInstance(NodeBts.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<NodeBts> createAndWrap(long userId,
            long bts, int weekday, int range) {
        return wrap(create(userId, bts, weekday, range));
    }

    public static String toString(NodeBts obj, String separator) {
        return obj.getUserId() + separator + obj.getBts() + separator
                + obj.getWeekday() + separator + obj.getRange();
    }
}


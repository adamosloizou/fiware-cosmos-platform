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

import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;

/**
 *
 * @author dmicol
 */
public final class BtsProfileUtil {

    private BtsProfileUtil() {}

    public static BtsProfile create(long bts, int profile, int wday,
                                    int hour) {
        return BtsProfile.newBuilder()
                .setBts(bts)
                .setProfile(profile)
                .setWday(wday)
                .setHour(hour)
                .build();
    }

    public static ProtobufWritable<BtsProfile> wrap(BtsProfile obj) {
        ProtobufWritable<BtsProfile> wrapper =
                ProtobufWritable.newInstance(BtsProfile.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<BtsProfile> createAndWrap(long bts,
            int profile, int wday, int hour) {
        return wrap(create(bts, profile, wday, hour));
    }
}

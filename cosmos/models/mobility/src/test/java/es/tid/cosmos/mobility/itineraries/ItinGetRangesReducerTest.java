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
import java.util.Arrays;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.mobility.data.DateUtil;
import es.tid.cosmos.mobility.data.ItinMovementUtil;
import es.tid.cosmos.mobility.data.ItinTimeUtil;
import es.tid.cosmos.mobility.data.TimeUtil;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 *
 * @author dmicol
 */
public class ItinGetRangesReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<ItinMovement>,
            ProtobufWritable<ItinRange>, TypedProtobufWritable<Float64>>
            instance;

    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable,
                TypedProtobufWritable<ItinMovement>, ProtobufWritable<ItinRange>,
                TypedProtobufWritable<Float64>>(new ItinGetRangesReducer());
    }

    @Test
    public void testReduce() throws IOException {
        final Date srcDate = DateUtil.create(2012, 05, 27, 0);
        final Date tgtDate = DateUtil.create(2012, 05, 28, 1);
        final Time srcTime = TimeUtil.create(18, 01, 05);
        final Time tgtTime = TimeUtil.create(15, 35, 27);
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<ItinMovement> value =
                new TypedProtobufWritable<ItinMovement>(ItinMovementUtil.create(
                        ItinTimeUtil.create(tgtDate, tgtTime, 3L),
                        ItinTimeUtil.create(srcDate, srcTime, 2L)));
        List<Pair<ProtobufWritable<ItinRange>,
                  TypedProtobufWritable<Float64>>> results = this.instance
                .withInput(key, Arrays.asList(value))
                .run();
        assertNotNull(results);
        assertEquals(28, results.size());
    }
}

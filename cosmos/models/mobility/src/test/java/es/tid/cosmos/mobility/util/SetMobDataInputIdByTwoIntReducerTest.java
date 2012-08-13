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

package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class SetMobDataInputIdByTwoIntReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Message>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<InputIdRecord>> driver;
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<InputIdRecord>>(
                        new SetMobDataInputIdByTwoIntReducer());
    }

    @Test(expected=IllegalArgumentException.class)
    public void testNoInputId() {
        this.driver.runTest();
    }

    @Test
    public void testSetInputId() throws IOException {
        Configuration conf = this.driver.getConfiguration();
        conf.setInt("input_id", 3);
        List<Pair<ProtobufWritable<TwoInt>, TypedProtobufWritable<InputIdRecord>>> res =
                this.driver
                        .withInput(TwoIntUtil.createAndWrap(1L, 2L),
                                   asList(new TypedProtobufWritable<Message>(
                                           Null.getDefaultInstance())))
                        .run();
        assertEquals(1, res.size());
        TypedProtobufWritable<InputIdRecord> output = res.get(0).getSecond();
        assertEquals(3, output.get().getInputId());
    }
}

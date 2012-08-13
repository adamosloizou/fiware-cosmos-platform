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
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <ItinRange, ClusterVector>
 * Output: <TwoInt, ClusterVector>
 *
 * @author dmicol
 */
class MatrixSpreadVectorByPairMapper extends Mapper<
        ProtobufWritable<ItinRange>, TypedProtobufWritable<ClusterVector>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<ClusterVector>> {
    @Override
    protected void map(ProtobufWritable<ItinRange> key,
            TypedProtobufWritable<ClusterVector> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(ItinRange.class);
        final ItinRange itrang = key.get();
        context.write(TwoIntUtil.createAndWrap(itrang.getPoiSrc(),
                                               itrang.getPoiTgt()), value);
    }
}

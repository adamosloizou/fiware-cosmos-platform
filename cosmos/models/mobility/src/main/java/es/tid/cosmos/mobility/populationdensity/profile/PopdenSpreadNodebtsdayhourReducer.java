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

package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.NodeBtsDateUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <NodeBtsDate, Null>
 *
 * @author ximo
 */
public class PopdenSpreadNodebtsdayhourReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Cdr>, ProtobufWritable<NodeBtsDate>,
        TypedProtobufWritable<Null>> {
    private static List<Cell> cells = null;
    private static final TypedProtobufWritable<Null> nullValue =
            new TypedProtobufWritable<Null>(Null.getDefaultInstance());

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        if (cells == null) {
            final MobilityConfiguration conf = new MobilityConfiguration(context.
                    getConfiguration());
            cells = CellsCatalogue.load(new Path(conf.get("cells")), conf);
        }
    }

    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Cdr>> values, Context context)
            throws IOException, InterruptedException {
        List<Cell> filteredCells = CellsCatalogue.filter(cells, key.get());
        for (final TypedProtobufWritable<Cdr> value : values) {
            final Cdr cdr = value.get();
            for (Cell cell : filteredCells) {
                context.write(NodeBtsDateUtil.createAndWrap(cdr.getUserId(),
                                      cell.getBts(), cdr.getDate(),
                                      cdr.getTime().getHour()),
                              nullValue);
            }
        }
    }
}

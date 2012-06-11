package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, PoiNew>
 * Output: <Long, PoiNew>
 * 
 * @author dmicol
 */
public class AdjSpreadPoisByPoiIdMapper extends Mapper<
        ProtobufWritable<TwoInt>, MobilityWritable<PoiNew>, LongWritable,
        MobilityWritable<PoiNew>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            MobilityWritable<PoiNew> value, Context context)
            throws IOException, InterruptedException {
        final PoiNew poi = value.get();
        context.write(new LongWritable(poi.getId()), value);
    }
}

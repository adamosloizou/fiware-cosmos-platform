package es.tid.cosmos.mobility.mivs;

import java.io.IOException;
import java.util.List;
import java.util.ArrayList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobViMobVarsUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobProtocol.MobViMobVars;

/**
 *
 * @author losa
 */
public class FusionTotalVarsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobVars>, LongWritable,
        ProtobufWritable<MobViMobVars>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobVars>> values, Context context)
            throws IOException, InterruptedException {
        List<MobVars> allAreas = new ArrayList<MobVars>();
        for (ProtobufWritable<MobVars> value: values) {
            value.setConverter(MobVars.class);
            allAreas.add(value.get());
        }
        ProtobufWritable<MobViMobVars> wrappedAreas =
                MobViMobVarsUtil.createAndWrap(allAreas);
        context.write(key, wrappedAreas);
    }
}

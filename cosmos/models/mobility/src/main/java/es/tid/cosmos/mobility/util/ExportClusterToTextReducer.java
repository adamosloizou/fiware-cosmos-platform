package es.tid.cosmos.mobility.util;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;

/**
 * Input: <Long, Cluster>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class ExportClusterToTextReducer extends Reducer<
        LongWritable, MobilityWritable<Cluster>, NullWritable, Text> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<Cluster> value : values) {
            final Cluster cluster = value.get();
            context.write(NullWritable.get(),
                          new Text(key + ClusterUtil.DELIMITER
                                   + ClusterUtil.toString(cluster)));
        }
    }
}

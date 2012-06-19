package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;

/**
 * Input: <Long, Cluster>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
class VectorOneidOutReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Cluster>, NullWritable, Text> {
    private String separator;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf =
                (MobilityConfiguration) context.getConfiguration();
        this.separator = conf.getDataSeparator();
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        for (TypedProtobufWritable<Cluster> value : values) {
            final Cluster cluster = value.get();
            String output =
                    key.get() + this.separator
                    + cluster.getLabel() + this.separator
                    + cluster.getLabelgroup() + this.separator
                    + cluster.getConfident();
            for (double comm :cluster.getCoords().getComsList()) {
                 output += this.separator + comm;
            }
            context.write(NullWritable.get(), new Text(output));
        }
    }
}

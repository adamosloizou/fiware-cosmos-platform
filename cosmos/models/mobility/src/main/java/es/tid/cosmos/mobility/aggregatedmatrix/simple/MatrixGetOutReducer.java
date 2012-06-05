package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, ClusterVector>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class MatrixGetOutReducer extends Reducer<ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>, NullWritable, Text> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt pairId = key.get();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final ClusterVector moves = mobData.getClusterVector();
            String output = pairId.getNum1() + TwoIntUtil.DELIMITER
                            + pairId.getNum2();
            for (Double move : moves.getComsList()) {
                output += TwoIntUtil.DELIMITER + move;
            }
            context.write(NullWritable.get(), new Text(output));
        }
    }
}

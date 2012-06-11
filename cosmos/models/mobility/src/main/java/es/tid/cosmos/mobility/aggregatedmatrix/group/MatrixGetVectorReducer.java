package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <ItinRange, ItinPercMove>
 * Output: <TwoInt, ClusterVector>
 *
 * @author dmicol
 */
public class MatrixGetVectorReducer extends Reducer<
        ProtobufWritable<ItinRange>, MobilityWritable<ItinPercMove>,
        ProtobufWritable<TwoInt>, MobilityWritable<ClusterVector>> {
    @Override
    protected void reduce(ProtobufWritable<ItinRange> key,
            Iterable<MobilityWritable<ItinPercMove>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(ItinRange.class);
        final ItinRange moveRange = key.get();
        ClusterVector.Builder moves = ClusterVector.newBuilder();
        for (int i = 0; i < 168; i++) {
            moves.addComs(0.0D);
        }
        for (MobilityWritable<ItinPercMove> value : values) {
            final ItinPercMove percMoves = value.get();
            int j = percMoves.getGroup() - 1;  // Vector starts on Monday
            j = (j >= 0) ? j : 6;  // Sunday at the end
            j *= 24;
            j += percMoves.getRange();
            moves.setComs(j, percMoves.getPercMoves());
        }
        ProtobufWritable<TwoInt> pairGroup = TwoIntUtil.createAndWrap(
                moveRange.getPoiSrc(), moveRange.getPoiTgt());
        context.write(pairGroup, new MobilityWritable<ClusterVector>(
                moves.build()));
    }
}

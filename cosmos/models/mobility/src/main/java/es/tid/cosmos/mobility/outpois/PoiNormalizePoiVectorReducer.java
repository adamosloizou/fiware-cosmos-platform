package es.tid.cosmos.mobility.outpois;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Cluster>
 * Output: <TwoInt, Cluster>
 * 
 * @author dmicol
 */
public class PoiNormalizePoiVectorReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Cluster> clusterList = new LinkedList<Cluster>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            clusterList.add(value.get().getCluster());
        }
        if (clusterList.isEmpty()) {
            return;
        }

        final Cluster.Builder clusterSumBuilder = Cluster.newBuilder(
                clusterList.get(0));
        ClusterVector.Builder clusterSumCoordsBuilder =
                ClusterVector.newBuilder(clusterSumBuilder.getCoords());
        for (int i = 1; i < clusterList.size(); i++) {
            final Cluster cluster = clusterList.get(i);
            clusterSumCoordsBuilder.setComs(i,
                                            clusterSumCoordsBuilder.getComs(i)
                                            + cluster.getCoords().getComs(i));
        }
        clusterSumBuilder.setCoords(clusterSumCoordsBuilder);
        Cluster clusterSum = clusterSumBuilder.build();

        final Cluster.Builder clusterDivBuilder = Cluster.newBuilder(
                clusterSum);
        ClusterVector.Builder clusterDivCoordsBuilder =
                ClusterVector.newBuilder(clusterSum.getCoords());
        int sumValues = 0;
        for (int i = 0; i < clusterSum.getCoords().getComsCount(); i++) {
            double coms = clusterSum.getCoords().getComs(i);
            if (i < 24) {
                // Mondays, Tuesday, Wednesday and Thursday --> Total: 103 days
                coms /= 121.0D;
            } else {
                coms /= 31.0D;
            }
            sumValues += coms;
            clusterDivCoordsBuilder.addComs(coms);
        }
        clusterDivBuilder.setCoords(clusterDivCoordsBuilder);
        Cluster clusterDiv = clusterDivBuilder.build();

        final Cluster.Builder clusterNormBuilder = Cluster.newBuilder(
                clusterDiv);
        ClusterVector.Builder clusterNormCoordsBuilder =
                ClusterVector.newBuilder(clusterDiv.getCoords());
        for (int i = 0; i < clusterDiv.getCoords().getComsCount(); i++) {
            double coms = clusterDiv.getCoords().getComs(i);
            if (sumValues != 0) {
                coms /= sumValues;
            }
            clusterNormCoordsBuilder.addComs(coms);
        }
        clusterNormBuilder.setCoords(clusterNormCoordsBuilder);
        context.write(key,
                      MobDataUtil.createAndWrap(clusterNormBuilder.build()));
    }
}

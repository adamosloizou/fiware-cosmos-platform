package es.tid.cosmos.mobility.clientbtslabelling;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.util.CentroidsCatalogue;

/**
 *
 * @author dmicol
 */
public class ClusterClientBtsGetMinDistanceToClusterReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<ClusterVector>,
        ProtobufWritable<TwoInt>, ProtobufWritable<Cluster>> {
    private static CentroidsCatalogue centroids = null;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        if (centroids == null) {
            centroids = new CentroidsCatalogue(new Path(conf.get("centroids")),
                    conf);
        }
    }

    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<ClusterVector> value : values) {
            value.setConverter(ClusterVector.class);
            final ClusterVector clusVector = value.get();
            double mindist = 1000D;
            Cluster minDistCluster = null;
            for (Cluster cluster : centroids.getCentroids()) {
                double sumcom = 0;
                double sumccom = 0;
                double csumcom = 0;
                double csumccom = 0;
                double sumcomxccom = 0;
                for (int nComs = 0; nComs < clusVector.getComsCount(); nComs++) {
                    double com = clusVector.getComs(nComs);
                    double ccom = cluster.getCoords().getComs(nComs);
                    sumcom += com;				// S1
                    sumccom += ccom;			        // S2
                    csumcom += com * com;		        // SC1
                    csumccom += ccom * ccom;	                // SC2
                    sumcomxccom += com * ccom;	                // S12
                }

                int nElem = clusVector.getComsCount();
                double dist = 1.0 - (((nElem * sumcomxccom) - (sumcom * sumccom))
                        / (Math.sqrt((nElem * csumcom) - (sumcom * sumcom))
                        * Math.sqrt((nElem * csumccom) - (sumccom * sumccom))));
                if (dist < mindist || minDistCluster == null) {
                    mindist = dist;
                    minDistCluster = cluster;
                }
            }
            mindist = Math.sqrt(mindist);

            key.setConverter(NodeBts.class);
            final NodeBts nodeBts = key.get();
            ProtobufWritable<TwoInt> twoInt = TwoIntUtil.createAndWrap(
                    nodeBts.getUserId(), nodeBts.getPlaceId());
            ProtobufWritable<Cluster> cluster = ClusterUtil.createAndWrap(
                    minDistCluster.getLabel(), minDistCluster.getLabelgroup(),
                    mindist > minDistCluster.getDistance() ? 0 : 1,
                    0D, mindist, clusVector);
            context.write(twoInt, cluster);
        }
    }
}

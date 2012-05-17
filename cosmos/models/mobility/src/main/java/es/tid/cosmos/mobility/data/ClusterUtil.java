package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;

/**
 *
 * @author sortega
 */
public abstract class ClusterUtil {
    public static final String DELIMITER = "|";
    
    public static Cluster create(int label, int labelgroup, int confident,
            double mean, double distance, ClusterVector coords) {
        return Cluster.newBuilder()
                .setLabel(label)
                .setLabelgroup(labelgroup)
                .setConfident(confident)
                .setMean(mean)
                .setDistance(distance)
                .setCoords(coords)
                .build();
    }

    public static ProtobufWritable<Cluster> wrap(Cluster obj) {
        ProtobufWritable<Cluster> wrapper = ProtobufWritable.newInstance(
                Cluster.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<Cluster> createAndWrap(int label,
            int labelgroup, int confident, double mean, double distance,
            ClusterVector coords) {
        return wrap(create(label, labelgroup, confident, mean, distance, coords));
    }

    public static String toString(Cluster obj) {
        String str = obj.getLabel() + DELIMITER + obj.getLabelgroup()
                + DELIMITER + obj.getConfident() + DELIMITER + obj.getMean()
                + DELIMITER + obj.getDistance();
        for (double comm : obj.getCoords().getComsList()) {
            str += DELIMITER + comm;
        }
        return str;
    }
}
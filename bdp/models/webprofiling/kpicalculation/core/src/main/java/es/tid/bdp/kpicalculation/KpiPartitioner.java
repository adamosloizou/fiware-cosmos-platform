package es.tid.bdp.kpicalculation;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Partitioner;

import es.tid.bdp.base.mapreduce.CompositeKey;

/**
 * This class partitions the data output from the map phase before it is sent
 * through the shuffle phase.
 * 
 * The main method to pay attention to in this example is called
 * "getPartition()"
 * 
 * getPartition() determines how we group the data; In the case of this
 * secondary sort example this function partitions the data by only the first
 * half of the key
 * 
 * @author javierb
 */
public class KpiPartitioner extends Partitioner<CompositeKey, IntWritable> {
    private static final int HASH_CODE_FACTOR = 127;
    
    /**
     * Get the paritition number for a given key, in this case the partition is
     * calculate from the naturalKey of the structure CompositeKey
     * 
     * @param key
     *            the key to be paritioned.
     * @param value
     *            the entry value.
     * @param numPartitions
     *            the total number of partitions.
     * @return the partition number for the <code>key</code>.
     */
    @Override
    public int getPartition(CompositeKey key, IntWritable value,
                            int numPartitions) {
        return Math.abs(key.get(0).hashCode() * HASH_CODE_FACTOR)
                % numPartitions;
    }
}

package es.tid.cosmos.samples.wordcount;

import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import static org.junit.Assert.assertTrue;

/**
 * Base class for job tests
 *
 * @author sortega
 */
public class JobTest {
    public static final Class<? extends Mapper> DEFAULT_MAPPER = Mapper.class;

    public static void assertMRChain(Job instance,
            Class<? extends Mapper> mapperClass,
            Class<? extends Reducer> reducerClass) throws Exception {
        assertTrue("mapper class mismatch",
                   instance.getMapperClass().isAssignableFrom(mapperClass));
        assertTrue("reducer class mismatch",
                   instance.getReducerClass().isAssignableFrom(reducerClass));
    }
}

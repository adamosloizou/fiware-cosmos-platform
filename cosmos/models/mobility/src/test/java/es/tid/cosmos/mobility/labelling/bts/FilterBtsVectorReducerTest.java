package es.tid.cosmos.mobility.labelling.bts;

import java.io.InputStream;
import java.io.IOException;
import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.BtsUtil;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class FilterBtsVectorReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new FilterBtsVectorReducer());
        InputStream configInput = Config.class.getResource(
                "/mobility.properties").openStream();
        this.driver.setConfiguration(Config.load(configInput,
                this.driver.getConfiguration()));
    }

    @Test
    public void testNonConfidentOutput() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                BtsUtil.create(1, 50000, 2, 3, 6, asList(5L, 6L, 7L)));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 0, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }

    @Test
    public void testConfidentOutput() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                BtsUtil.create(1, 80000, 2, 3, 4, asList(5L, 6L, 7L)));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
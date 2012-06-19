package es.tid.cosmos.mobility.parsing;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjParseAdjBtsMapperTest {
    private MapDriver<LongWritable, Text, ProtobufWritable<TwoInt>,
            TypedProtobufWritable<Null>> driver;
    
    @Before
    public void setUp() throws IOException {
        this.driver = new MapDriver<LongWritable, Text,
                ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>>(
                        new AdjParseAdjBtsMapper());
        InputStream configInput = MobilityConfiguration.class.getResource(
                "/mobility.properties").openStream();
        MobilityConfiguration conf = new MobilityConfiguration();
        conf.load(configInput);
        this.driver.setConfiguration(conf);
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>>> res =
                this.driver
                        .withInput(new LongWritable(1L), new Text("123|456"))
                        .run();
        assertNotNull(res);
        assertEquals(1, res.size());
        ProtobufWritable<TwoInt> wrappedKey = res.get(0).getFirst();
        wrappedKey.setConverter(TwoInt.class);
        assertNotNull(wrappedKey.get());
        TypedProtobufWritable<Null> wrappedNull = res.get(0).getSecond();
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           new Text("2221436242|blah blah|1234"))
                .runTest();
    }
}

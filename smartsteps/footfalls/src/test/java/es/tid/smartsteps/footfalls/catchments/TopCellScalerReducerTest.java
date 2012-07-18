package es.tid.smartsteps.footfalls.catchments;

import java.io.IOException;
import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author sortega
 */
public class TopCellScalerReducerTest extends CatchmentsTestBase {

    private ReduceDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<Catchments>> instance;
    private Text key;

    public TopCellScalerReducerTest() throws IOException {}

    @Before
    public void setUp() {
        this.instance = new ReduceDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<Catchments>>(
                        new TopCellScalerReducer());
        this.key = new Text("src");
    }

    @Test
    public void shouldScaleCounts() throws Exception {
        Lookup lookup = Lookup.newBuilder()
                .setKey("src")
                .setValue("dst")
                .setProportion(0.4d)
                .build();
        this.instance.withInput(this.key,
                asList(new TypedProtobufWritable<Message>(lookup),
                       new TypedProtobufWritable<Message>(
                        singletonCatchment("topLevelId", 22, "src", 100d))))
                .withOutput(this.key, new TypedProtobufWritable<Catchments>(
                        singletonCatchment("topLevelId", 22, "dst", 40d)))
                .runTest();
    }

    @Test
    public void shouldHaveMultipleWrites() throws Exception {
        Lookup lookup1 = Lookup.newBuilder()
                .setKey("src")
                .setValue("dst1")
                .setProportion(0.4d)
                .build();
        Lookup lookup2 = Lookup.newBuilder()
                .setKey("src")
                .setValue("dst2")
                .setProportion(0.6d)
                .build();
        int results = this.instance.withInput(this.key,
                asList(new TypedProtobufWritable<Message>(lookup1),
                       new TypedProtobufWritable<Message>(lookup2),
                       new TypedProtobufWritable<Message>(
                        singletonCatchment("tld1", 22, "src", 100d)),
                       new TypedProtobufWritable<Message>(
                        singletonCatchment("tld2",  8, "src",  10d)),
                       new TypedProtobufWritable<Message>(
                        singletonCatchment("tld3",  8, "src", 200d))))
                .run().size();
        assertEquals(6, results);
    }

    @Test
    public void shouldCountDiscatedEntries() throws Exception {
        this.instance.withInput(this.key,
                asList(new TypedProtobufWritable<Message>(
                        singletonCatchment("tld1", 22, "src", 100d)),
                       new TypedProtobufWritable<Message>(
                        singletonCatchment("tld2",  8, "src",  10d)),
                       new TypedProtobufWritable<Message>(
                        singletonCatchment("tld3",  8, "src", 200d))))
                .runTest();
        assertEquals(3, this.instance.getCounters()
                .findCounter(Counters.ENTRIES_NOT_IN_LOOKUP).getValue());
    }
}

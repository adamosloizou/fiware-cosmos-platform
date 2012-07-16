package es.tid.cosmos.mobility.labelling.secondhomes;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class PoiCellToBtsMapperTest {
    private MapDriver<LongWritable, TypedProtobufWritable<Cell>, LongWritable,
            TypedProtobufWritable<Cell>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, TypedProtobufWritable<Cell>,
                LongWritable, TypedProtobufWritable<Cell>>(new PoiCellToBtsMapper());
    }

    @Test
    public void testReduce() {
        TypedProtobufWritable<Cell> value = new TypedProtobufWritable<Cell>(
                CellUtil.create(12321L, 432L, 40, 50, 3.21D, 54.5D));
        this.driver
                .withInput(new LongWritable(57L), value)
                .withOutput(new LongWritable(432L), value)
                .runTest();
    }
}

package es.tid.cosmos.mobility.aggregatedmatrix.group;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.MatrixRangeUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Float64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixRange;

/**
 *
 * @author dmicol
 */
public class MatrixCountRangesReducerTest {
    private ReduceDriver<ProtobufWritable<MatrixRange>, MobilityWritable<Float64>,
            ProtobufWritable<MatrixRange>, MobilityWritable<ItinPercMove>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<MatrixRange>,
                MobilityWritable<Float64>, ProtobufWritable<MatrixRange>,
                MobilityWritable<ItinPercMove>>(new MatrixCountRangesReducer());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<MatrixRange> key = MatrixRangeUtil.createAndWrap(
                1, 2, 3, 4, 5);
        final MobilityWritable<Float64> value1 = MobilityWritable.create(5.7D);
        final MobilityWritable<Float64> value2 = MobilityWritable.create(8.8D);
        final ProtobufWritable<MatrixRange> outKey =
                MatrixRangeUtil.createAndWrap(1, 2, 3, 0, 0);
        final MobilityWritable<ItinPercMove> outValue = new MobilityWritable<ItinPercMove>(
                ItinPercMoveUtil.create(4, 5, 14.5D));
        this.instance
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}

package es.tid.cosmos.mobility.labelling.clientbts;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, BtsCounter>
 * Output: <NodeBts, TwoInt>
 * 
 * @author dmicol
 */
public class VectorFiltClientbtsReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<InputIdRecord>,
        ProtobufWritable<NodeBts>, MobilityWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<InputIdRecord>> values, Context context)
            throws IOException, InterruptedException {
        int reprBtsCounterCount = 0;
        List<BtsCounter> sumBtsCounterList = new LinkedList<BtsCounter>();
        for (MobilityWritable<InputIdRecord> value : values) {
            final InputIdRecord record = value.get();
            switch (record.getInputId()) {
                case 0:
                    final BtsCounter counter = BtsCounter.parseFrom(
                            record.getMessageBytes());
                    sumBtsCounterList.add(counter);
                    break;
                case 1:
                    reprBtsCounterCount++;
                    break;
                default:
                    throw new IllegalStateException("Unexpected input ID: "
                            + record.getInputId());
            }
        }
        
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (int i = 0; i < reprBtsCounterCount; i++) {
            for (BtsCounter sumBtsCounter : sumBtsCounterList) {
                ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                        twoInt.getNum1(), (int)twoInt.getNum2(),
                        sumBtsCounter.getWeekday(), 0);
                TwoInt hourComs = TwoIntUtil.create(sumBtsCounter.getRange(),
                                                    sumBtsCounter.getCount());
                context.write(nodeBts, new MobilityWritable<TwoInt>(hourComs));
            }
        }
    }
}

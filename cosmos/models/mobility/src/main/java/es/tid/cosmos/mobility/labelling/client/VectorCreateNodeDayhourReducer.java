package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.DailyVector;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 * Input: <NodeBts, TwoInt>
 * Output: <TwoInt, DailyVector>
 * 
 * @author dmicol
 */
public class VectorCreateNodeDayhourReducer extends Reducer
        <ProtobufWritable<NodeBts>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts bts = key.get();
        
        List<TwoInt> valueList = new LinkedList<TwoInt>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final TwoInt hourComms = value.get().getTwoInt();
            valueList.add(hourComms);
        }
        
        DailyVector.Builder vectorBuilder = DailyVector.newBuilder();
        for (int hour = 0; hour < 24; hour++) {
            long num2 = 0L;
            boolean added = false;
            for (TwoInt hourComms : valueList) {
                num2 = hourComms.getNum2();
                if (hourComms.getNum1() == hour) {
                    added = true;
                    break;
                }
            }
            if (!added) {
                num2 = 0L;
            }
            TwoInt hourComms = TwoIntUtil.create(bts.getWeekday(), num2);
            vectorBuilder.addHours(hourComms);
        }
        ProtobufWritable<TwoInt> node = TwoIntUtil.createAndWrap(
                bts.getUserId(), bts.getPlaceId());
        context.write(node, MobDataUtil.createAndWrap(vectorBuilder.build()));
    }
}

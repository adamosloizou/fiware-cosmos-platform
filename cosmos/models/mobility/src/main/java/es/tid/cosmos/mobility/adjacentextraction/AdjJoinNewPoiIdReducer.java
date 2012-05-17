package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Long|PoiNew>
 * Output: <TwoInt, PoiNew>
 * 
 * @author dmicol
 */
public class AdjJoinNewPoiIdReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Long> longList = new LinkedList<Long>();
        List<PoiNew> poiNewList = new LinkedList<PoiNew>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            MobData mobData = value.get();
            switch (mobData.getType()) {
                case LONG:
                    longList.add(mobData.getLong());
                    break;
                case POI_NEW:
                    poiNewList.add(mobData.getPoiNew());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        
        for (long poiMod : longList) {
            for (PoiNew poi : poiNewList) {
                PoiNew.Builder outputPoiBuilder = PoiNew.newBuilder(poi);
                outputPoiBuilder.setId((int)poiMod);
                context.write(TwoIntUtil.createAndWrap(poi.getNode(),
                                                       poi.getBts()),
                              MobDataUtil.createAndWrap(
                                      outputPoiBuilder.build()));
            }
        }
    }
}

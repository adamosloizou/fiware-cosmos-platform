package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.PoiPosUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;

/**
 * Input: <Long, Poi|Cell>
 * Output: <Long, PoiPos>
 * 
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiPosReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Poi> poiList = new LinkedList<Poi>();
        Cell cell = null;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI:
                    poiList.add(mobData.getPoi());
                    break;
                case CELL:
                    if (cell == null) {
                        cell = mobData.getCell();
                    }
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        
        for (Poi poi : poiList) {
            PoiPos poiPos = PoiPosUtil.create(
                    poi.getNode(), poi.getBts(),
                    poi.getConfidentnodebts() == 0 ? 0 :
                                                     poi.getLabelgroupnodebts(),
                    cell.getPosx(), cell.getPosy(), poi.getInoutWeek(),
                    poi.getInoutWend(), -1D, -1D, -1D, -1D);
            context.write(new LongWritable(poi.getNode()),
                          MobDataUtil.createAndWrap(poiPos));
        }
    }
}
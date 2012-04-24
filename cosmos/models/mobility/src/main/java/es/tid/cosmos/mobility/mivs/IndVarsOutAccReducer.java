package es.tid.cosmos.mobility.mivs;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobVarsUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobProtocol.MobViMobVars;

/**
 *
 * @author logc
 */
public class  IndVarsOutAccReducer extends Reducer<LongWritable,
        ProtobufWritable<MobViMobVars>, NullWritable, Text> {
    private static final String DELIMITER = "|";
    private static final String MISSING = "-1|-1|-1|-1|-1|-1|-1|-1";

    @Override
    public void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobViMobVars>> values,
            Context context) throws IOException, InterruptedException {
        for (ProtobufWritable<MobViMobVars> value : values) {
            value.setConverter(MobViMobVars.class);
            final MobViMobVars activityAreas = value.get();
            List<MobVars> areasList = activityAreas.getVarsList();
            boolean exists = false;
            String ans = key.toString();
            for (MobVars area : areasList) {
                if (area.getWorkingday()) {
                    ans += DELIMITER + MobVarsUtil.toString(area);
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                ans += DELIMITER + MISSING;
            }
            exists = false;
            for (MobVars area : areasList) {
                if (!area.getWorkingday()) {
                    ans += DELIMITER + MobVarsUtil.toString(area);
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                ans += DELIMITER + MISSING;
            }
            context.write(NullWritable.get(), new Text(ans));
        }
    }
}

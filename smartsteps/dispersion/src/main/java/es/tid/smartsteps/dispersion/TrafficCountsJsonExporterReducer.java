package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.List;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.config.Config;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.Counts;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 *
 * @author dmicol
 */
class TrafficCountsJsonExporterReducer extends Reducer<
        Text, TypedProtobufWritable<TrafficCounts>,
        NullWritable, Text> {

    private boolean shouldRoundResults;
    private Text outValue;
    
    @Override
    protected void setup(Context context) {
        this.shouldRoundResults = context.getConfiguration().getBoolean(
                Config.ROUND_RESULTS, false);
        this.outValue = new Text();
    }
    
    @Override
    protected void reduce(Text key,
            Iterable<TypedProtobufWritable<TrafficCounts>> values,
            Context context) throws IOException, InterruptedException {
        for (TypedProtobufWritable<TrafficCounts> value : values) {
            this.outValue.set(
                    toJson(value.get(), this.shouldRoundResults).toString());
            context.write(NullWritable.get(), this.outValue);
        }
    }
    
    private static JSONObject toJson(TrafficCounts counts, boolean shouldRound) {
        final JSONObject obj = new JSONObject();
        obj.put(TrafficCountsParser.CELLID_FIELD_NAME, counts.getCellId());
        obj.put(TrafficCountsParser.DATE_FIELD_NAME, counts.getDate());
        obj.put(TrafficCountsParser.LATITUDE_FIELD_NAME, counts.getLatitude());
        obj.put(TrafficCountsParser.LONGITUDE_FIELD_NAME, counts.getLongitude());
        for (Counts footfallCounts : counts.getFootfallsList()) {
            obj.put(footfallCounts.getName(),
                    toJsonArray(footfallCounts.getValuesList(), shouldRound));
        }
        JSONObject pois = new JSONObject();
        for (Counts poiCounts : counts.getPoisList()) {
            pois.put(poiCounts.getName(),
                     toJsonArray(poiCounts.getValuesList(), shouldRound));
        }
        obj.put(TrafficCountsParser.POIS_FIELD_NAME, pois);
        return obj;
    }
    
    private static JSONArray toJsonArray(List<Double> values,
                                         boolean shouldRound) {
        JSONArray array = new JSONArray();
        for (int i = 0; i < values.size(); i++) {
            double count = values.get(i);
            if (shouldRound) {
                array.add(i, (long) Math.round(count));
            } else {
                array.add(i, count);
            }
        }
        return array;
    }
}

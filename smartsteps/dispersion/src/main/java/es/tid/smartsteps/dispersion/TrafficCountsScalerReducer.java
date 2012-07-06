package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.Counts;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author dmicol
 */
class TrafficCountsScalerReducer extends Reducer<
        Text, TypedProtobufWritable<Message>,
        Text, TypedProtobufWritable<TrafficCounts>> {
    
    private Text outKey;
    private TypedProtobufWritable<TrafficCounts> scaledCounts;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.outKey = new Text();
        this.scaledCounts = new TypedProtobufWritable<TrafficCounts>();
    }
 
    @Override
    protected void reduce(Text key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = TypedProtobufWritable.groupByClass(
                values, TrafficCounts.class, Lookup.class);
        List<TrafficCounts> counts = dividedLists.get(TrafficCounts.class);
        List<Lookup> lookups = dividedLists.get(Lookup.class);
        if (lookups.isEmpty()) {
            context.getCounter(Counters.ENTRIES_NOT_IN_LOOKUP)
                   .increment(lookups.size());
            return;
        }
        
        for (TrafficCounts count : counts) {
            context.getCounter(Counters.ENTRIES_IN_LOOKUP).increment(1L);
            for (Lookup lookup : lookups) {
                TrafficCounts.Builder scaledCountsBuilder =
                        TrafficCounts.newBuilder(count);
                scaledCountsBuilder.setCellId(lookup.getValue());
                for (int i = 0; i < count.getFootfallsCount(); i++) {
                    scaledCountsBuilder.setFootfalls(i,
                            scaleCounts(count.getFootfalls(i),
                                        lookup.getProportion()));
                }
                for (int i = 0; i < count.getPoisCount(); i++) {
                    scaledCountsBuilder.setPois(i,
                            scaleCounts(count.getPois(i),
                                        lookup.getProportion()));
                }
                this.outKey.set(scaledCountsBuilder.getCellId());
                this.scaledCounts.set(scaledCountsBuilder.build());
                context.write(this.outKey, this.scaledCounts);
            }
        }
    }
    
    private static Counts scaleCounts(Counts a, double proportion) {
        Counts.Builder scaledCounts = Counts.newBuilder();
        scaledCounts.setName(a.getName());
        for (int i = 0; i < a.getValuesCount(); i++) {
            scaledCounts.addValues(a.getValues(i) * proportion);
        }
        return scaledCounts.build();
    }
}

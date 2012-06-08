package es.tid.cosmos.mobility.preparing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 * Input: <Long, Cdr>
 * Output: <Long, Cdr>
 * 
 * @author dmicol
 */
public class FilterCellnoinfoMapper extends Mapper<LongWritable,
        MobilityWritable<Cdr>, LongWritable, MobilityWritable<Cdr>> {
    @Override
    public void map(LongWritable key, MobilityWritable<Cdr> value,
                    Context context) throws IOException, InterruptedException {
        final Cdr cdr = value.get();
        if (cdr.getCellId() != 0) {
            context.write(new LongWritable(cdr.getCellId()), value);
        } else {
            context.write(key, value);
        }
    }
}

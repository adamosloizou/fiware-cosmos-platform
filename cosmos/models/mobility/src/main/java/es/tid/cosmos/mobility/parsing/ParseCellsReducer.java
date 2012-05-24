package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <Long, Text>
 * Output: <Long, Cell>
 * 
 * @author dmicol, sortega
 */
public class ParseCellsReducer extends Reducer<LongWritable, Text, LongWritable,
        ProtobufWritable<MobData>> {
    private LongWritable cellId;
    
    @Override
    public void setup(Context context) {
        this.cellId = new LongWritable();
    }
    
    @Override
    public void reduce(LongWritable key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        for (Text value : values) {
            Cell cell;
            try {
                cell = new CellParser(value.toString()).parse();
            } catch (Exception ex) {
                context.getCounter(Counters.INVALID_LINES).increment(1L);
                continue;
            }
            this.cellId.set(cell.getCellId());
            context.write(this.cellId, MobDataUtil.createAndWrap(cell));
        }
    }
}

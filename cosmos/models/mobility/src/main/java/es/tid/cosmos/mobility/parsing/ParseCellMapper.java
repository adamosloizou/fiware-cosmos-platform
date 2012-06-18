package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 * Input: <Long, Text>
 * Output: <Long, Cell>
 * 
 * @author dmicol, sortega
 */
public class ParseCellMapper extends Mapper<LongWritable, Text, LongWritable,
        TypedProtobufWritable<Cell>> {
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            final Cell cell = new CellParser(value.toString()).parse();
            context.write(new LongWritable(cell.getCellId()),
                          new TypedProtobufWritable<Cell>(cell));
        } catch (Exception ex) {
            context.getCounter(Counters.INVALID_CELLS).increment(1L);
        }
    }
}

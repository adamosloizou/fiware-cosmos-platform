package es.tid.ps.kpicalculation;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.kpicalculation.data.WebLog;

/**
 * This class makes the reduce phase in the kpi aggregates grouped by field
 * calculation of the web profiling module. When the target aggregation is a
 * counter of unique values of a field of the items, depending on the values of
 * some other of its fields, this reducer will be used.
 * 
 * The fields will be received sorted from the sorting phase depending on their
 * primary key ( filtering fields ), so in the reduce phase the number of
 * different elements ( depending on the grouping field ) with the same primary
 * key will be counted. When an item with a different primary key is received,
 * the previous key is emitted to the output and a new counting process for the
 * new key is performed.
 * 
 * In order to emit the last key will be necessary to perform a write operation
 * in the cleanup phase.
 * 
 * @author javierb
 */
public class KpiCounterByReducer extends
        Reducer<WebLog, IntWritable, Text, IntWritable> {

    private String currentKey = "";
    private int currentValue = 0;
    private IntWritable counter;
    private Text text;

    /**
     * Method that creates the objects that will be used during the reduce
     * 
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.counter = new IntWritable();
        this.text = new Text();
    }

    /**
     * @param key
     *            is the key emitted by the mapper.
     * @param values
     *            are all the values aggregated during the mapping phase
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void reduce(WebLog key, Iterable<IntWritable> values,
            Context context) throws IOException, InterruptedException {

        if (!key.mainKey.equals(currentKey)) {
            if (!currentKey.equals("")) {
                this.setValues();
                context.write(text, counter);
            }
            currentKey = key.mainKey;
            currentValue = 1;
        } else {
            currentValue++;
        }
    }

    /**
     * The cleanup phase has to be override to be able to emit the last key
     * retrieved in the reduce process
     * 
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void cleanup(Context context) throws IOException,
            InterruptedException {
        this.setValues();
        context.write(text, counter);
        super.cleanup(context);
    }

    /**
     * Method that prepares the values to be emit before writing them to the
     * output.
     */
    private void setValues() {
        text.set(currentKey);
        counter.set(currentValue);
    }
}
package es.tid.ps.mapreduce.mobility;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;
import es.tid.ps.mapreduce.mobility.data.FileType;
import es.tid.ps.mapreduce.mobility.data.MobilityCounter;

/**
 * This class parses the CDRs files, and emit one value for each line, this line
 * is used for check that the calls have been done by users that we have their
 * data. For doing it we used Reduce-Side Joins, in where the Key is a composite
 * key of the user_id and a identifier of the file type. The format of file is:
 * TODO rgc: I dont know the meaning of all fields
 * <Phone_caller>|<Phone_receptor|....
 * 
 * Example:
 * <ol>
 * <li>Input: {key{line},value :
 * 33F4303E978D89|4531232401|33F4303E978D89|0454531232404|2|01
 * /06/2010|11:16:47|115|RMNACIO}</li>
 * <li>output(1): { key : {primary:33F4303E978D89, secondary:1}, values {} }</li>
 * </ol>
 * 
 * @author rgc
 */
public class JoinUserMapper extends
        Mapper<LongWritable, Text, CompositeKey, Text> {
    final private static String DELIMITER = "\\|";

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.mapreduce.Mapper#map(KEYIN, VALUEIN,
     * org.apache.hadoop.mapreduce.Mapper.Context)
     */
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        StringTokenizer st = new StringTokenizer(value.toString(), DELIMITER);
        if (st.hasMoreElements()) {
            context.write(
                    new CompositeKey(st.nextToken(), FileType.FILE_USERS
                            .getValue()), new Text(""));
        } else {
            context.getCounter(MobilityCounter.LINE_PARSER_USER_ERROR)
                    .increment(1L);
        }
    }
}

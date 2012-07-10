package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.DateUtil;
import es.tid.cosmos.mobility.data.generated.BaseProtocol;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 * Parses and filters by date the input CDRs.
 *
 * Input: <Long, Text>
 * Output: <Long, Cdr>
 * 
 * @author dmicol, sortega
 */
public class ParseCdrMapper extends Mapper<LongWritable, Text, LongWritable,
        TypedProtobufWritable<Cdr>> {

    private String separator;
    private DateParser dateParser;
    private BaseProtocol.Date startDate;
    private BaseProtocol.Date endDate;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(
                context.getConfiguration());
        this.separator = conf.getDataSeparator();
        this.dateParser = new DateParser(conf.getDataDateFormat());
        this.startDate = parseOptionalDate(conf.getDataStartDate());
        this.endDate = parseOptionalDate(conf.getDataEndDate());
    }

    private BaseProtocol.Date parseOptionalDate(String input) {
        if (input.trim().isEmpty()) {
            return null;
        }
        return this.dateParser.parse(input);
    }

    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            final Cdr cdr = new CdrParser(value.toString(), this.separator,
                                          this.dateParser).parse();
            if (this.isOnRange(cdr.getDate())) {
                context.write(new LongWritable(cdr.getUserId()),
                              new TypedProtobufWritable<Cdr>(cdr));
            }
        } catch (Exception ex) {
            context.getCounter(Counters.INVALID_CDRS).increment(1L);
        }
    }

    private boolean isOnRange(BaseProtocol.Date date) {
        if (this.startDate != null && DateUtil.compare(date, this.startDate) < 0) {
            return false;
        }
        if (this.endDate != null && DateUtil.compare(date, this.endDate) > 0) {
            return false;
        }
        return true;
    }
}

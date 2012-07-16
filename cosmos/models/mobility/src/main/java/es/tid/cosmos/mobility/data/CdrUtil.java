package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public final class CdrUtil {

    private CdrUtil() {}

    public static Cdr create(long userId, long cellId, Date date, Time time) {
        return Cdr.newBuilder()
                .setUserId(userId)
                .setCellId(cellId)
                .setDate(date)
                .setTime(time)
                .build();
    }

    public static ProtobufWritable<Cdr> wrap(Cdr obj) {
        ProtobufWritable<Cdr> wrapper = ProtobufWritable.newInstance(Cdr.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<Cdr> createAndWrap(long userId, long cellId,
                                                      Date date, Time time) {
        return wrap(create(userId, cellId, date, time));
    }
}

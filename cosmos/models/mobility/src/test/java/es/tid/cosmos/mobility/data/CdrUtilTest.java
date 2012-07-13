package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class CdrUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(CdrUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        long userId = 132L;
        long cell = 81L;
        Date date = Date.getDefaultInstance();
        Time time = Time.getDefaultInstance();
        ProtobufWritable<Cdr> wrapper = CdrUtil.createAndWrap(
                userId, cell, date, time);
        wrapper.setConverter(Cdr.class);
        Cdr obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(userId, obj.getUserId());
        assertEquals(cell, obj.getCellId());
        assertEquals(date, obj.getDate());
        assertEquals(time, obj.getTime());
    }
}

package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.MobVars;

/**
 *
 * @author dmicol
 */
public class MobVarsUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<MobVars> wrapper = MobVarsUtil.createAndWrap(1, true,
                2, 3, 4, 5, 6, 7, 8, 9);
        wrapper.setConverter(MobVars.class);
        final MobVars obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1, obj.getMonth());
        assertTrue(obj.getWorkingday());
        assertEquals(2, obj.getNumPos());
        assertEquals(3, obj.getDifBtss());
        assertEquals(4, obj.getDifMuns());
        assertEquals(5, obj.getDifStates());
        assertEquals(6.0D, obj.getMasscenterUtmx(), 0.0D);
        assertEquals(7.0D, obj.getMasscenterUtmy(), 0.0D);
        assertEquals(8.0D, obj.getRadius(), 0.0D);
        assertEquals(9.0D, obj.getDiamAreainf(), 0.0D);
    }
}

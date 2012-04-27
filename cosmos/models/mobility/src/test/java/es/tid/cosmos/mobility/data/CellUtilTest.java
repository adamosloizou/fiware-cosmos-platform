package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class CellUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<Cell> wrapper = CellUtil.createAndWrap(
                1L, 2L, 3, 4, 5D, 6D);
        wrapper.setConverter(Cell.class);
        Cell obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1L, obj.getCellId());
        assertEquals(2L, obj.getPlaceId());
        assertEquals(3, obj.getGeoloc1());
        assertEquals(4, obj.getGeoloc2());
        assertEquals(5D, obj.getPosx(), 0.0D);
        assertEquals(6D, obj.getPosy(), 0.0D);
    }
}

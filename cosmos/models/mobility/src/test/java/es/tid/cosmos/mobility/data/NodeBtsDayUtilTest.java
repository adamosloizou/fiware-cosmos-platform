package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class NodeBtsDayUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(NodeBtsDayUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        long userId = 132L;
        long placeId = 81L;
        int workday = 5;
        int count = 3;
        ProtobufWritable<NodeBtsDay> wrapper = NodeBtsDayUtil.createAndWrap(
                userId, placeId, workday, count);
        wrapper.setConverter(NodeBtsDay.class);
        NodeBtsDay obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(userId, obj.getUserId());
        assertEquals(placeId, obj.getBts());
        assertEquals(workday, obj.getWorkday());
        assertEquals(count, obj.getCount());
    }
}

package es.tid.cosmos.base.data;

import com.google.protobuf.Message;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

import es.tid.cosmos.base.data.generated.CdrProtocol.Uler;
import es.tid.cosmos.base.data.generated.WebLogProtocol.WebLog;

/**
 *
 * @author dmicol
 */
public class MessageGeneratorTest {
    @Test
    public void shouldGenerateUler() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.set("type", "uler");
        messageDescriptor.set("hashed_uid", "ABC");
        messageDescriptor.set("cell_lat", "76.3");
        messageDescriptor.set("cell_long", "0.5");
        messageDescriptor.set("event_day", "27/05/2012");
        messageDescriptor.set("event_time", "18:00");
        messageDescriptor.set("cell_id", "DEF");
        messageDescriptor.set("event_type", "call");
        messageDescriptor.set("event_duration", "2:04");
        messageDescriptor.set("age", "28");
        messageDescriptor.set("gender", "male");
        messageDescriptor.set("socio_eco_state", "unknown");
        messageDescriptor.set("home_city", "mutxamel");
        messageDescriptor.set("home_zip_code", "03110");
        Message message = MessageGenerator.generate(messageDescriptor);
        assertTrue(message instanceof Uler);
        final Uler uler = (Uler)message;
        assertEquals("ABC", uler.getHashedUid());
        assertEquals(76.3F, uler.getCellLat(), 0.0F);
        assertEquals(0.5F, uler.getCellLong(), 0.0F);
        assertEquals("27/05/2012", uler.getEventDay());
        assertEquals("18:00", uler.getEventTime());
        assertEquals("DEF", uler.getCellId());
        assertEquals("call", uler.getEventType());
        assertEquals("2:04", uler.getEventDuration());
        assertEquals(28, uler.getAge());
        assertEquals("male", uler.getGender());
        assertEquals("unknown", uler.getSocioEcoState());
        assertEquals("mutxamel", uler.getHomeCity());
        assertEquals("03110", uler.getHomeZipCode());
    }
    
    @Test
    public void shouldGenerateWebLog() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.set("type", "weblog");
        messageDescriptor.set("user_id", "13213AB");
        messageDescriptor.set("url", "http://www.google.com");
        messageDescriptor.set("date", "27/05/2012");
        Message message = MessageGenerator.generate(messageDescriptor);
        assertTrue(message instanceof WebLog);
        final WebLog webLog = (WebLog)message;
        assertEquals("13213AB", webLog.getUserId());
        assertEquals("http://www.google.com", webLog.getUrl());
        assertEquals("27/05/2012", webLog.getDate());
    }
    
    @Test(expected=IllegalArgumentException.class)
    public void shouldFailOnInvalidType() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.set("type", "invalid");
        MessageGenerator.generate(messageDescriptor);
    }
}

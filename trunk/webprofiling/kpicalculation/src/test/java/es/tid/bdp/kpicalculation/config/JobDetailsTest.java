package es.tid.bdp.kpicalculation.config;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;


/**
 *
 * @author dmicol, sortega
 */
public class JobDetailsTest {
    private KpiFeature instance;

    @Before
    public void setUp() {
        this.instance = new KpiFeature("a", new String[] { "b" });
    }

    @Test
    public void testGettersAndSetters() throws Exception {
        assertEquals("a", this.instance.getName());
        assertArrayEquals(new String[] { "b" }, this.instance.getFields());
        assertNull(this.instance.getGroup());
    }

    @Test
    public void testEquality() throws Exception {
        assertFalse(this.instance.equals(null));
        assertFalse(this.instance.equals(new Object()));
        assertFalse(this.instance.equals(new KpiFeature("different",
                new String[] { "b" })));
        assertFalse(this.instance.equals(new KpiFeature("a", new String[] {
            "different" })));
        assertFalse(this.instance.equals(new KpiFeature("a", new String[] {
            "b" }, "different")));
        assertEquals(this.instance, new KpiFeature("a", new String[] { "b" }));
    }
}

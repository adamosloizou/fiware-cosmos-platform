package es.tid.cosmos.base.util;

import java.security.NoSuchAlgorithmException;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class SHAEncoderTest {
    @Test
    public void testEncode() throws NoSuchAlgorithmException {
        assertEquals("3fd75f795d13da104dd29ae84bd6116e050feff4e762dd07567613722"
                + "45fa7d4acdef8a8c0d2666d81d3e2da52dc4ba37656631bf136d80e3f926"
                + "35580482b2b", SHAEncoder.encode("432asdfAC"));
    }
}

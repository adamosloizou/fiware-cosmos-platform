package es.tid.analytics.mobility.core.test;

import static org.junit.Assert.assertEquals;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import es.tid.analytics.mobility.core.data.GeoLocation;
import es.tid.analytics.mobility.core.data.GeoLocationContainer;

public class GeoLocationContainerTest {

	@Before
	public void setUp() throws Exception {
	}

	@After
	public void tearDown() throws Exception {
	}

	@Test
	public void testGeoLocationContainer() {
		GeoLocation geoLocation = new GeoLocation(666, (byte) 34, (byte) 5);
		GeoLocationContainer geoLocationsActual = new GeoLocationContainer();
		GeoLocationContainer geoLocationsExpected = new GeoLocationContainer();
//		Map<GeoLocation, Integer> geoLocationsExpected = new TreeMap<GeoLocation, Integer>();

		geoLocationsExpected.putGeoLocation(geoLocation, Integer.valueOf(1));
		geoLocationsActual.incrementGeoLocation(geoLocation);
		
		assertEquals(geoLocationsExpected, geoLocationsActual);
	}

	@Test
	public void testGeoLocationContainerNull() {
		GeoLocation geoLocation = null;
		GeoLocationContainer geoLocationsActual = new GeoLocationContainer();
		GeoLocationContainer geoLocationsExpected = new GeoLocationContainer();
//		Map<GeoLocation, Integer> geoLocationsExpected = new TreeMap<GeoLocation, Integer>();

		try {

			geoLocationsActual.incrementGeoLocation(geoLocation);
			Assert.fail("Se debe lanzar una excepcion");
		} catch (Exception e) {
			geoLocation = new GeoLocation(666, (byte) 34, (byte) 5);
			geoLocationsActual.incrementGeoLocation(geoLocation);
			geoLocationsExpected.putGeoLocation(geoLocation, Integer.valueOf(1));
			assertEquals(geoLocationsExpected,
					geoLocationsActual);

		}

	}

}

package es.tid.bdp.profile.userprofile;

import static java.util.Arrays.asList;
import java.util.Collections;

import static org.hamcrest.Matchers.*;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;
import org.junit.Before;
import org.junit.Test;

import static es.tid.bdp.profile.data.CategoryCountUtil.create;

/**
 * Test case for CategoryCountAggregator
 *
 * @author sortega
 */
public class CategoryCountAggregatorTest {
    private CategoryCountAggregator instance;

    @Before
    public void setUp() {
        this.instance = new CategoryCountAggregator();
    }

    @Test
    public void shouldAccumValues() {
        this.instance.add(create("name", 1L));
        this.instance.add(create("name", 2L));
        assertEquals(asList(create("name", 3L)), this.instance.getSortedCounts());
    }

    @Test
    public void shouldForgetValues() {
        this.instance.add(create("name", 1L));
        assertThat(this.instance.getSortedCounts(),is(not(Collections.EMPTY_LIST)));
        this.instance.clear();
        assertThat(this.instance.getSortedCounts(), is(Collections.EMPTY_LIST));
    }

    @Test
    public void shouldSortByCount() {
        this.instance.add(create("more", 1L));
        this.instance.add(create("less", 1L));
        this.instance.add(create("more", 2L));
        assertEquals(asList(create("more", 3L), create("less", 1L)),
                this.instance.getSortedCounts());
    }
}

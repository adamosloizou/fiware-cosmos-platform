/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;

/**
 * Test case for CategoryCountUtil
 *
 * @author dmicol
 */
public class CategoryCountUtilTest {
    @Test
    public void shouldBeUtilityClass() {
        assertUtilityClass(CategoryCountUtil.class);
    }

    @Test
    public void testCreate() {
        final String name = "Sports";
        final long count = 5L;

        ProtobufWritable<CategoryCount> catCountWrapper = CategoryCountUtil.
                createAndWrap(name, count);
        CategoryCount catCount = catCountWrapper.get();
        assertEquals(name, catCount.getName());
        assertEquals(count, catCount.getCount());
    }
}
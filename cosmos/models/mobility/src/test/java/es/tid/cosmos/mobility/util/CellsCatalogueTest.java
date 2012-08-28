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

package es.tid.cosmos.mobility.util;

import java.io.IOException;
import java.io.StringReader;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class CellsCatalogueTest {

    @Test
    public void testLoadAndfilter() throws IOException {
        String text = "334305|2221436242|12|34|56.1|78.2\n"
                + "334306|2221436242|12|34|56.3|78.4\n"
                + "334307|2221436242|12|34|56|78.6\n"
                + "334305|2221436242|12|34|56.7|78";
        List<Cell> cells = CellsCatalogue.load(new StringReader(text), "|");
        assertNotNull(cells);
        assertEquals(4, cells.size());
        assertEquals(3359493L, cells.get(0).getCellId());
        assertEquals(3359494L, cells.get(1).getCellId());
        assertEquals(3359495L, cells.get(2).getCellId());
        assertEquals(3359493L, cells.get(3).getCellId());

        List<Cell> filteredCells = CellsCatalogue.filter(cells, 3359493L);
        assertEquals(2, filteredCells.size());
        for (Cell cell : filteredCells) {
            assertEquals(3359493L, cell.getCellId());
        }
    }
}

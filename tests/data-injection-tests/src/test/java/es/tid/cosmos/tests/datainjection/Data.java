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

package es.tid.cosmos.tests.datainjection;

import java.util.Iterator;

/**
 *
 * @author ximo
 */
class Data implements Iterable<Integer> {
    private final int step;

    public Data(int step) {
        this.step = step;
    }

    @Override
    public Iterator<Integer> iterator() {
        return new Iterator<Integer>() {
            private int item = Byte.MIN_VALUE;

            @Override
            public boolean hasNext() {
                return (this.item + Data.this.step) > Byte.MAX_VALUE;
            }

            @Override
            public Integer next() {
                this.item += Data.this.step;
                return item;
            }

            @Override
            public void remove() {
                throw new UnsupportedOperationException(
                        "This iterable does not support removal");
            }
        };
    }
}

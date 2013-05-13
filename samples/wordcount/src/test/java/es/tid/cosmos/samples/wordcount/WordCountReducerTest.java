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

package es.tid.cosmos.samples.wordcount;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import static java.util.Arrays.asList;

/**
 * Test case for WordCount sample model reducer
 *
 * @author logc
 */
public class WordCountReducerTest {

    private ReduceDriver<Text, IntWritable, Text, LongWritable> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<Text, IntWritable, Text, LongWritable>(
                new WordCountReducer());
    }

    @Test
    public void shouldCountUniqueWord() {
        this.driver
            .withInput(new Text("this"), asList(new IntWritable(1)))
            .withOutput(new Text("this"), new LongWritable(1L))
            .runTest();
    }

    @Test
    public void shouldCountRepeatedWords() {
        this.driver
            .withInput(
                    new Text("a"),
                    asList(new IntWritable(1), new IntWritable(1)))
            .withOutput(new Text("a"), new LongWritable(2L))
            .runTest();
    }
}

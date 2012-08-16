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
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.junit.Before;
import org.junit.Test;

/**
 * Test case for WordCount sample model
 *
 * @author logc
 */
public class WordCountTest {
    private MapReduceDriver<LongWritable, Text, Text, IntWritable, Text,
            LongWritable> driver;

    @Before
    public void setUp() {
        this.driver = new MapReduceDriver<LongWritable, Text, Text,
                IntWritable, Text, LongWritable>(new WordCountMapper(),
                                                 new WordCountReducer());
    }

    @Test
    public void shouldCountWords() {
        this.driver
            .withInput(new LongWritable(1L),
                       new Text("a string of text where a word is repeated"))
            .withOutput(new Text("a"), new LongWritable(2))
            .withOutput(new Text("is"), new LongWritable(1))
            .withOutput(new Text("of"), new LongWritable(1))
            .withOutput(new Text("repeated"), new LongWritable(1))
            .withOutput(new Text("string"), new LongWritable(1))
            .withOutput(new Text("text"), new LongWritable(1))
            .withOutput(new Text("where"), new LongWritable(1))
            .withOutput(new Text("word"), new LongWritable(1))
            .runTest();
    }
}

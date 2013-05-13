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

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

/**
 * Application entry point. Runs a job that counts each different word
 * in a given text.
 *
 * @author logc
 */
public class WordCountMain extends Configured implements Tool {

    private static final Logger LOGGER = Logger.getLogger(WordCountMain.class);

    @Override
    public int run(String[] args) throws Exception {
        if (args.length != 2) {
            throw new IllegalArgumentException(
                    "Usage: text_path output_path");
        }

        final Path inputPath = new Path(args[0]);
        final Path outputPath = new Path(args[1]);

        WordCountJob wcJob = new WordCountJob(this.getConf());
        wcJob.configure(inputPath, outputPath);
        if (!wcJob.waitForCompletion(true)) {
            throw new Exception("Failed to process word counts");
        }
        return 0;
    }

    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(
                    new Configuration(), new WordCountMain(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            LOGGER.fatal(ex);
            throw ex;
        }
    }
}

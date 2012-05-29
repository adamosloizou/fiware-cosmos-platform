package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.cosmos.mobility.parsing.ParserClientProfilesReducer;
import es.tid.cosmos.mobility.populationdensity.PopdenCreateVectorReducer;
import es.tid.cosmos.mobility.populationdensity.PopdenProfileGetOutReducer;
import es.tid.cosmos.mobility.populationdensity.PopdenSumCommsReducer;

/**
 *
 * @author dmicol
 */
public final class PopulationDensityProfileRunner {
    private PopulationDensityProfileRunner() {
    }
    
    public static void run(Path clientProfilePath, Path clientsInfoPath,
                           Path populationDensityProfileOut, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        Path clientProfileParsedPath = new Path(tmpDirPath,
                                                "client_profile_parsed");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ParserClientProfiles",
                    TextInputFormat.class,
                    ParserClientProfilesReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsInfoPath);
            FileOutputFormat.setOutputPath(job, clientProfileParsedPath);
            job.waitForCompletion(true);
        }

        Path popdenprofBtsprofPath = new Path(tmpDirPath, "popdenprof_btsprof");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenJoinArrayProfile",
                    SequenceFileInputFormat.class,
                    PopdenJoinArrayProfileReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] { clientsInfoPath,
                clientProfileParsedPath });
            FileOutputFormat.setOutputPath(job, popdenprofBtsprofPath);
            job.waitForCompletion(true);
        }

        Path popdenBtsprofCountPath = new Path(tmpDirPath,
                                               "popden_btsprof_count");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenSumComms",
                    SequenceFileInputFormat.class,
                    PopdenSumCommsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, popdenprofBtsprofPath);
            FileOutputFormat.setOutputPath(job, popdenBtsprofCountPath);
            job.waitForCompletion(true);
        }
        
        Path populationDensityProfilePath = new Path(tmpDirPath,
                "population_density_profile");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenCreateVector",
                    SequenceFileInputFormat.class,
                    PopdenCreateVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, popdenBtsprofCountPath);
            FileOutputFormat.setOutputPath(job, populationDensityProfilePath);
            job.waitForCompletion(true);
        }
        
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenProfileGetOut",
                    SequenceFileInputFormat.class,
                    PopdenProfileGetOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, populationDensityProfilePath);
            FileOutputFormat.setOutputPath(job, populationDensityProfileOut);
            job.waitForCompletion(true);
        }
        
        if (!isDebug) {
            FileSystem fs = FileSystem.get(conf);
            fs.delete(popdenprofBtsprofPath, true);
            fs.delete(popdenBtsprofCountPath, true);
        }
    }
}

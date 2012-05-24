package es.tid.cosmos.mobility.labelling.clientbts;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.ReduceJob;
import es.tid.cosmos.mobility.labelling.client.VectorCreateNodeDayhourReducer;
import es.tid.cosmos.mobility.labelling.client.VectorFuseNodeDaygroupReducer;
import es.tid.cosmos.mobility.labelling.client.VectorNormalizedReducer;
import es.tid.cosmos.mobility.util.*;

/**
 *
 * @author dmicol
 */
public final class ClientBtsLabellingRunner {
    private ClientBtsLabellingRunner() {
    }
    
    public static void run(Path clientsInfoPath, Path clientsRepbtsPath,
                           Path vectorClientbtsPath, Path centroidsPath,
                           Path pointsOfInterestTempPath,
                           Path vectorClientbtsClusterPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path clientsbtsSpreadPath = new Path(tmpDirPath, "clientsbts_spread");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorSpreadNodbts",
                    SequenceFileInputFormat.class,
                    VectorSpreadNodbtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsInfoPath);
            FileOutputFormat.setOutputPath(job, clientsbtsSpreadPath);
            job.waitForCompletion(true);
        }

        Path clientsbtsSumPath = new Path(tmpDirPath, "clientsbts_sum");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorSumGroupcomms",
                    SequenceFileInputFormat.class,
                    VectorSumGroupcommsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsbtsSpreadPath);
            FileOutputFormat.setOutputPath(job, clientsbtsSumPath);
            job.waitForCompletion(true);
        }

        Path clientsbtsSumWithInputIdPath = new Path(tmpDirPath,
                "clientsbts_sum_with_input_id");
        {
            ReduceJob job = ReduceJob.create(conf, "SetMobDataInputIdByTwoInt",
                    SequenceFileInputFormat.class,
                    SetMobDataInputIdByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setInt("input_id", 0);
            FileInputFormat.setInputPaths(job, clientsbtsSumPath);
            FileOutputFormat.setOutputPath(job, clientsbtsSumWithInputIdPath);
            job.waitForCompletion(true);
        }

        Path clientsRepbtsWithInputIdPath = new Path(tmpDirPath,
                "clients_repbts_with_input_id");
        {
            ReduceJob job = ReduceJob.create(conf, "SetMobDataInputIdByTwoInt",
                    SequenceFileInputFormat.class,
                    SetMobDataInputIdByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setInt("input_id", 1);
            FileInputFormat.setInputPaths(job, clientsRepbtsPath);
            FileOutputFormat.setOutputPath(job, clientsRepbtsWithInputIdPath);
            job.waitForCompletion(true);
        }
        
        Path clientsbtsRepbtsPath = new Path(tmpDirPath, "clientsbts_repbts");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorFiltClientbts",
                    SequenceFileInputFormat.class,
                    VectorFiltClientbtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                clientsbtsSumWithInputIdPath, clientsRepbtsWithInputIdPath });
            FileOutputFormat.setOutputPath(job, clientsbtsRepbtsPath);
            job.waitForCompletion(true);
        }
        
        fs.delete(clientsbtsSumWithInputIdPath, true);
        fs.delete(clientsRepbtsWithInputIdPath, true);
        
        Path clientsbtsGroupPath = new Path(tmpDirPath, "clientsbts_group");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorCreateNodeDayhour",
                    SequenceFileInputFormat.class,
                    VectorCreateNodeDayhourReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsbtsRepbtsPath);
            FileOutputFormat.setOutputPath(job, clientsbtsGroupPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "VectorFuseNodeDaygroup",
                    SequenceFileInputFormat.class,
                    VectorFuseNodeDaygroupReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsbtsGroupPath);
            FileOutputFormat.setOutputPath(job, vectorClientbtsPath);
            job.waitForCompletion(true);
        }

        Path vectorClientbtsNormPath = new Path(tmpDirPath,
                                                "vector_clientbts_norm");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorNormalized",
                    SequenceFileInputFormat.class,
                    VectorNormalizedReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorClientbtsPath);
            FileOutputFormat.setOutputPath(job, vectorClientbtsNormPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf,
                    "ClusterClientBtsGetMinDistanceToPoi",
                    SequenceFileInputFormat.class,
                    ClusterClientBtsGetMinDistanceToPoiReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("centroids", centroidsPath.toString());
            FileInputFormat.setInputPaths(job, vectorClientbtsNormPath);
            FileOutputFormat.setOutputPath(job, pointsOfInterestTempPath);
            job.waitForCompletion(true);
        }
        
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ClusterClientBtsGetMinDistanceToCluster",
                    SequenceFileInputFormat.class,
                    ClusterClientBtsGetMinDistanceToClusterReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("centroids", centroidsPath.toString());
            FileInputFormat.setInputPaths(job, vectorClientbtsNormPath);
            FileOutputFormat.setOutputPath(job, vectorClientbtsClusterPath);
            job.waitForCompletion(true);
        }

        if (isDebug) {
            Path vectorClientbtsClusterTextPath = new Path(tmpDirPath,
                    "vector_clientbts_cluster_text");
            {
                ReduceJob job = ReduceJob.create(conf,
                        "ExportClusterToTextByTwoInt",
                        SequenceFileInputFormat.class,
                        ExportClusterToTextByTwoIntReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, vectorClientbtsClusterPath);
                FileOutputFormat.setOutputPath(job,
                                               vectorClientbtsClusterTextPath);
                job.waitForCompletion(true);
            }
        } else {
            fs.delete(clientsbtsSpreadPath, true);
            fs.delete(clientsbtsSumPath, true);
            fs.delete(clientsbtsRepbtsPath, true);
            fs.delete(clientsbtsGroupPath, true);
            fs.delete(vectorClientbtsNormPath, true);
        }
    }
}
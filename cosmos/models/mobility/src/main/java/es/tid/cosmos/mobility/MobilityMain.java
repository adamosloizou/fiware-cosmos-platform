package es.tid.cosmos.mobility;

import es.tid.cosmos.mobility.pois.NodeBtsCounterJob;
import es.tid.cosmos.mobility.pois.NodeMobInfoJob;
import es.tid.cosmos.mobility.pois.RepbtsAggbybtsJob;
import es.tid.cosmos.mobility.pois.RepbtsSpreadNodebtsJob;
import es.tid.cosmos.mobility.pois.RepbtsJoinDistCommsJob;
import es.tid.cosmos.mobility.pois.RepbtsGetRepresentativeBtsJob;
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.cosmos.mobility.jobs.*;

/**
 *
 * @author dmicol
 */
public class MobilityMain extends Configured implements Tool {
    private static final Logger LOG = Logger.getLogger(MobilityMain.class);
    
    // TODO: don't use hard-coded paths. Use properties file instead
    private static final String WORKING_DIRECTORY = "/home/hdfs/mobility";
    private static final String CDRS_MOB_PATH =
            WORKING_DIRECTORY + "/cdrs_mob";
    private static final String CELLS_MOB_PATH =
            WORKING_DIRECTORY + "/cells_mob";
    
    @Override
    public int run(String[] args) throws Exception {
        if (args.length != 4) {
            throw new IllegalArgumentException("Mandatory parameters: "
                                               + "cdrs_path cells_path");
        }

        Path cdrsPath = new Path(args[0]);
        Path cellsPath = new Path(args[1]);
        Path btsPath = new Path(args[2]);
        Path poisPath = new Path(args[3]);
        Path tmpPath = new Path("/tmp");
        
        this.runParsingJobs(cdrsPath, cellsPath);
        this.extractPointsOfInterest(btsPath, poisPath, tmpPath);
        
        return 0;
    }

    private void runParsingJobs(Path cdrsPath, Path cellsPath)
            throws Exception {
        ParseCdrsJob parseCdrsJob = new ParseCdrsJob(this.getConf());
        parseCdrsJob.configure(cdrsPath, new Path(CDRS_MOB_PATH));
        if (!parseCdrsJob.waitForCompletion(true)) {
            throw new Exception("Failed to parse CDRs");
        }
        
        ParseCellsJob parseCellsJob = new ParseCellsJob(this.getConf());
        parseCellsJob.configure(cellsPath, new Path(CELLS_MOB_PATH));
        if (!parseCellsJob.waitForCompletion(true)) {
            throw new Exception("Failed to parse cells");
        }
    }
    
    private void extractPointsOfInterest(Path input, Path output, Path tmpDir)
            throws Exception {
        Path nodeBtsCounter = tmpDir.suffix("node_bts_counter");
        {
            NodeBtsCounterJob job = new NodeBtsCounterJob(this.getConf());
            job.configure(input, nodeBtsCounter);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run NodeBtsCounterJob");
            }
        }
        
        Path nodeMobInfo = tmpDir.suffix("node_mob_info");
        {
            NodeMobInfoJob job = new NodeMobInfoJob(this.getConf());
            job.configure(nodeBtsCounter, nodeMobInfo);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run NodeMobInfoJob");
            }
        }
        
        Path repbtsSpreadNodebts = tmpDir.suffix("repbts_spread_nodebts");
        {
            RepbtsSpreadNodebtsJob job = new RepbtsSpreadNodebtsJob(
                    this.getConf());
            job.configure(nodeMobInfo, repbtsSpreadNodebts);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run RepbtsSpreadNodebtsJob");
            }
        }
        
        Path repbtsAggbybts = tmpDir.suffix("repbts_aggbybts");
        {
            RepbtsAggbybtsJob job = new RepbtsAggbybtsJob(this.getConf());
            job.configure(repbtsSpreadNodebts, repbtsAggbybts);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run RepbtsAggbybtsJob");
            }
        }      

        Path repbtsJoinDistComms = tmpDir.suffix("repbts_join_dist_comms");
        {
            RepbtsJoinDistCommsJob job = new RepbtsJoinDistCommsJob(
                    this.getConf());
            job.configure(repbtsAggbybts, repbtsJoinDistComms);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run RepbtsJoinDistCommsJob");
            }
        }

        {
            RepbtsGetRepresentativeBtsJob job =
                    new RepbtsGetRepresentativeBtsJob(this.getConf());
            job.configure(repbtsJoinDistComms, output);
            if (!job.waitForCompletion(true)) {
                throw new Exception(
                        "Failed to run RepbtsGetRepresentativeBtsJob");
            }
        }
    }
    
    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new MobilityMain(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            LOG.fatal(ex);
            throw ex;
        }
    }
}

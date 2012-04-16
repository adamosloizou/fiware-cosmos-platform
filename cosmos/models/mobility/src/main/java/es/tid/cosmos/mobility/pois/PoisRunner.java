package es.tid.cosmos.mobility.pois;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

/**
 *
 * @author dmicol
 */
public final class PoisRunner {
    private PoisRunner() {
    }
    
    public static void run(Path tmpDir, Path clientsBtsPath,
                           Path clientsRepbtsPath, Configuration conf)
            throws Exception {
        Path nodeBtsCounter = tmpDir.suffix("/node_bts_counter");
        {
            NodeBtsCounterJob job = new NodeBtsCounterJob(conf);
            job.configure(clientsBtsPath, nodeBtsCounter);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path nodeMobInfo = tmpDir.suffix("/node_mob_info");
        {
            NodeMobInfoJob job = new NodeMobInfoJob(conf);
            job.configure(nodeBtsCounter, nodeMobInfo);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path repbtsSpreadNodebts = tmpDir.suffix("/repbts_spread_nodebts");
        {
            RepbtsSpreadNodebtsJob job = new RepbtsSpreadNodebtsJob(conf);
            job.configure(nodeMobInfo, repbtsSpreadNodebts);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path repbtsAggbybts = tmpDir.suffix("/repbts_aggbybts");
        {
            RepbtsAggbybtsJob job = new RepbtsAggbybtsJob(conf);
            job.configure(repbtsSpreadNodebts, repbtsAggbybts);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path repbtsJoinDistComms = tmpDir.suffix("/repbts_join_dist_comms");
        {
            RepbtsJoinDistCommsJob job = new RepbtsJoinDistCommsJob(conf);
            job.configure(repbtsAggbybts, repbtsJoinDistComms);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            RepbtsGetRepresentativeBtsJob job =
                    new RepbtsGetRepresentativeBtsJob(conf);
            job.configure(repbtsJoinDistComms, clientsRepbtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}

package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.smartsteps.footfalls.microgrids.LookupRekeyerMapper.RekeyBy;
import es.tid.smartsteps.footfalls.microgrids.config.Config;
import es.tid.smartsteps.footfalls.microgrids.parsing.CatchmentsParserMapper;
import es.tid.smartsteps.footfalls.microgrids.parsing.CentroidParserMapper;
import es.tid.smartsteps.footfalls.microgrids.parsing.LookupParserMapper;
import es.tid.smartsteps.footfalls.microgrids.parsing.TrafficCountsParserMapper;

/**
 *
 * @author dmicol
 */
public class Main extends Configured implements Tool {

    private static final Logger LOGGER = Logger.getLogger(Main.class);

    @Override
    public int run(String[] args) throws ClassNotFoundException,
                                         InterruptedException, IOException {
        if (args.length != 6) {
            throw new IllegalArgumentException(
                    "Usage: trafficCountsPath cellToMicrogridPath "
                    + "microgridToPolygonPath soaCentroidsPath catchmentsPath "
                    + "outputDir");
        }

        final Configuration config = Config.load(Config.class.getResource(
                "/config.properties").openStream(), this.getConf());

        final Path trafficCountsPath = new Path(args[0]);
        final Path cellToMicrogridPath = new Path(args[1]);
        final Path microgridToPolygonPath = new Path(args[2]);
        final Path soaCentroidsPath = new Path(args[3]);
        final Path catchmentsPath = new Path(args[4]);
        final Path outputDir = new Path(args[5]);

        FileSystem fs = FileSystem.get(this.getConf());

        Path trafficCountsParsedPath = new Path(outputDir,
                                                "traffic_counts_parsed");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "TrafficCountsParser",
                    TextInputFormat.class,
                    TrafficCountsParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, trafficCountsPath);
            FileOutputFormat.setOutputPath(job, trafficCountsParsedPath);
            job.waitForCompletion(true);
        }

        Path cellToMicrogridParsedPath = new Path(outputDir,
                                                  "cell_to_microgrid_parsed");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "CellToMicrogridLookupParser",
                    TextInputFormat.class,
                    LookupParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellToMicrogridPath);
            FileOutputFormat.setOutputPath(job, cellToMicrogridParsedPath);
            job.waitForCompletion(true);
        }

        Path countsByMicrogridPath = new Path(outputDir, "counts_by_microgrid");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "CellToMicrogridEntryScaler",
                    SequenceFileInputFormat.class,
                    TrafficCountsScalerMapper.class,
                    TrafficCountsScalerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, trafficCountsParsedPath,
                                          cellToMicrogridParsedPath);
            FileOutputFormat.setOutputPath(job, countsByMicrogridPath);
            job.waitForCompletion(true);
        }

        fs.delete(trafficCountsParsedPath, true);

        Path microgridToPolygonParsedPath = new Path(outputDir,
                "microgrid_to_polygon_parsed");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "MicrogridToPolygonLookupParser",
                    TextInputFormat.class,
                    LookupParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, microgridToPolygonPath);
            FileOutputFormat.setOutputPath(job, microgridToPolygonParsedPath);
            job.waitForCompletion(true);
        }

        Path countsByPolygonPath = new Path(outputDir, "counts_by_polygon");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "MicrogridToPolygonEntryScaler",
                    SequenceFileInputFormat.class,
                    TrafficCountsScalerMapper.class,
                    TrafficCountsScalerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, countsByMicrogridPath,
                                          microgridToPolygonParsedPath);
            FileOutputFormat.setOutputPath(job, countsByPolygonPath);
            job.waitForCompletion(true);
        }

        fs.delete(countsByMicrogridPath, true);

        Path aggregatedCountsByPolygonPath = new Path(outputDir,
                "aggregated_counts_by_polygon");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "AggregationByCellIdAndDate",
                    SequenceFileInputFormat.class,
                    CellIdAndDateMapper.class,
                    AggregationReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, countsByPolygonPath);
            FileOutputFormat.setOutputPath(job, aggregatedCountsByPolygonPath);
            job.waitForCompletion(true);
        }

        fs.delete(countsByPolygonPath, true);

        Path soaCentroidsParsedPath = new Path(outputDir, "soa_centroids_parsed");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "SOACentroidParser",
                    TextInputFormat.class,
                    CentroidParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, soaCentroidsPath);
            FileOutputFormat.setOutputPath(job, soaCentroidsParsedPath);
            job.waitForCompletion(true);
        }

        Path aggregatedCountsByPolygonJoinedPath = new Path(outputDir,
                "soa_centroids_parsed_joined");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "SOACentroidJoiner",
                    SequenceFileInputFormat.class,
                    CentroidJoinerMapper.class,
                    CentroidJoinerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, aggregatedCountsByPolygonPath,
                                          soaCentroidsParsedPath);
            FileOutputFormat.setOutputPath(job,
                                           aggregatedCountsByPolygonJoinedPath);
            job.waitForCompletion(true);
        }

        fs.delete(aggregatedCountsByPolygonPath, true);
        fs.delete(soaCentroidsParsedPath, true);

        Path aggregatedCountsByPolygonJoinedTextPath = new Path(outputDir,
                "aggregated_counts_by_polygon_joined_text");
        {
            CosmosJob job = CosmosJob.createReduceJob(config,
                    "TrafficCountsJsonExporter",
                    SequenceFileInputFormat.class,
                    TrafficCountsJsonExporterReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job,
                                          aggregatedCountsByPolygonJoinedPath);
            FileOutputFormat.setOutputPath(job,
                    aggregatedCountsByPolygonJoinedTextPath);
            job.waitForCompletion(true);
        }

        fs.delete(aggregatedCountsByPolygonJoinedPath, true);

        Path catchmentsParsedPath = new Path(outputDir, "catchments_parsed");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "CatchmentsParser",
                    TextInputFormat.class,
                    CatchmentsParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, catchmentsPath);
            FileOutputFormat.setOutputPath(job, catchmentsParsedPath);
            job.waitForCompletion(true);
        }

        Path cellToMicrogridParsedRekeyedByValuePath = new Path(outputDir,
                "cell_to_microgrid_parsed_rekeyed_by_value");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "LookupRekeyer",
                    SequenceFileInputFormat.class,
                    LookupRekeyerMapper.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setEnum(RekeyBy.class.getName(), RekeyBy.VALUE);
            FileInputFormat.setInputPaths(job, cellToMicrogridParsedPath);
            FileOutputFormat.setOutputPath(job,
                    cellToMicrogridParsedRekeyedByValuePath);
            job.waitForCompletion(true);
        }

        Path cellToPolygonParsedPath = new Path(outputDir,
                                                "cell_to_polygon_parsed");
        {
            CosmosJob job = CosmosJob.createReduceJob(config,
                    "TransitiveLookup",
                    SequenceFileInputFormat.class,
                    TransitiveLookupReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job,
                    cellToMicrogridParsedRekeyedByValuePath,
                    microgridToPolygonParsedPath);
            FileOutputFormat.setOutputPath(job, cellToPolygonParsedPath);
            job.waitForCompletion(true);
        }

        fs.delete(cellToMicrogridParsedRekeyedByValuePath, true);
        fs.delete(microgridToPolygonParsedPath, true);

        Path catchmentsByMicrogridPath = new Path(outputDir,
                                                  "catchments_by_microgrid");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "CellToMicrogridCatchmentsScaler",
                    SequenceFileInputFormat.class,
                    CatchmentsScalerMapper.class,
                    CatchmentsScalerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, catchmentsParsedPath,
                                          cellToPolygonParsedPath);
            FileOutputFormat.setOutputPath(job, catchmentsByMicrogridPath);
            job.waitForCompletion(true);
        }

        fs.delete(catchmentsParsedPath, true);

        Path scaledTopCellsPath = new Path(outputDir, "scaled_topcells");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "ScaleTopCells",
                    SequenceFileInputFormat.class,
                    TopCellScalerMapper.class,
                    TopCellScalerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, catchmentsByMicrogridPath,
                                          cellToPolygonParsedPath);
            FileOutputFormat.setOutputPath(job, scaledTopCellsPath);
        }

        fs.delete(catchmentsByMicrogridPath, true);
        fs.delete(cellToPolygonParsedPath, true);

        return 0;
    }

    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(), new Main(), args);
            if (res != 0) {
                throw new Exception("Uknown error");
            }
        } catch (Exception ex) {
            LOGGER.fatal(ex);
            throw ex;
        }
    }
}

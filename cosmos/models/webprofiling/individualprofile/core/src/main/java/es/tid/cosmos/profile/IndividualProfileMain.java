package es.tid.cosmos.profile;

import java.io.IOException;
import java.util.Calendar;

import com.hadoop.mapreduce.LzoTextInputFormat;
import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.util.MongoConfigUtil;
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

import es.tid.cosmos.base.mapreduce.MapReduceJob;
import es.tid.cosmos.base.mapreduce.ReduceJob;
import es.tid.cosmos.profile.categoryextraction.CategoryExtractionReducer;
import es.tid.cosmos.profile.categoryextraction.TextCategoryExtractionMapper;
import es.tid.cosmos.profile.export.mongodb.MongoDBExporterReducer;
import es.tid.cosmos.profile.export.text.TextExporterReducer;
import es.tid.cosmos.profile.userprofile.UserProfileMapper;
import es.tid.cosmos.profile.userprofile.UserProfileReducer;

/**
 * Application entry point. Configures and chains mapreduce jobs.
 *
 * @author dmicol, sortega
 */
public class IndividualProfileMain extends Configured implements Tool {
    private static final Logger LOG = Logger.getLogger(
            IndividualProfileMain.class);
    private static final String TMP_DIR = "tmp";
    private static final String CATEGORIES_DIR = "categories";
    private static final String PROFILE_DIR = "profile";

    private Path categoriesPath;
    private Path profilePath;

    @Override
    public int run(String[] args) throws Exception {
        if (args.length != 3) {
            throw new IllegalArgumentException("Mandatory parameters: "
                    + "weblogs_path textoutput_path mongo_url");
        }

        this.initTempPaths();
        final Path webLogsPath = new Path(args[0]);
        final Path textOutputPath = new Path(args[1]);
        final String mongoUrl = args[2];

        MapReduceJob ceJob = MapReduceJob.create(this.getConf(),
                "CategoryExtraction",
                LzoTextInputFormat.class,
                TextCategoryExtractionMapper.class,
                CategoryExtractionReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(ceJob, webLogsPath);
        FileOutputFormat.setOutputPath(ceJob, this.categoriesPath);

        MapReduceJob upJob = MapReduceJob.create(this.getConf(),
                "UserProfile",
                SequenceFileInputFormat.class,
                UserProfileMapper.class,
                UserProfileReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(upJob, this.categoriesPath);
        FileOutputFormat.setOutputPath(upJob, this.profilePath);
        upJob.addDependentJob(ceJob);

        ReduceJob exTextJob = ReduceJob.create(this.getConf(),
                "TextExporter",
                SequenceFileInputFormat.class,
                TextExporterReducer.class,
                TextOutputFormat.class);
        TextInputFormat.setInputPaths(exTextJob, this.profilePath);
        FileOutputFormat.setOutputPath(exTextJob, textOutputPath);
        exTextJob.addDependentJob(upJob);
        exTextJob.waitForCompletion(true);
        
        ReduceJob exMongoJob = ReduceJob.create(this.getConf(),
                "MongoDBExporter",
                SequenceFileInputFormat.class,
                MongoDBExporterReducer.class,
                MongoOutputFormat.class);
        TextInputFormat.setInputPaths(exMongoJob, this.profilePath);
        MongoConfigUtil.setOutputURI(exMongoJob.getConfiguration(), mongoUrl);
        exMongoJob.addDependentJob(upJob);
        exMongoJob.waitForCompletion(true);

        return 0;
    }

    private void initTempPaths() throws IOException {
        FileSystem fs = FileSystem.get(this.getConf());
        Path tmpDir = new Path(getTmpDir());
        if (!fs.mkdirs(tmpDir)) {
            throw new IOException("Failed to create temp directory");
        }
        LOG.info("Using " + tmpDir + " as temp directory");
        if (!fs.deleteOnExit(tmpDir)) {
            LOG.warn("Could not set temp directory for automatic deletion");
        }
        this.categoriesPath = new Path(tmpDir + Path.SEPARATOR +
                                       CATEGORIES_DIR);
        this.profilePath = new Path(tmpDir + Path.SEPARATOR + PROFILE_DIR);
    }

    private static String getTmpDir() {
        Calendar now = Calendar.getInstance();
        StringBuilder buffer = new StringBuilder();
        buffer.append(Path.SEPARATOR)
                .append(TMP_DIR)
                .append(Path.SEPARATOR)
                .append("individualprofile_")
                .append(now.get(Calendar.YEAR))
                .append(now.get(Calendar.MONTH))
                .append(now.get(Calendar.DATE))
                .append(now.get(Calendar.HOUR_OF_DAY))
                .append(now.get(Calendar.MINUTE))
                .append(now.get(Calendar.SECOND))
                .append(now.get(Calendar.MILLISECOND));
        return buffer.toString();
    }

    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new IndividualProfileMain(), args);
            if (res != 0) {
                throw new Exception("Uknown error");
            }
        } catch (Exception ex) {
            LOG.fatal(ex);
            throw ex;
        }
    }
}

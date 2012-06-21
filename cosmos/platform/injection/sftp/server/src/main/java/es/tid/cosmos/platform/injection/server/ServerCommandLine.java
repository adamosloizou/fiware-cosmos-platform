package es.tid.cosmos.platform.injection.server;

import org.apache.commons.cli.*;

/**
 * @author sortega
 */
public class ServerCommandLine {
    private static final String SHORT_CONFIG_FILE = "c";
    private static final String LONG_CONFIG_FILE = "config";

    private final GnuParser parser;
    private final Options options;
    private CommandLine commandLine;

    public ServerCommandLine() {
        this.parser = new GnuParser();
        this.options = new Options().addOption(SHORT_CONFIG_FILE,
                                               LONG_CONFIG_FILE,
                                               true, "Configuration file");
    }

    public void parse(String[] args) throws ParseException {
        this.commandLine = this.parser.parse(this.options, args.clone());
    }

    public void printUsage() {
        new HelpFormatter().printHelp("injection-server", this.options);
    }

    public boolean hasConfigFile() {
        return this.commandLine.hasOption(SHORT_CONFIG_FILE);
    }

    public String getConfigFile() {
        return this.commandLine.getOptionValue(SHORT_CONFIG_FILE);
    }
}

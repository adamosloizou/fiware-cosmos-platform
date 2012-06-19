package es.tid.smartsteps.ipm;

import java.io.*;
import java.nio.charset.Charset;

/**
 *
 * @author dmicol
 */
public final class IpmMain {
    private static final String INET_TYPE = "inet";
    private static final String CRM_TYPE = "crm";
    
    private IpmMain() {
    }
    
    public static void main(String[] args) throws Exception {
        if (args.length != 3) {
            throw new IllegalArgumentException("Invalid number of arguments.\n"
                    + "Usage: ipm.jar input output <inet|crm>");
        }
        
        final String input = args[0];
        final String output = args[1];
        final String type = args[2];
        RawToIpmConverter converter;
        if (type.equals(INET_TYPE)) {
             converter = new InetRawToIpmConverter();
        } else if (type.equals(CRM_TYPE)) {
            converter = new CrmRawToIpmConverter("|",
                    Charset.forName("UTF-8"));
        } else {
            throw new IllegalArgumentException("Invalid data type: " + type);
        }
        
        BufferedReader reader = null;
        BufferedWriter writer = null;
        try {
            reader = new BufferedReader(new FileReader(input));
            writer = new BufferedWriter(new FileWriter(output));
            String line;
            while ((line = reader.readLine()) != null) {
                writer.write(converter.convert(line));
            }
        } finally {
            if (writer != null) {
                writer.close();
            }
            if (reader != null) {
                reader.close();
            }
        }
    }
}

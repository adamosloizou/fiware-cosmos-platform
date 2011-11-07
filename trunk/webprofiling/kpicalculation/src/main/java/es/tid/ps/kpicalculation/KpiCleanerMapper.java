package es.tid.ps.kpicalculation;

import java.io.IOException;

import javax.annotation.Resource;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.kpicalculation.cleaning.KpiCalculationFilterChain;
import es.tid.ps.kpicalculation.data.PageView;

/**
 * This class receives lines of information of CDR´s files that will be used in
 * the Web Profiling module. The lines are parsed to a PageView object, whose
 * fullUrl field will be verified to asses that it passes all the filters
 * defined for the web profiling module in the KpiCalculationFilterChain
 * resource. If the url passes every filter then the PageView string value,
 * which fits the format of PAGE_VIEWS hive's table, will be emited. If the url
 * does not pass any of the filters nothing will be emited. Examples:
 * <ol>
 * <li>Input : {key: 1, values: 16737b1873ef03ad http://www.tid.es/index.html
 * 1Dec2010000001 304 application/pkix-crl -Microsoft-CryptoAPI/6.1 GET}</li>
 * <li>Output: {key: 1, values: 16737b1873ef03ad http http://tid.es/ tid.es /
 * null 1Dec2010000001 1Dec2010000001 -Microsoft-CryptoAPI/6.1
 * -Microsoft-CryptoAPI/6.1 -Microsoft-CryptoAPI/6.1 -Microsoft-CryptoAPI/6.1
 * GET 304"}</li>
 * </ol>
 * 
 * <ol>
 * <li>Input : {key: 1, values: 16737b1873ef03ad http http://www.tid.es/foto.jpg
 * 1Dec2010000001 304 application/pkix-crl -Microsoft-CryptoAPI/6.1 GET}</li>
 * <li>Output: void</li>
 * </ol>
 * 
 * 
 * @author javierb@tid.es
 */
public class KpiCleanerMapper extends Mapper<LongWritable, Text, LongWritable, Text> {

    @Resource
    private KpiCalculationFilterChain filter;

    /**
     * Method that prepares the filters to be applied
     * 
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void setup(Context context) throws IOException, InterruptedException {
        filter = new KpiCalculationFilterChain();
    }

    /**
     * @param key
     *            is the byte offset of the current line in the file;
     * @param value
     *            is the line from the file
     * @param context
     *            has the method "write()" to output the key,value pair
     */
    @Override
    public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
        PageView view = new PageView(value.toString());
        try {
            filter.filter(view.getFullUrl());
            context.write(key, new Text(view.toString()));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
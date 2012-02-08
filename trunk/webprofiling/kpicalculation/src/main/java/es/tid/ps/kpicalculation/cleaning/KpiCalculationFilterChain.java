package es.tid.ps.kpicalculation.cleaning;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;

import org.apache.hadoop.conf.Configuration;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;

/**
 * Abstract class to implement the "chain of responsibility" pattern. This
 * pattern is used apply different filters to the input data, to asses if it
 * will be useful for calculating kpi's for the personalisation process.
 * 
 * @author javierb
 * 
 */
public class KpiCalculationFilterChain {
    private static final String COLLECTION_ID = "kpifilters";

    private List<IKpiCalculationFilter> handlers;

    public KpiCalculationFilterChain(Configuration conf) {
        handlers = new ArrayList<IKpiCalculationFilter>();
        Collection<String> classes = conf.getStringCollection(COLLECTION_ID);
        Iterator<String> it = classes.iterator();
        while (it.hasNext()) {
            IKpiCalculationFilter filter;
            try {
                filter = (IKpiCalculationFilter) Class
                        .forName(it.next().trim())
                        .getConstructor(Configuration.class).newInstance(conf);
                handlers.add(filter);
            } catch (Exception exception) {
                throw new KpiCalculationFilterException(
                        "Wrong configuration of filter", exception,
                        KpiCalculationCounter.WRONG_FILTER_CONFIGURATION);
            }
        }
    }

    public void filter(String url) {
        IKpiCalculationFilter currentFilter;
        ListIterator<IKpiCalculationFilter> it = handlers.listIterator();
        while (it.hasNext()) {
            currentFilter = it.next();
            currentFilter.filter(url);
        }
    }
}

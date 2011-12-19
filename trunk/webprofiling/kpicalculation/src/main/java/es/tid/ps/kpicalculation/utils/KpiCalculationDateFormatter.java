package es.tid.ps.kpicalculation.utils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * Class used to format the date string received in log data allowing to get the
 * date and time in the wanted format
 * 
 * @author javierb
 */
public class KpiCalculationDateFormatter {
    private static SimpleDateFormat inputFormat;
    private static SimpleDateFormat dateFormat;
    private static SimpleDateFormat timeFormat;

    /**
     * Method that initializes the formatters
     */
    public static void init() {
        if (inputFormat != null && dateFormat != null && timeFormat != null) {
            // Aviod unnecessary re-initializations.
            return;
        }
        inputFormat = new SimpleDateFormat("ddMMMyyyyhhmmss", Locale.ENGLISH);
        dateFormat = new SimpleDateFormat("dd MM yyyy");
        timeFormat = new SimpleDateFormat("HH:mm:ss");
    }

    /**
     * Method that provides the formatted date string corresponding to the input
     * 
     * @param inputDate
     *            String date to format
     * @return the formatted date
     */
    public static String getDate(String inputDate) throws ParseException {
        Date date = inputFormat.parse(inputDate);
        return dateFormat.format(date);
    }

    /**
     * Method that provides the formatted time string corresponding to the input
     * 
     * @param inputDate
     *            String date to format
     * @return the formatted time
     */
    public static String getTime(String inputDate) throws ParseException {
        Date date = inputFormat.parse(inputDate);
        return timeFormat.format(date);
    }
}

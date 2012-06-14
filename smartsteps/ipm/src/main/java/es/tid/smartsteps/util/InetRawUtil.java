package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;

/**
 *
 * @author dmicol
 */
public abstract class InetRawUtil {
    private static final char DELIMITER = '|';
    
    private InetRawUtil() {
    }
    
    public static InetRaw create(String type, String callType,
            String imsi, String firstTempImsi, String lastTempImsi,
            String imei, String lacod, String cellId,
            String eventDateTime, String dtapCause, String bssmapCause,
            String ccCause, String mmCause, String ranapCause) {
        return InetRaw.newBuilder()
                .setType(type)
                .setCallType(callType)
                .setImsi(imsi)
                .setFirstTempImsi(firstTempImsi)
                .setLastTempImsi(lastTempImsi)
                .setImei(imei)
                .setLacod(lacod)
                .setCellId(cellId)
                .setEventDateTime(eventDateTime)
                .setDtapCause(dtapCause)
                .setBssmapCause(bssmapCause)
                .setCcCause(ccCause)
                .setMmCause(mmCause)
                .setRanapCause(ranapCause)
                .build();
    }
    
    public static InetRaw parse(String line) {
        String[] fields = line.split("\\" + DELIMITER);
        return create(fields[0], fields[1], fields[2], fields[3], fields[4],
                fields[5], fields[6], fields[7], fields[8], fields[9],
                fields[10], fields[11], fields[12], fields[13]);
    }
    
    public static String toString(InetRaw inetRaw) {
        return (inetRaw.getType() + DELIMITER
                + inetRaw.getCallType() + DELIMITER
                + inetRaw.getImsi() + DELIMITER
                + inetRaw.getFirstTempImsi() + DELIMITER
                + inetRaw.getLastTempImsi() + DELIMITER
                + inetRaw.getImei() + DELIMITER
                + inetRaw.getLacod() + DELIMITER
                + inetRaw.getCellId() + DELIMITER
                + inetRaw.getEventDateTime() + DELIMITER
                + inetRaw.getDtapCause() + DELIMITER
                + inetRaw.getBssmapCause() + DELIMITER
                + inetRaw.getCcCause() + DELIMITER
                + inetRaw.getMmCause() + DELIMITER
                + inetRaw.getRanapCause());
    }
}

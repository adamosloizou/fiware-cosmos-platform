package es.tid.bdp.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.profile.generated.data.ProfileProtocol.WebProfilingLog;

/**
 * Construction utils for WebProfilingLog class.
 *
 * @author dmicol
 */
public class WebProfilingLogUtil {
    protected WebProfilingLogUtil() {
    }

    public static WebProfilingLog create(String visitorId, String protocol,
                                         String fullUrl, String urlDomain,
                                         String urlPath, String urlQuery,
                                         String date, String userAgent,
                                         String browser, String device,
                                         String operSys, String method,
                                         String status, String mimeType) {
        return WebProfilingLog.newBuilder()
                .setVisitorId(visitorId)
                .setProtocol(protocol)
                .setFullUrl(fullUrl)
                .setUrlDomain(urlDomain)
                .setUrlPath(urlPath)
                .setUrlQuery(urlQuery)
                .setDate(date)
                .setUserAgent(userAgent)
                .setBrowser(browser)
                .setDevice(device)
                .setOperSys(operSys)
                .setMethod(method)
                .setStatus(status)
                .setMimeType(mimeType)
                .build();
    }

    public static ProtobufWritable createAndWrap(
            String visitorId, String protocol, String fullUrl, String urlDomain,
            String urlPath, String urlQuery, String date, String userAgent,
            String browser, String device, String operSys, String method,
            String status, String mimeType) {
        ProtobufWritable<WebProfilingLog> wrapper =
                ProtobufWritable.newInstance(WebProfilingLog.class);
        wrapper.set(create(visitorId, protocol, fullUrl, urlDomain, urlPath,
                           urlQuery, date, userAgent, browser, device, operSys,
                           method, status, mimeType));
        return wrapper;
    }
}

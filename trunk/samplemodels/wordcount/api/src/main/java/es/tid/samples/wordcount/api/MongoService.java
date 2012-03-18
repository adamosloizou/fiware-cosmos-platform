package es.tid.samples.wordcount.api;

import com.mongodb.DB;
import com.mongodb.Mongo;
import java.net.UnknownHostException;
import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

/**
 *
 * @author sortega
 */
@Component
public class MongoService {
    private Mongo mongo;
    private DB db;

    @Value("#{config.db_hostname}")
    private String hostname;
    @Value("#{config.db_name}")
    private String databaseName;

    @PostConstruct
    public void init() throws UnknownHostException {
        this.mongo = new Mongo(hostname);
        this.db = this.mongo.getDB(databaseName);
    }

    public void setDatabaseName(String databaseName) {
        this.databaseName = databaseName;
    }

    public void setHostname(String hostname) {
        this.hostname = hostname;
    }

    public DB getDb() {
        return db;
    }

    public Mongo getMongo() {
        return mongo;
    }

    @PreDestroy
    public void destroy() {
        this.mongo.close();
    }
}

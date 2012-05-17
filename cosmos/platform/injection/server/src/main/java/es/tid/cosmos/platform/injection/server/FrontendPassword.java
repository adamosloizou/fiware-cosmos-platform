package es.tid.cosmos.platform.injection.server;

import java.sql.*;
import java.util.StringTokenizer;

import org.apache.commons.codec.digest.DigestUtils;
import org.apache.sshd.server.PasswordAuthenticator;
import org.apache.sshd.server.session.ServerSession;

import es.tid.cosmos.base.util.Logger;

/**
 * FrontendPassword
 *
 * @author logc
 * @since  CTP 2
 */
public class FrontendPassword implements PasswordAuthenticator {
    private static final String djangoSeparator = "$";

    private String frontendDbUrl;
    private String dbName;
    private String dbUserName;
    private String dbPassword;
    private Connection connection;
    private final org.apache.log4j.Logger LOG = Logger.get(FrontendPassword.class);

    @Override
    public boolean authenticate(String username,
                                String password, ServerSession session) {
        LOG.debug(String.format("received %s as username, %d chars as password",
                username, password.length()));
        boolean ans = false;
        try {
            this.connect(this.frontendDbUrl, this.dbName, this.dbUserName,
                         this.dbPassword);
            String sql = "SELECT password FROM auth_user WHERE username = ?";
            PreparedStatement preparedStatement =
                    this.connection.prepareStatement(sql);
            preparedStatement.setString(1, username);
            ResultSet resultSet = preparedStatement.executeQuery();
            String algorithm = "";
            String hash = "";
            String salt = "";
            while (resultSet.next()) {
                StringTokenizer algorithmSaltHash = new StringTokenizer(
                        resultSet.getString(1), djangoSeparator);
                algorithm = algorithmSaltHash.nextToken();
                salt = algorithmSaltHash.nextToken();
                hash = algorithmSaltHash.nextToken();
            }
            if (algorithm.equals("sha1")) {
                ans = hash.equals(DigestUtils.shaHex(salt + password));
            } else if (algorithm.equals("md5")) {
                ans = hash.equals(DigestUtils.md5Hex(salt + password));
            }
        } catch (SQLException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
        return ans;
    }

    /**
     * Connect to the platform frontend database with the configured
     * credentials
     *
     * @param url      the frontend database host URL
     * @param dbName   the frontend database name, if any
     * @param userName the username to connect to the frontend database
     * @param password the password to connect to the frontend database
     */
    private void connect(String url, String dbName, String userName,
                         String password) throws SQLException {
        if (url == null) {
            throw new IllegalArgumentException("no database URL set up");
        }
        try {
            String driver = "";
            if (url.contains("sqlite")) {
                driver = "org.sqlite.JDBC";
            } else if (url.contains("mysql")) {
                driver = "com.mysql.jdbc.Driver";
            }
            if (!driver.isEmpty()) {
                Class.forName(driver).newInstance();
                this.connection = DriverManager.getConnection(url + dbName,
                                                              userName,
                                                              password);
            }
        } catch (Exception e) {
            LOG.error(e.getMessage(), e);
        }
    }

    /**
     * Set all instance variables required to connect to the platform frontend
     * database
     *
     * @param url      the frontend database host URL
     * @param dbName   the frontend database name, if any
     * @param userName the username to connect to the frontend database
     * @param password the password to connect to the frontend database
     */
    public void setFrontendCredentials(String url, String dbName,
                                       String userName, String password) {
        this.frontendDbUrl = url;
        this.dbName = dbName;
        this.dbUserName = userName;
        this.dbPassword = password;
    }
}
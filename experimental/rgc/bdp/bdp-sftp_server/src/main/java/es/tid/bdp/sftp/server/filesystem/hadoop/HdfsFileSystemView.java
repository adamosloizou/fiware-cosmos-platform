package es.tid.bdp.sftp.server.filesystem.hadoop;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.sshd.server.FileSystemView;
import org.apache.sshd.server.SshFile;
import org.apache.sshd.server.filesystem.NativeFileSystemFactory;
import org.apache.sshd.server.filesystem.NativeFileSystemView;
import org.apache.sshd.server.filesystem.NativeSshFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class HdfsFileSystemView implements FileSystemView {
    
    private final Logger LOG = LoggerFactory
    .getLogger(NativeFileSystemView.class);

    
    private final static String FS_DEFAULT_NAME = "fs.default.name";

    private String userName;
    private FileSystem hdfsSrc;
    
    private String currDir;


    private boolean caseInsensitive = false;

    /**
     * Constructor - internal do not use directly, use {@link NativeFileSystemFactory} instead
     */
    protected HdfsFileSystemView(String userName) {
        this(userName, false);
    }

    /**
     * Constructor - internal do not use directly, use {@link NativeFileSystemFactory} instead
     */
    public HdfsFileSystemView(String userName, boolean caseInsensitive) {
        if (userName == null) {
            throw new IllegalArgumentException("user can not be null");
        }

        this.caseInsensitive = caseInsensitive;

        currDir = System.getProperty("user.dir");
        this.userName = userName;
        
        Configuration confSrc = new Configuration();
        confSrc.set(FS_DEFAULT_NAME, "hdfs://pshdp01.hi.inet:8011");

        try {
            hdfsSrc = FileSystem.get(confSrc);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        // add last '/' if necessary
        LOG.debug("Hdfs filesystem view created for user \"{}\" with root \"{}\"", userName, currDir);
    }

    /**
     * Get file object.
     */
    public SshFile getFile(String file) {
        return getFile(currDir, file);
    }

    public SshFile getFile(SshFile baseDir, String file) {
        return getFile(baseDir.getAbsolutePath(), file);
    }

    protected SshFile getFile(String dir, String file) {
        // get actual file object
        String physicalName = NativeSshFile.getPhysicalName("/",
                dir, file, caseInsensitive);
        Path path = new Path(physicalName);

        // strip the root directory and return
        String userFileName = physicalName.substring("/".length() - 1);
        return new HdfsSshFile(hdfsSrc, userFileName, path, userName);
    }
}

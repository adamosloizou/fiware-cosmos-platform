package es.tid.cosmos.tests.frontend.om;

import java.io.File;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

import es.tid.cosmos.tests.tasks.Environment;
import es.tid.cosmos.tests.tasks.Task;
import es.tid.cosmos.tests.tasks.TaskStatus;

/**
 *
 * @author ximo
 */
public class FrontEndTask extends Task {
    private static final String FILE_BROWSER_URL = "filebrowser/view/";
    private boolean isRun;
    private final FrontEnd frontend;
    private final String taskId;
    private final String jarHdfsPath;
    private final String inputHdfsPath;

    public static String getJarHdfsPath(String user, String jarFileName) {
        return ("/user/" + user + "/jars/" + jarFileName);
    }

    public static String getJarHdfsPath(String user) {
        return getJarHdfsPath(user, "");
    }

    public static String getDataHdfsPath(String user, String dataFileName) {
        return ("/user/" + user + "/datasets/" + dataFileName);
    }

    public static String getDataHdfsPath(String user) {
        return getDataHdfsPath(user, "");
    }

    public static boolean jarExists(FrontEnd frontend, String jarName) {
        return fileExists(frontend, getJarHdfsPath(frontend.getUsername()),
                          jarName);
    }

    public static boolean dataSetExists(FrontEnd frontend,  String dataSetName) {
        return fileExists(frontend, getDataHdfsPath(frontend.getUsername()),
                          dataSetName);
    }

    private static boolean fileExists(FrontEnd frontend, String hdfsPath,
                                      String fileName) {
        frontend.gotoCosmosHome();
        WebDriver driver = frontend.getDriver();
        driver.get(frontend.getBaseUrl() + "/" + FILE_BROWSER_URL + hdfsPath);
        List<WebElement> files = driver.findElements(By.className("fb-file"));
        for (WebElement file : files) {
            if (file.getText().equals(fileName)) {
                return true;
            }
        }
        return false;
    }

    public static void uploadJar(FrontEnd frontend, String filePath) {
        UploadPage uploadPage = frontend.goToUpload();
        UploadJarPage uploadJarPage = uploadPage.goToUploadJar();
        uploadJarPage.setName(new File(filePath).getName());
        uploadJarPage.setJarFile(filePath);
        uploadJarPage.submitForm();
    }

    public static void uploadData(FrontEnd frontend, String filePath) {
        UploadPage uploadPage = frontend.goToUpload();
        UploadDataPage uploadDataPage = uploadPage.goToUploadData();
        uploadDataPage.setName(new File(filePath).getName());
        uploadDataPage.setDataFile(filePath);
        uploadDataPage.submitForm();
    }

    public FrontEndTask(FrontEnd frontend,
                        String inputHdfsPath,
                        String jarHdfsPath,
                        String taskId) {
        this.frontend = frontend;
        this.taskId = taskId;
        this.jarHdfsPath = jarHdfsPath;
        this.inputHdfsPath = inputHdfsPath;
        this.isRun = false;
    }

    public FrontEndTask(FrontEnd frontend,
                        String inputFilePath,
                        String jarPath) {
        this(frontend, inputFilePath, jarPath, UUID.randomUUID().toString());
    }

    public FrontEndTask(Environment env, String inputFilePath, String jarPath) {
        this(new FrontEnd(env), inputFilePath, jarPath);
    }

    public FrontEndTask(Environment env, String inputFilePath, String jarPath,
                        String taskId) {
        this(new FrontEnd(env), inputFilePath, jarPath, taskId);
    }

    private FrontEndTask(Environment env, String taskId) {
        this.frontend = new FrontEnd(env);
        assertTrue(this.frontend.taskExists(taskId));

        this.taskId = taskId;
        this.isRun = true;
        this.jarHdfsPath = null;
        this.inputHdfsPath = null;
    }

    public static FrontEndTask createFromExistingTaskId(Environment env,
                                                        String taskId) {
        return new FrontEndTask(env, taskId);
    }

    @Override
    public void run() {
        assertFalse(this.isRun, "Veryfing run hasn't been called previously."
                + " If this fails, it is a test bug.");

        CreateJobPage createJobPage = this.frontend.goToCreateNewJob();
        createJobPage.setName(this.taskId);
        createJobPage.setInputJar(this.jarHdfsPath);
        createJobPage.setInputFile(this.inputHdfsPath);
        createJobPage.create();
        this.isRun = true;
    }

    @Override
    public TaskStatus getStatus() {
        if (!this.isRun) {
            return TaskStatus.Created;
        }
        return this.frontend.getTaskStatus(taskId);
    }

    @Override
    public List<Map<String, String>> getResults() {
        ResultsPage resultsPage = this.frontend.goToResultsPage(this.taskId);
        return resultsPage.getResults();
    }

    @Override
    public String toString() {
        return "[FrontEndTask] Id = " + taskId;
    }
}

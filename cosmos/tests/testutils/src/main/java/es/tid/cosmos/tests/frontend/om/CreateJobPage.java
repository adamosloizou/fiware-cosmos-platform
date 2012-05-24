package es.tid.cosmos.tests.frontend.om;

import java.util.Set;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

/**
 *
 * @author ximo
 */
public class CreateJobPage {
    // HTML classes and ids
    public static final String INPUT_FILE_HTML_ID = "id_dataset_path";
    public static final String JAR_FILE_HTML_ID = "id_jar_path";
    public static final String NAME_FILE_HTML_ID = "id_name";
    public static final String SAMPLE_JAR_LINK_ID = "sample-jar-link";
    public static final String JAR_RESTRICTIONS_ID = "jar-restrictions";
    private final String createJobUrl;
    private final String mainWindow;
    private WebDriver driver;

    public String getJarRestrictions() {
        assertCorrectUrl();
        this.driver.findElement(By.id(JAR_RESTRICTIONS_ID)).click();
        this.switchToPopup();
        String source = this.driver.getPageSource();
        this.driver.get(this.createJobUrl);
        return source;

    }

    private void switchToPopup() {
        Set<String> handles = this.driver.getWindowHandles();
        assertEquals(handles.size(), 2);
        for (String handle : handles) {
            if (!handle.equals(this.mainWindow)) {
                this.driver.switchTo().window(handle);
                break;
            }
        }
    }

    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.createJobUrl);
    }

    public CreateJobPage(WebDriver driver) {
        this.driver = driver;
        this.createJobUrl = this.driver.getCurrentUrl();
        this.mainWindow = this.driver.getWindowHandle();
    }

    public void setInputFile(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(INPUT_FILE_HTML_ID));
        inputElement.sendKeys(filePath);
    }

    public void setName(String name) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(NAME_FILE_HTML_ID));
        inputElement.sendKeys(name);
    }

    public void setInputJar(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(JAR_FILE_HTML_ID));
        inputElement.sendKeys(filePath);
    }

    public void create() {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(INPUT_FILE_HTML_ID));
        inputElement.submit();
    }
}
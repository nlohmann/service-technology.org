package lscminer.utilities;

import java.io.File;
import javax.swing.filechooser.FileFilter;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class ClasspathFilter extends FileFilter{

    @Override
    public boolean accept(File f) {
        if (f.isDirectory()) {
            return true;
        }
        if (f.getName().endsWith(".jar") || f.getName().endsWith(".zip")) {
            return true;
        } else {
            return false;
        }
    }

    @Override
    public String getDescription() {
        return "Just Java Libraris";
    }

}

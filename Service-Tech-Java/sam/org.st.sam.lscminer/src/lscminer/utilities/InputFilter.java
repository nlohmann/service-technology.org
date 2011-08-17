package lscminer.utilities;

import java.io.File;
import javax.swing.filechooser.FileFilter;

/**
 *
 * @author nacuong
 */
public class InputFilter extends FileFilter{

    /**
     * 
     * @param f
     * @return
     */
    @Override
    public boolean accept(File f) {
        if (f.isDirectory()) {
          return true;
        }
        if (f.getName().endsWith(".txt") || f.getName().endsWith(".in")){
          return true;
        } else {
          return false;
        }
    }

    /**
     * 
     * @return
     */
    @Override
    public String getDescription() {
        return "Just Inputs";
    }
    
}

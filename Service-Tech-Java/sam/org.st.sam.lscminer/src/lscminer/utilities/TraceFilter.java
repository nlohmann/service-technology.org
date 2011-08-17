package lscminer.utilities;

import java.io.File;
import javax.swing.filechooser.FileFilter;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class TraceFilter extends FileFilter{

  /**
   * Accept on traces file with extension .txt or .trace
   * @param f a file to be considered
   * @return
   */
  public boolean accept(File file) {
    if (file.isDirectory()) {
      return true;
    }
    if (file.getName().endsWith(".xes")){
      return true;
    } else {
      return false;
    }
  }

  /**
   * Description of this filter
   * @return
   */
  public String getDescription() {
    return "Just Traces";
  }
}

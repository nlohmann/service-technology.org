package lscminer.utilities;

import java.io.File;
import javax.swing.filechooser.FileFilter;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class BinaryFilter extends FileFilter {

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
    if (f.getName().endsWith(".exe") || !f.getName().contains(".")) {
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

package lscminer.utilities;

import java.io.File;
import javax.swing.filechooser.FileFilter;;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class ProjectFolderFilter extends FileFilter{

    @Override
    public boolean accept(File f) {
        return isLSCProjectParentFolder(f);
    }

    public static boolean isLSCTaskFolder(File f){
        boolean containTraces = false;
        boolean containLog = false;
        boolean containLSC = false;
        boolean containLTL = false;
        boolean containPattern = false;

        if (f.isFile()) return false;
        File[] subDirs = f.listFiles();
        if (subDirs != null){
            for (File subDir : subDirs){
                if (subDir.isFile()){
                    if (subDir.getName().equals("log.txt")) containLog = true;
                    else if (subDir.getName().equals("lscs.txt")) containLSC = true;
                    else if(subDir.getName().equals("ltls.txt")) containLTL = true;
                    else if(subDir.getName().equals("patterns.txt")) containPattern = true;
                } else if (subDir.getName().equals("traces")){
                        containTraces = true;
                }
            }
        }

        return containTraces && containLog && containLSC && containLTL && containPattern;
    }

    public static boolean isLSCProjectParentFolder(File f){
        if (isLSCTaskFolder(f)) {
            return true;
        }
        else {
            File[] subDirs = f.listFiles();
            if (subDirs != null){
                for (File subDir : subDirs){
                    if (isLSCTaskFolder(subDir)) return true;
                }
            }
        }

        return false;
    }


    @Override
    /**
    * Description of this filter
    * @return
    */
    public String getDescription() {
        return "Just LSC Projects";
    }
    
}

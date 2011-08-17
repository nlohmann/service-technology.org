package lscminer.gui;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreePath;
import lscminer.datastructure.Project;
import lscminer.utilities.ProjectParser;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class ProjectTreeMouseAdapter extends MouseAdapter {
  MainFrame frame;

  public ProjectTreeMouseAdapter(MainFrame frame){
    this.frame = frame;
  }

  @Override
  public void mousePressed(MouseEvent e) {
    JTree tree = frame.getProjectTree();
    TreePath selPath = tree.getPathForLocation(e.getX(), e.getY());
      if (selPath != null){
      Object[] selNodes = selPath.getPath();
      DefaultMutableTreeNode selNode = (DefaultMutableTreeNode) selNodes[selNodes.length-1];
      if (e.getClickCount() == 2 && tree.getModel().isLeaf(selNode)){
        /* trigger clicking action */
        String taskName = (String)selNode.getUserObject();
        Project project = (Project) ((DefaultMutableTreeNode)selNodes[selNodes.length-2]).getUserObject();

        /* display lsc result into main panel */
        ProjectParser projectParser = new ProjectParser();
        projectParser.parse(project, taskName);
        frame.displayMineResult(projectParser.projectName,
                projectParser.taskName,
                projectParser.projectLocation,
                projectParser.support,
                projectParser.confidence,
                projectParser.density,
                projectParser.mineType,
                projectParser.mineTime,
                projectParser.mineRuntime,
                projectParser.lscs);
        frame.displayDataset(projectParser.dataset);

        /* update history tree */
        frame.updateHistoryTree(project, taskName);

        /* record into history file */
        File history = new File(frame.getHistoryFileLocation());
        try {
          PrintWriter hisPw = new PrintWriter(new FileOutputStream(history, true));
          hisPw.println(projectParser.projectName);
          hisPw.println(projectParser.projectLocation);
          hisPw.close();
        } catch (IOException exp){
        }
      }
    }
  }
}

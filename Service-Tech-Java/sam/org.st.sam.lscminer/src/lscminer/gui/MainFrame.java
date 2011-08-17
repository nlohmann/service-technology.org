/*
 * Main.java
 *
 * Created on Oct 3, 2010, 3:56:12 AM
 */
package lscminer.gui;

import lscminer.*;
import java.awt.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;
import javax.swing.*;
import javax.swing.table.*;
import javax.swing.tree.*;
import lscminer.datastructure.*;
import lscminer.utilities.ProjectFolderFilter;
import lscminer.utilities.ProjectParser;

/**
 *
 * @author nacuong
 */
public class MainFrame extends javax.swing.JFrame {

    static final String hitoryFileLocation = Main.getHisLoc();
    static final int maxProject = 5;
    LinkedList<Project> history;

    /** Creates new form Main */
    public MainFrame() {
        initComponents();
        hideMainPanel();

        // <editor-fold desc="initialize history">
        history = new LinkedList<Project>();
        HashMap<String, Boolean> projectLocations = new HashMap<String, Boolean>();
        try {
            File historyFile = new File(hitoryFileLocation);
            Scanner historySc = new Scanner(new File(hitoryFileLocation));
            while (historySc.hasNext()) {
                String projectName = historySc.nextLine();
                if (!projectName.trim().equals("")) {
                    String projectLocation = historySc.nextLine();
                    Project project = new Project(projectName, projectLocation);
                    if (!projectLocations.containsKey(projectLocation)) {
                        history.addFirst(project);
                        projectLocations.put(projectLocation, Boolean.TRUE);
                    }
                }
            }

            if (historyFile.length() > 10000000) { // 10MB
        /* consider to rewrittern the file */
                PrintWriter historyPw = new PrintWriter(historyFile.getAbsolutePath());
                int historyProject = 0;
                for (int inx = history.size() - 1; inx >= 0; inx--) {
                    Project project = history.get(inx);
                    historyPw.println(project.getName());
                    historyPw.println(project.getLocation());
                    historyProject++;
                    if (historyProject == 1000) {
                        /* consider to store maximum 1000 project only */
                        break;
                    }
                }
                historyPw.close();
            }
        } catch (FileNotFoundException ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
        }
        createHistoryTree();
        // </editor-fold>

        projectTree.setEditable(true);
        projectTree.addMouseListener(new ProjectTreeMouseAdapter(this));
    }

    public JTree getProjectTree() {
        return projectTree;
    }

    public String getHistoryFileLocation() {
        return hitoryFileLocation;
    }

    private LinkedList<String> getTaskList(String projectLocation) {
        LinkedList<String> taskList = new LinkedList<String>();
        File projectFolder = new File(projectLocation);
        File[] taskFolders = projectFolder.listFiles();
        for (File taskFolder : taskFolders) {
            if (taskFolder.isDirectory()) {
                taskList.add(taskFolder.getName());
            }
        }

        return taskList;
    }

    private void createHistoryTree() {
        if (history.isEmpty()) {
            projectTree.setVisible(false);
        } else {
            DefaultTreeModel tree = (DefaultTreeModel) projectTree.getModel();
            DefaultMutableTreeNode root = (DefaultMutableTreeNode) tree.getRoot();
            /* create tree from history */
            int projectCount = 0;
            for (Project project : history) {
                /* display maxProject projects only */
                if (projectCount < maxProject) {
                    LinkedList<String> taskNames = getTaskList(project.getLocation());

                    DefaultMutableTreeNode projectNode = new DefaultMutableTreeNode(project);
                    root.add(projectNode);

                    for (String taskName : taskNames) {
                        DefaultMutableTreeNode taskNode = new DefaultMutableTreeNode(taskName);
                        projectNode.add(taskNode);
                    }
                    projectCount++;
                }
            }
        }
    }

    public void updateHistoryTree(Project project, String taskName) {
        boolean isUpdated = false;
        boolean noUpdate = false;

        projectTree.setVisible(true);
        DefaultTreeModel tree = (DefaultTreeModel) projectTree.getModel();
        DefaultMutableTreeNode root = (DefaultMutableTreeNode) tree.getRoot();
        for (int childInx = 0; childInx < root.getChildCount(); childInx++) {
            DefaultMutableTreeNode projectNode = (DefaultMutableTreeNode) root.getChildAt(childInx);
            Project currentProject = (Project) projectNode.getUserObject();
            if (currentProject.equals(project)) {
                /* update task inside this project */
                for (int projChildInx = 0; projChildInx < projectNode.getChildCount(); projChildInx++) {
                    DefaultMutableTreeNode taskNode = (DefaultMutableTreeNode) projectNode.getChildAt(projChildInx);
                    if (((String) taskNode.getUserObject()).trim().equals(taskName.trim())) {
                        /* match a task, stop udating since the task is already there
                         * bring the project to first */
                        isUpdated = true;
                        if (childInx != 0) {
                            root.insert(projectNode, 0);
                        } else {
                            noUpdate = true;
                        }
                        break;
                    }
                }
                if (!isUpdated) {
                    /* the task is not there yet, add it in */
                    projectNode.insert(new DefaultMutableTreeNode(taskName), 0);
                    isUpdated = true;
                } else {
                    break;
                }
            }
            if (isUpdated) {
                break;
            }
        }

        if (!isUpdated) {
            /* the project is not there yet, add it in */
            if (root.getChildCount() == maxProject) {
                /* remove the last project first */
                root.remove(root.getChildCount() - 1);
            }
            DefaultMutableTreeNode newProjectNode = new DefaultMutableTreeNode(project);
            LinkedList<String> taskList = getTaskList(project.getLocation());
            for (String task : taskList) {
                DefaultMutableTreeNode newTaskNode = new DefaultMutableTreeNode(task);
                newProjectNode.add(newTaskNode);
            }
            root.insert(newProjectNode, 0);
        }

        if (!noUpdate) {
            tree.reload();
        }
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    mineFromTracesButton = new javax.swing.JButton();
    mineJavaProgramButton = new javax.swing.JButton();
    mineBinaryProgramButton = new javax.swing.JButton();
    settingButton = new javax.swing.JButton();
    historyPanel = new javax.swing.JPanel();
    jScrollPane5 = new javax.swing.JScrollPane();
    projectTree = new javax.swing.JTree();
    recentProjectLabel = new javax.swing.JLabel();
    openProjectButton = new javax.swing.JButton();
    mainPanel = new javax.swing.JPanel();
    projectNameLabel = new javax.swing.JLabel();
    taskNameLabel = new javax.swing.JLabel();
    projectLocationLabel = new javax.swing.JLabel();
    projectNameTextField = new javax.swing.JTextField();
    taskNameTextField = new javax.swing.JTextField();
    projectLocationTextField = new javax.swing.JTextField();
    mainTabbedPane = new javax.swing.JTabbedPane();
    jScrollPane1 = new javax.swing.JScrollPane();
    lscTable = new javax.swing.JTable();
    traceTabbedPane = new javax.swing.JTabbedPane();
    miningParametePane = new javax.swing.JPanel();
    supportLabel = new javax.swing.JLabel();
    supportTextField = new javax.swing.JTextField();
    confidenceLabel = new javax.swing.JLabel();
    confidenceTextField = new javax.swing.JTextField();
    densityLabel = new javax.swing.JLabel();
    densityTextField = new javax.swing.JTextField();
    miningTypeLabel = new javax.swing.JLabel();
    miningTypeTextField = new javax.swing.JTextField();
    miningTimeTextField = new javax.swing.JTextField();
    miningRuntimeTextField = new javax.swing.JTextField();
    miningTimeLabel = new javax.swing.JLabel();
    miningRuntimeLabel = new javax.swing.JLabel();
    jSeparator1 = new javax.swing.JSeparator();
    jSeparator2 = new javax.swing.JSeparator();
    projectNameLabel2 = new javax.swing.JLabel();
    projectNameTextField2 = new javax.swing.JTextField();
    taskNameLabel2 = new javax.swing.JLabel();
    taskNameTextField2 = new javax.swing.JTextField();
    projectLocationLabel2 = new javax.swing.JLabel();
    projectLocationTextField2 = new javax.swing.JTextField();
    topSeparator = new javax.swing.JSeparator();

    setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
    setTitle("NUS Live Sequence Chart Miner");
    setFocusable(false);
    setIconImage(new ImageIcon("lib/lsc-miner.jpg").getImage());
    setLocationByPlatform(true);
    setName("mainFrame"); // NOI18N
    setResizable(false);

    mineFromTracesButton.setFont(new java.awt.Font("Lucida Sans Unicode", 0, 18));
    mineFromTracesButton.setText("<html> <center> Mine from Traces </center> </html>");
    mineFromTracesButton.setFocusable(false);
    mineFromTracesButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        mineFromTracesButtonActionPerformed(evt);
      }
    });

    mineJavaProgramButton.setFont(new java.awt.Font("Lucida Sans Unicode", 0, 18));
    mineJavaProgramButton.setText("Mine Java Program");
    mineJavaProgramButton.setFocusable(false);
    mineJavaProgramButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        mineJavaProgramButtonActionPerformed(evt);
      }
    });

    mineBinaryProgramButton.setFont(new java.awt.Font("Lucida Sans Unicode", 0, 18));
    mineBinaryProgramButton.setText("Mine Binary Program");
    mineBinaryProgramButton.setFocusable(false);
    mineBinaryProgramButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        mineBinaryProgramButtonActionPerformed(evt);
      }
    });

    settingButton.setFont(new java.awt.Font("Lucida Sans Unicode", 0, 18));
    settingButton.setText("Preferences");
    settingButton.setFocusable(false);
    settingButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        settingButtonActionPerformed(evt);
      }
    });

    historyPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "   History Panel   ", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 12))); // NOI18N

    projectTree.setBackground(javax.swing.UIManager.getDefaults().getColor("Panel.background"));
    projectTree.setBorder(javax.swing.BorderFactory.createEmptyBorder(10, 10, 10, 10));
    projectTree.setFont(new java.awt.Font("Tahoma", 0, 13));
    javax.swing.tree.DefaultMutableTreeNode treeNode1 = new javax.swing.tree.DefaultMutableTreeNode("Projects");
    projectTree.setModel(new javax.swing.tree.DefaultTreeModel(treeNode1));
    projectTree.setCellRenderer(null);
    projectTree.setOpaque(false);
    jScrollPane5.setViewportView(projectTree);

    recentProjectLabel.setFont(new java.awt.Font("Tahoma", 0, 13));
    recentProjectLabel.setText("Recent Projects");

    openProjectButton.setFont(new java.awt.Font("Tahoma", 0, 14));
    openProjectButton.setText("Open Another Project");
    openProjectButton.setFocusPainted(false);
    openProjectButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        openProjectButtonActionPerformed(evt);
      }
    });

    javax.swing.GroupLayout historyPanelLayout = new javax.swing.GroupLayout(historyPanel);
    historyPanel.setLayout(historyPanelLayout);
    historyPanelLayout.setHorizontalGroup(
      historyPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(historyPanelLayout.createSequentialGroup()
        .addContainerGap()
        .addGroup(historyPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(openProjectButton, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 178, Short.MAX_VALUE)
          .addComponent(jScrollPane5, javax.swing.GroupLayout.DEFAULT_SIZE, 178, Short.MAX_VALUE)
          .addComponent(recentProjectLabel))
        .addContainerGap())
    );
    historyPanelLayout.setVerticalGroup(
      historyPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(historyPanelLayout.createSequentialGroup()
        .addContainerGap()
        .addComponent(recentProjectLabel)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jScrollPane5, javax.swing.GroupLayout.PREFERRED_SIZE, 443, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addGap(18, 18, 18)
        .addComponent(openProjectButton, javax.swing.GroupLayout.DEFAULT_SIZE, 39, Short.MAX_VALUE)
        .addContainerGap())
    );

    mainPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "   Main Panel   ", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 12))); // NOI18N

    projectNameLabel.setFont(new java.awt.Font("Tahoma", 0, 13));
    projectNameLabel.setText("Project Name :");

    taskNameLabel.setFont(new java.awt.Font("Tahoma", 0, 13));
    taskNameLabel.setText("Task Name :");

    projectLocationLabel.setFont(new java.awt.Font("Tahoma", 0, 13));
    projectLocationLabel.setText("Project Location :");

    projectNameTextField.setBackground(new java.awt.Color(240, 240, 240));
    projectNameTextField.setEditable(false);
    projectNameTextField.setFont(new java.awt.Font("Tahoma", 0, 13));
    projectNameTextField.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 1, 1, 1));
    projectNameTextField.setEnabled(false);

    taskNameTextField.setBackground(new java.awt.Color(240, 240, 240));
    taskNameTextField.setEditable(false);
    taskNameTextField.setFont(new java.awt.Font("Tahoma", 0, 13));
    taskNameTextField.setBorder(null);
    taskNameTextField.setEnabled(false);
    taskNameTextField.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        taskNameTextFieldActionPerformed(evt);
      }
    });

    projectLocationTextField.setBackground(new java.awt.Color(240, 240, 240));
    projectLocationTextField.setEditable(false);
    projectLocationTextField.setFont(new java.awt.Font("Tahoma", 0, 13));
    projectLocationTextField.setBorder(null);
    projectLocationTextField.setEnabled(false);

    mainTabbedPane.setFont(new java.awt.Font("Tahoma", 0, 12));

    lscTable.setAutoCreateRowSorter(true);
    lscTable.setFont(new java.awt.Font("Tahoma", 0, 12));
    lscTable.setModel(new javax.swing.table.DefaultTableModel(
      new Object [][] {

      },
      new String [] {
        "LSC#", "Distinct Objects", "Pre-charts Events", "Main-chart Events", "Support", "Confidence"
      }
    ) {
      Class[] types = new Class [] {
        java.lang.Integer.class, java.lang.Integer.class, java.lang.Integer.class, java.lang.Integer.class, java.lang.Integer.class, java.lang.Double.class
      };

      public Class getColumnClass(int columnIndex) {
        return types [columnIndex];
      }
    });
    jScrollPane1.setViewportView(lscTable);

    mainTabbedPane.addTab("Live Sequence Charts", jScrollPane1);
    mainTabbedPane.addTab("Call Traces", traceTabbedPane);

    supportLabel.setFont(new java.awt.Font("Tahoma", 1, 14));
    supportLabel.setText("Support :");

    supportTextField.setBackground(new java.awt.Color(240, 240, 240));
    supportTextField.setEditable(false);
    supportTextField.setFont(new java.awt.Font("Tahoma", 0, 14));
    supportTextField.setBorder(null);

    confidenceLabel.setFont(new java.awt.Font("Tahoma", 1, 14));
    confidenceLabel.setText("Confidence :");

    confidenceTextField.setBackground(new java.awt.Color(240, 240, 240));
    confidenceTextField.setEditable(false);
    confidenceTextField.setFont(new java.awt.Font("Tahoma", 0, 14));
    confidenceTextField.setBorder(null);

    densityLabel.setFont(new java.awt.Font("Tahoma", 1, 14));
    densityLabel.setText("Density :");

    densityTextField.setBackground(new java.awt.Color(240, 240, 240));
    densityTextField.setEditable(false);
    densityTextField.setFont(new java.awt.Font("Tahoma", 0, 14));
    densityTextField.setBorder(null);

    miningTypeLabel.setFont(new java.awt.Font("Tahoma", 1, 14));
    miningTypeLabel.setText("Mining Type :");

    miningTypeTextField.setBackground(new java.awt.Color(240, 240, 240));
    miningTypeTextField.setFont(new java.awt.Font("Tahoma", 0, 14));
    miningTypeTextField.setBorder(null);
    miningTypeTextField.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        miningTypeTextFieldActionPerformed(evt);
      }
    });

    miningTimeTextField.setBackground(new java.awt.Color(240, 240, 240));
    miningTimeTextField.setFont(new java.awt.Font("Tahoma", 0, 14));
    miningTimeTextField.setBorder(null);

    miningRuntimeTextField.setBackground(new java.awt.Color(240, 240, 240));
    miningRuntimeTextField.setFont(new java.awt.Font("Tahoma", 0, 14));
    miningRuntimeTextField.setBorder(null);
    miningRuntimeTextField.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        miningRuntimeTextFieldActionPerformed(evt);
      }
    });

    miningTimeLabel.setFont(new java.awt.Font("Tahoma", 1, 14));
    miningTimeLabel.setText("Time of Mining :");

    miningRuntimeLabel.setFont(new java.awt.Font("Tahoma", 1, 14));
    miningRuntimeLabel.setText("Mining Runtime :");

    projectNameLabel2.setFont(new java.awt.Font("Tahoma", 1, 14));
    projectNameLabel2.setText("Project Name :");

    projectNameTextField2.setBackground(new java.awt.Color(240, 240, 240));
    projectNameTextField2.setEditable(false);
    projectNameTextField2.setFont(new java.awt.Font("Tahoma", 0, 14));
    projectNameTextField2.setBorder(null);

    taskNameLabel2.setFont(new java.awt.Font("Tahoma", 1, 14));
    taskNameLabel2.setText(" Task Name :");

    taskNameTextField2.setBackground(new java.awt.Color(240, 240, 240));
    taskNameTextField2.setEditable(false);
    taskNameTextField2.setFont(new java.awt.Font("Tahoma", 0, 14));
    taskNameTextField2.setBorder(null);

    projectLocationLabel2.setFont(new java.awt.Font("Tahoma", 1, 14));
    projectLocationLabel2.setText("Project Location :");

    projectLocationTextField2.setBackground(new java.awt.Color(240, 240, 240));
    projectLocationTextField2.setEditable(false);
    projectLocationTextField2.setFont(new java.awt.Font("Tahoma", 0, 14));
    projectLocationTextField2.setBorder(null);

    javax.swing.GroupLayout miningParametePaneLayout = new javax.swing.GroupLayout(miningParametePane);
    miningParametePane.setLayout(miningParametePaneLayout);
    miningParametePaneLayout.setHorizontalGroup(
      miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, miningParametePaneLayout.createSequentialGroup()
        .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
          .addGroup(javax.swing.GroupLayout.Alignment.LEADING, miningParametePaneLayout.createSequentialGroup()
            .addGap(115, 115, 115)
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(densityLabel)
              .addComponent(confidenceLabel)
              .addComponent(supportLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(confidenceTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 402, Short.MAX_VALUE)
              .addComponent(supportTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 402, Short.MAX_VALUE)
              .addComponent(densityTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 402, Short.MAX_VALUE)))
          .addGroup(miningParametePaneLayout.createSequentialGroup()
            .addGap(84, 84, 84)
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(miningTimeLabel)
              .addComponent(miningTypeLabel)
              .addComponent(miningRuntimeLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(miningRuntimeTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 403, Short.MAX_VALUE)
              .addComponent(miningTimeTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 403, Short.MAX_VALUE)
              .addComponent(miningTypeTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 403, Short.MAX_VALUE))))
        .addGap(212, 212, 212))
      .addGroup(miningParametePaneLayout.createSequentialGroup()
        .addGap(78, 78, 78)
        .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
          .addComponent(projectLocationLabel2)
          .addComponent(taskNameLabel2)
          .addComponent(projectNameLabel2))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(taskNameTextField2, javax.swing.GroupLayout.DEFAULT_SIZE, 459, Short.MAX_VALUE)
          .addComponent(projectNameTextField2, javax.swing.GroupLayout.DEFAULT_SIZE, 459, Short.MAX_VALUE)
          .addComponent(projectLocationTextField2, javax.swing.GroupLayout.PREFERRED_SIZE, 431, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addGap(156, 156, 156))
      .addGroup(miningParametePaneLayout.createSequentialGroup()
        .addGap(63, 63, 63)
        .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
          .addComponent(jSeparator2, javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(jSeparator1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 659, Short.MAX_VALUE))
        .addGap(97, 97, 97))
    );
    miningParametePaneLayout.setVerticalGroup(
      miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, miningParametePaneLayout.createSequentialGroup()
        .addContainerGap(69, Short.MAX_VALUE)
        .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
          .addGroup(miningParametePaneLayout.createSequentialGroup()
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(taskNameLabel2)
              .addComponent(taskNameTextField2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addGap(26, 26, 26))
          .addGroup(miningParametePaneLayout.createSequentialGroup()
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(projectNameLabel2)
              .addComponent(projectNameTextField2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addGap(36, 36, 36)
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(projectLocationLabel2)
              .addComponent(projectLocationTextField2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))))
        .addGap(18, 18, 18)
        .addComponent(jSeparator2, javax.swing.GroupLayout.PREFERRED_SIZE, 18, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
          .addGroup(miningParametePaneLayout.createSequentialGroup()
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(confidenceLabel)
              .addComponent(confidenceTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addGap(26, 26, 26))
          .addGroup(miningParametePaneLayout.createSequentialGroup()
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(supportLabel)
              .addComponent(supportTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addGap(36, 36, 36)
            .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(densityLabel)
              .addComponent(densityTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))))
        .addGap(18, 18, 18)
        .addComponent(jSeparator1, javax.swing.GroupLayout.PREFERRED_SIZE, 10, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(miningTypeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(miningTypeLabel))
        .addGap(10, 10, 10)
        .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(miningTimeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(miningTimeLabel))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addGroup(miningParametePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(miningRuntimeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(miningRuntimeLabel))
        .addGap(105, 105, 105))
    );

    mainTabbedPane.addTab("Mining Parameter", miningParametePane);

    javax.swing.GroupLayout mainPanelLayout = new javax.swing.GroupLayout(mainPanel);
    mainPanel.setLayout(mainPanelLayout);
    mainPanelLayout.setHorizontalGroup(
      mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(mainPanelLayout.createSequentialGroup()
        .addContainerGap()
        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(mainPanelLayout.createSequentialGroup()
            .addComponent(mainTabbedPane, javax.swing.GroupLayout.DEFAULT_SIZE, 824, Short.MAX_VALUE)
            .addContainerGap())
          .addGroup(mainPanelLayout.createSequentialGroup()
            .addComponent(topSeparator, javax.swing.GroupLayout.DEFAULT_SIZE, 824, Short.MAX_VALUE)
            .addContainerGap())
          .addGroup(mainPanelLayout.createSequentialGroup()
            .addGap(35, 35, 35)
            .addComponent(projectNameLabel)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(projectNameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 98, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(18, 18, 18)
            .addComponent(taskNameLabel)
            .addGap(4, 4, 4)
            .addComponent(taskNameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 101, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(18, 18, 18)
            .addComponent(projectLocationLabel)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(projectLocationTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 174, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(8, 8, 8))))
    );
    mainPanelLayout.setVerticalGroup(
      mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(mainPanelLayout.createSequentialGroup()
        .addComponent(topSeparator, javax.swing.GroupLayout.PREFERRED_SIZE, 7, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(projectNameLabel)
          .addComponent(taskNameLabel)
          .addComponent(taskNameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(projectLocationLabel)
          .addComponent(projectLocationTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(projectNameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addComponent(mainTabbedPane, javax.swing.GroupLayout.DEFAULT_SIZE, 491, Short.MAX_VALUE)
        .addContainerGap())
    );

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(layout.createSequentialGroup()
            .addGap(18, 18, 18)
            .addComponent(mineFromTracesButton, javax.swing.GroupLayout.PREFERRED_SIZE, 210, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(18, 18, 18)
            .addComponent(mineJavaProgramButton, javax.swing.GroupLayout.PREFERRED_SIZE, 210, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(18, 18, 18)
            .addComponent(mineBinaryProgramButton))
          .addGroup(layout.createSequentialGroup()
            .addContainerGap()
            .addComponent(mainPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
          .addComponent(historyPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
          .addComponent(settingButton, javax.swing.GroupLayout.DEFAULT_SIZE, 210, Short.MAX_VALUE))
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(mineFromTracesButton, javax.swing.GroupLayout.PREFERRED_SIZE, 83, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
            .addComponent(settingButton, javax.swing.GroupLayout.PREFERRED_SIZE, 83, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addComponent(mineJavaProgramButton, javax.swing.GroupLayout.PREFERRED_SIZE, 83, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addComponent(mineBinaryProgramButton, javax.swing.GroupLayout.PREFERRED_SIZE, 83, javax.swing.GroupLayout.PREFERRED_SIZE)))
        .addGap(23, 23, 23)
        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(mainPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
          .addComponent(historyPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        .addContainerGap())
    );

    pack();
  }// </editor-fold>//GEN-END:initComponents

    private void hideMainPanel() {
        projectNameLabel.setVisible(false);
        taskNameLabel.setVisible(false);
        projectLocationLabel.setVisible(false);
        mainTabbedPane.setVisible(false);
        topSeparator.setBackground(new Color(240, 240, 240));
        topSeparator.setForeground(new Color(240, 240, 240));
    }

    private void showMainPanel() {
        projectNameLabel.setVisible(true);
        taskNameLabel.setVisible(true);
        projectLocationLabel.setVisible(true);
        mainTabbedPane.setVisible(true);
        projectNameTextField.setDisabledTextColor(new Color(109, 109, 109));
        taskNameTextField.setDisabledTextColor(new Color(109, 109, 109));
        projectLocationTextField.setDisabledTextColor(new Color(109, 109, 109));
        projectNameTextField2.setDisabledTextColor(new Color(109, 109, 109));
        taskNameTextField2.setDisabledTextColor(new Color(109, 109, 109));
        projectLocationTextField2.setDisabledTextColor(new Color(109, 109, 109));
        supportTextField.setDisabledTextColor(new Color(109, 109, 109));
        confidenceTextField.setDisabledTextColor(new Color(109, 109, 109));
        densityTextField.setDisabledTextColor(new Color(109, 109, 109));
        miningTypeTextField.setDisabledTextColor(new Color(109, 109, 109));
        miningTimeTextField.setDisabledTextColor(new Color(109, 109, 109));
        miningRuntimeTextField.setDisabledTextColor(new Color(109, 109, 109));
        mainTabbedPane.setVisible(true);
    }

    private void setTextField(String text, JTextField textField) {
        if (text.length() > 20) {
            textField.setText(text.substring(0, 20) + "...");
            textField.setToolTipText(text);
        } else {
            textField.setText(text);
        }
    }

    private void mineFromTracesButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_mineFromTracesButtonActionPerformed
        MineFromTracesDialog traceMinerDialog = new MineFromTracesDialog(this, true);
        traceMinerDialog.setVisible(true);

        if (traceMinerDialog.mineStatus) {
            // <editor-fold desc="set project information">
            String projectName = traceMinerDialog.mineProjectName;
            String taskName = traceMinerDialog.mineTaskName;
            String projectLocation = traceMinerDialog.mineProjectLocation;
            String support = traceMinerDialog.mineSupport + "";
            String confidence = traceMinerDialog.mineConfidence + "";
            String density = traceMinerDialog.mineDensity + "";
            String mineType = traceMinerDialog.mineType;
            String mineTime = traceMinerDialog.mineTime;
            String mineRuntime = traceMinerDialog.mineRuntime + "s";
            ArrayList<LSC> lscs = traceMinerDialog.mineLSC;
            //LSCEvent[][] dataset = traceMinerDialog.mineDataSet;
            // </editor-fold>

            // <editor-fold desc="create lsc table">
            displayMineResult(projectName, taskName, projectLocation, support, confidence, density, mineType, mineTime, mineRuntime, lscs);
            //displayDataset(dataset);
            // </editor-fold>

            /* update history tree */
            updateHistoryTree(new Project(projectName, projectLocation), taskName);
        }
}//GEN-LAST:event_mineFromTracesButtonActionPerformed

    public void displayMineResult(String projectName, String taskName, String projectLocation,
            String support, String confidence, String density,
            String mineType, String mineTime, String mineRuntime, ArrayList<LSC> lscs) {
        setTextField(projectName, projectNameTextField);
        setTextField(taskName, taskNameTextField);
        setTextField(projectLocation, projectLocationTextField);

        projectNameTextField2.setText(projectName);
        taskNameTextField2.setText(taskName);
        projectLocationTextField2.setText(projectLocation);

        supportTextField.setText(support);
        confidenceTextField.setText(confidence);
        densityTextField.setText(density);

        miningTypeTextField.setText(mineType);
        miningTimeTextField.setText(mineTime);
        miningRuntimeTextField.setText(mineRuntime + "s");

        lscTable.setDefaultEditor(Integer.class, new LSCCellEditor(lscs));
        lscTable.setDefaultEditor(Double.class, new LSCCellEditor(lscs));
        DefaultTableModel table = (DefaultTableModel) lscTable.getModel();

        /* remove all rows */
        int rowCount = table.getRowCount();
        while (rowCount != 0) {
            table.removeRow(rowCount - 1);
            rowCount--;
        }

        /* create new rows */
        for (int irow = 0; irow < lscs.size(); irow++) {
            LSC lsc = lscs.get(irow);
            Object[] row = {irow + 1, lsc.getObjectsNo(), lsc.getPrechartEventNo(), lsc.getMainchartEventNo(), lsc.getSupport(), lsc.getConfidence()};
            ((DefaultTableModel) lscTable.getModel()).addRow(row);
        }


        /* show main panel */
        showMainPanel();
    }
    
    public void displayDataset(LSCEvent[][] dataset){
      traceTabbedPane.removeAll();
      int traceInx = 1;
      for (LSCEvent[] trace : dataset) {
        /* create trace table */
        JTable traceTable = new JTable();
        traceTable.setFont(new java.awt.Font("Tahoma", 0, 12));
        traceTable.setModel(new javax.swing.table.DefaultTableModel(
                new Object[][]{},
                new String[]{
                  "Index", "Caller Object", "Callee Object", "Method Call"
                }) {

          Class[] types = new Class[]{
            java.lang.Integer.class, java.lang.String.class, java.lang.String.class, java.lang.String.class,
          };

          @Override
          public Class getColumnClass(int columnIndex) {
            return types[columnIndex];
          }
        });

        /* add data into table */
        int eventInx = 1;
        for (LSCEvent event: trace){
          Object[] row = {eventInx, event.getCaller(), event.getCallee(), event.getMethod()};
          ((DefaultTableModel)traceTable.getModel()).addRow(row);
          eventInx++;
        }

        /* add table into main view */
        JScrollPane traceScrollPane = new JScrollPane();
        traceScrollPane.setViewportView(traceTable);
        traceTabbedPane.addTab("Trace #" + traceInx, traceScrollPane);
        traceInx++;
      }
    }

    private void mineJavaProgramButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_mineJavaProgramButtonActionPerformed
        MineFromJavaDialog javaMinerDialog = new MineFromJavaDialog(this, true);
        javaMinerDialog.setVisible(true);

        if (javaMinerDialog.mineStatus) {
            // <editor-fold desc="set project information">
            String projectName = javaMinerDialog.mineProjectName;
            String taskName = javaMinerDialog.mineTaskName;
            String projectLocation = javaMinerDialog.mineProjectLocation;
            String support = javaMinerDialog.mineSupport + "";
            String confidence = javaMinerDialog.mineConfidence + "";
            String density = javaMinerDialog.mineDensity + "";
            String mineType = javaMinerDialog.mineType;
            String mineTime = javaMinerDialog.mineTime;
            String mineRuntime = javaMinerDialog.mineRuntime + "s";
            ArrayList<LSC> lscs = javaMinerDialog.mineLSC;
            LSCEvent[][] dataset = javaMinerDialog.mineDataSet;
            // </editor-fold>

            // <editor-fold desc="create lsc table">
            displayMineResult(projectName, taskName, projectLocation, support, confidence, density, mineType, mineTime, mineRuntime, lscs);
            displayDataset(dataset);
            // </editor-fold>

            /* update history tree */
            updateHistoryTree(new Project(projectName, projectLocation), taskName);
        }
}//GEN-LAST:event_mineJavaProgramButtonActionPerformed

    private void mineBinaryProgramButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_mineBinaryProgramButtonActionPerformed
      MineFromBinaryDialog binaryMinerDialog = new MineFromBinaryDialog(this, true);
      binaryMinerDialog.setVisible(true);

      if (binaryMinerDialog.mineStatus) {
            // <editor-fold desc="set project information">
            String projectName = binaryMinerDialog.mineProjectName;
            String taskName = binaryMinerDialog.mineTaskName;
            String projectLocation = binaryMinerDialog.mineProjectLocation;
            String support = binaryMinerDialog.mineSupport + "";
            String confidence = binaryMinerDialog.mineConfidence + "";
            String density = binaryMinerDialog.mineDensity + "";
            String mineType = binaryMinerDialog.mineType;
            String mineTime = binaryMinerDialog.mineTime;
            String mineRuntime = binaryMinerDialog.mineRuntime + "s";
            ArrayList<LSC> lscs = binaryMinerDialog.mineLSC;
            LSCEvent[][] dataset = binaryMinerDialog.mineDataSet;
            // </editor-fold>

            // <editor-fold desc="create lsc table">
            displayMineResult(projectName, taskName, projectLocation, support, confidence, density, mineType, mineTime, mineRuntime, lscs);
            displayDataset(dataset);
            // </editor-fold>

            /* update history tree */
            updateHistoryTree(new Project(projectName, projectLocation), taskName);
        }
}//GEN-LAST:event_mineBinaryProgramButtonActionPerformed

    private void settingButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_settingButtonActionPerformed
      PreferenceDialog prefDialog = new PreferenceDialog(this, true);
      prefDialog.setVisible(true);
}//GEN-LAST:event_settingButtonActionPerformed

    private void taskNameTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_taskNameTextFieldActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_taskNameTextFieldActionPerformed

    private void openProjectButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openProjectButtonActionPerformed
        JFileChooser openProjectChooser = new JFileChooser();
        openProjectChooser.setCurrentDirectory(new File("projects"));
        openProjectChooser.setDialogTitle("Select Project");
        openProjectChooser.setApproveButtonText("Select");
        openProjectChooser.setApproveButtonToolTipText("Select an already created LSC project");
        openProjectChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        openProjectChooser.setAcceptAllFileFilterUsed(false);
        openProjectChooser.addChoosableFileFilter(new ProjectFolderFilter());

        if (openProjectChooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            File taskFolder = openProjectChooser.getSelectedFile();
            if (!ProjectFolderFilter.isLSCTaskFolder(taskFolder)) {
                JOptionPane.showMessageDialog(this, "Invalid LSC task folder. Please select another one!", "Error Message", JOptionPane.ERROR_MESSAGE);
            } else {
                File projectFolder = taskFolder.getParentFile();
                Project project = new Project(projectFolder.getName(), projectFolder.getPath());
                String taskName = taskFolder.getName();
                ProjectParser projectParser = new ProjectParser();
                projectParser.parse(project, taskName);

                /* display mine result */
                displayMineResult(projectParser.projectName,
                        projectParser.taskName,
                        projectParser.projectLocation,
                        projectParser.support,
                        projectParser.confidence,
                        projectParser.density,
                        projectParser.mineType,
                        projectParser.mineTime,
                        projectParser.mineRuntime,
                        projectParser.lscs);

                /* update history tree */
                updateHistoryTree(project, taskName);

                /* record into history file */
                File historyFile = new File(getHistoryFileLocation());
                try {
                    PrintWriter hisPw = new PrintWriter(new FileOutputStream(historyFile, true));
                    hisPw.println(projectParser.projectName);
                    hisPw.println(projectParser.projectLocation);
                    hisPw.close();
                } catch (IOException exp) {
                }
            }
        }
    }//GEN-LAST:event_openProjectButtonActionPerformed

    private void miningRuntimeTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miningRuntimeTextFieldActionPerformed
      // TODO add your handling code here:
}//GEN-LAST:event_miningRuntimeTextFieldActionPerformed

    private void miningTypeTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miningTypeTextFieldActionPerformed
      // TODO add your handling code here:
}//GEN-LAST:event_miningTypeTextFieldActionPerformed

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JLabel confidenceLabel;
  private javax.swing.JTextField confidenceTextField;
  private javax.swing.JLabel densityLabel;
  private javax.swing.JTextField densityTextField;
  private javax.swing.JPanel historyPanel;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JScrollPane jScrollPane5;
  private javax.swing.JSeparator jSeparator1;
  private javax.swing.JSeparator jSeparator2;
  private javax.swing.JTable lscTable;
  private javax.swing.JPanel mainPanel;
  private javax.swing.JTabbedPane mainTabbedPane;
  private javax.swing.JButton mineBinaryProgramButton;
  private javax.swing.JButton mineFromTracesButton;
  private javax.swing.JButton mineJavaProgramButton;
  private javax.swing.JPanel miningParametePane;
  private javax.swing.JLabel miningRuntimeLabel;
  private javax.swing.JTextField miningRuntimeTextField;
  private javax.swing.JLabel miningTimeLabel;
  private javax.swing.JTextField miningTimeTextField;
  private javax.swing.JLabel miningTypeLabel;
  private javax.swing.JTextField miningTypeTextField;
  private javax.swing.JButton openProjectButton;
  private javax.swing.JLabel projectLocationLabel;
  private javax.swing.JLabel projectLocationLabel2;
  private javax.swing.JTextField projectLocationTextField;
  private javax.swing.JTextField projectLocationTextField2;
  private javax.swing.JLabel projectNameLabel;
  private javax.swing.JLabel projectNameLabel2;
  private javax.swing.JTextField projectNameTextField;
  private javax.swing.JTextField projectNameTextField2;
  private javax.swing.JTree projectTree;
  private javax.swing.JLabel recentProjectLabel;
  private javax.swing.JButton settingButton;
  private javax.swing.JLabel supportLabel;
  private javax.swing.JTextField supportTextField;
  private javax.swing.JLabel taskNameLabel;
  private javax.swing.JLabel taskNameLabel2;
  private javax.swing.JTextField taskNameTextField;
  private javax.swing.JTextField taskNameTextField2;
  private javax.swing.JSeparator topSeparator;
  private javax.swing.JTabbedPane traceTabbedPane;
  // End of variables declaration//GEN-END:variables
}

package lscminer.gui;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.table.*;
import lscminer.datastructure.*;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class LSCCellEditor extends AbstractCellEditor implements TableCellEditor {

  int lscInx;
  Object cellValue;
  ArrayList<LSC> lscs;
  JButton button;

  public LSCCellEditor(ArrayList<LSC> lscs) {
    /* Set up the editor (from the table's point of view), which is a button.
    This button brings up the color chooselsc displayr dialog, which is the editor from the user's point of view. */
    this.lscs = lscs;
    button = new JButton();
    button.addActionListener(new LSCDrawer());
  }

  public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) {
    cellValue = value;
    lscInx = (Integer)table.getValueAt(row, 0)-1;
    return button;
  }

  public Object getCellEditorValue() {
    return cellValue; // do not change cell value
  }

  public class LSCDrawer implements ActionListener{
    public void actionPerformed(ActionEvent e) {
//      JOptionPane.showMessageDialog(button, lscs.get(lscInx), "LSC Content", JOptionPane.PLAIN_MESSAGE);
      VisualLSCDialog lscDialog = new VisualLSCDialog(null, lscs.get(lscInx));
      lscDialog.setTitle("LSC# " + (lscInx+1));
      lscDialog.setVisible(true);
      fireEditingStopped();
    }
  }
}

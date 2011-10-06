package hub.top.editor.eclipse.ui;

import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.transaction.TransactionalEditingDomain;

public class ConvertDiagramFileAction_FCtoPTnetDiag extends ConvertDiagramFileAction {

  /**
   * Create a {@link ConvertDiagramFileAction_FCtoPTnetDiag} to convert a selected
   * Flowchart diagram to a PNML document.
   * 
   * @see hub.top.editor.eclipse.ui.ConvertDiagramFileAction#createConvertFileWizard(org.eclipse.emf.common.util.URI, org.eclipse.emf.ecore.EObject, org.eclipse.emf.transaction.TransactionalEditingDomain)
   */
  @Override
  public ConvertFileWizard createConvertFileWizard(URI srcURI, Object root,
      TransactionalEditingDomain editingDomain)
  {
    ConvertFileWizard wizard = new ConvertFileWizard_FCtoPTnetDiag(
        srcURI, root, editingDomain, "ptnet");
    wizard.setWindowTitle("Convert Flowchart to P/T Net Diagram");
    return wizard;
  }

}

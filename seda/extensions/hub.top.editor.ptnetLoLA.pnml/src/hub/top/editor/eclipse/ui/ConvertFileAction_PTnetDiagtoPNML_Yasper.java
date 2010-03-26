package hub.top.editor.eclipse.ui;

import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.transaction.TransactionalEditingDomain;

public class ConvertFileAction_PTnetDiagtoPNML_Yasper extends ConvertDiagramFileAction {

    /**
     * Create a {@link ConvertFileWizard_PNMLtoPT} to convert a selected
     * Flowchart diagram to a PNML document.
     * 
     * @see hub.top.editor.eclipse.ui.ConvertDiagramFileAction#createConvertFileWizard(org.eclipse.emf.common.util.URI, org.eclipse.emf.ecore.EObject, org.eclipse.emf.transaction.TransactionalEditingDomain)
     */
    @Override
    public ConvertFileWizard createConvertFileWizard(URI srcURI, Object root,
        TransactionalEditingDomain editingDomain)
    {
      ConvertFileWizard wizard = new ConvertFileWizard_PTnetDiagtoPNML_Yasper(
          srcURI, root, editingDomain, "pnml");
      wizard.setWindowTitle("Convert P/T Net Diagram to PNML (Yasper)");
      return wizard;
    }

}

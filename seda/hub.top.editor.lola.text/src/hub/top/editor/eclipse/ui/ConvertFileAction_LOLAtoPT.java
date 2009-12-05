package hub.top.editor.eclipse.ui;

import hub.top.editor.eclipse.FileIOHelper;

import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.transaction.TransactionalEditingDomain;

public class ConvertFileAction_LOLAtoPT extends ConvertFileAction {

 
  /**
   * Create a {@link ConvertFileWizard_LOLAtoPT} to convert a selected
   * Flowchart diagram to a PNML document.
   * 
   * @see hub.top.editor.eclipse.ui.ConvertDiagramFileAction#createConvertFileWizard(org.eclipse.emf.common.util.URI, org.eclipse.emf.ecore.EObject, org.eclipse.emf.transaction.TransactionalEditingDomain)
   */
  @Override
  public ConvertFileWizard createConvertFileWizard(URI srcURI, Object root,
      TransactionalEditingDomain editingDomain)
  {
    ConvertFileWizard wizard = new ConvertFileWizard_LOLAtoPT(
        srcURI, root, editingDomain, "ptnet");
    wizard.setWindowTitle("Convert LoLA to P/T net");
    return wizard;
  }
  
  @Override
  protected Object getModelObject(TransactionalEditingDomain editingDomain,
      URI uri) {
    
    IPath path = new Path(uri.toPlatformString(true));
    return FileIOHelper.readFile(path);
  }

}

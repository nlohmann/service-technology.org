package hub.top.editor.ui.actions;

import hub.top.editor.eclipse.EditorUtil;
import hub.top.editor.eclipse.IFrameWorkEditor;
import hub.top.editor.eclipse.PluginHelper;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditor;
import hub.top.editor.ptnetLoLA.transaction.PtnetLoLAEditingDomainFactory;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.IHandler;
import org.eclipse.core.commands.IHandlerListener;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.parts.DiagramDocumentEditor;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

public class AnonymizeModel extends AbstractHandler implements IHandler {

	public Object execute(ExecutionEvent event) throws ExecutionException {
		IWorkbenchWindow window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
		PluginHelper plugin = hub.top.editor.Activator.getPluginHelper();
		try
		{
			/*
			if (!(window.getActivePage().getActiveEditor() instanceof
					hub.top.editor.eclipse.IFrameWorkEditor ))
				throw new Exception("Anonymize: action works in ServiceTechnology.org framework editors only.");
			
			// retrieve and prepare editor utils
			EditorUtil sourceEditor = ((IFrameWorkEditor)window.getActivePage().getActiveEditor()).getEditorUtil();
			EObject obj = sourceEditor.getCurrentResource().getContents().get(0);
			
			System.out.println(obj);
			*/
			IEditorPart editor = window.getActivePage().getActiveEditor();
			
			Resource res = null;
			PtNet net = null;
			EditorUtil sourceEditor = null;
			
			if (editor instanceof DiagramDocumentEditor) {
				DiagramDocumentEditor diagEdit = (DiagramDocumentEditor)editor;
				net = (PtNet)diagEdit.getDiagram().getElement();
				res = diagEdit.getDiagram().eResource();
			} else if (editor instanceof hub.top.editor.eclipse.IFrameWorkEditor) {
				sourceEditor = ((IFrameWorkEditor)window.getActivePage().getActiveEditor()).getEditorUtil();
				net = (PtNet) sourceEditor.getCurrentRootModel();
				res = net.eResource();
			}
			
			

			if (net != null) {
				TransactionalEditingDomain editDomain = PtnetLoLAEditingDomainFactory.INSTANCE.getEditingDomain(net.eResource().getResourceSet());
				if (editDomain == null)
					editDomain = GMFEditingDomainFactory.INSTANCE.createEditingDomain();
				
				CompoundCommand cc = new CompoundCommand();
				int placeNum = 0;
				for (Place p : net.getPlaces()) {
					String name = "p"+placeNum;
					SetCommand set = new SetCommand(editDomain, p, PtnetLoLAPackage.eINSTANCE.getNode_Name(), name);
					cc.append(set);
					placeNum++;
				}
				int transitionNum = 0;
				for (Transition t : net.getTransitions()) {
					String name = "t"+transitionNum;
					SetCommand set = new SetCommand(editDomain, t, PtnetLoLAPackage.eINSTANCE.getNode_Name(), name);
					cc.append(set);
					transitionNum++;
				}
				editDomain.getCommandStack().execute(cc);
				return res;
			}
			
			if (sourceEditor != null) {
				sourceEditor.refreshEditorView();
			}
			
			return null;
		}	
		catch (Exception exception) {
			plugin.logError(exception);
			MessageDialog.openInformation(window.getShell(),
					"Anonymize", "We're sorry, we could not anonymize the model. See the error log for details.");
		}
		return null;
	}



}

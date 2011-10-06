package hub.top.editor.ptnetLoLA.diagram.navigator;

import java.util.Iterator;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.ui.URIEditorInput;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.workspace.util.WorkspaceSynchronizer;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gmf.runtime.diagram.ui.parts.DiagramEditor;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.document.IDiagramDocument;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.navigator.ILinkHelper;
import org.eclipse.ui.part.FileEditorInput;

/**
 * @generated
 */
public class PtnetLoLANavigatorLinkHelper implements ILinkHelper {

	/**
	 * @generated
	 */
	private static IEditorInput getEditorInput(Diagram diagram) {
		Resource diagramResource = diagram.eResource();
		for (Iterator it = diagramResource.getContents().iterator(); it.hasNext();) {
			EObject nextEObject = (EObject) it.next();
			if (nextEObject == diagram) {
				return new FileEditorInput(WorkspaceSynchronizer
						.getFile(diagramResource));
			}
			if (nextEObject instanceof Diagram) {
				break;
			}
		}
		URI uri = EcoreUtil.getURI(diagram);
		String editorName = uri.lastSegment()
				+ "#" + diagram.eResource().getContents().indexOf(diagram); //$NON-NLS-1$
		IEditorInput editorInput = new URIEditorInput(uri, editorName);
		return editorInput;
	}

	/**
	 * @generated
	 */
	public IStructuredSelection findSelection(IEditorInput anInput) {
		IDiagramDocument document = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
				.getInstance().getDocumentProvider().getDiagramDocument(anInput);
		if (document == null) {
			return StructuredSelection.EMPTY;
		}
		Diagram diagram = document.getDiagram();
		IFile file = WorkspaceSynchronizer.getFile(diagram.eResource());
		if (file != null) {
			hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem item = new hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem(
					diagram, file, false);
			return new StructuredSelection(item);
		}
		return StructuredSelection.EMPTY;
	}

	/**
	 * @generated
	 */
	public void activateEditor(IWorkbenchPage aPage,
			IStructuredSelection aSelection) {
		if (aSelection == null || aSelection.isEmpty()) {
			return;
		}
		if (false == aSelection.getFirstElement() instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLAAbstractNavigatorItem) {
			return;
		}

		hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLAAbstractNavigatorItem abstractNavigatorItem = (hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLAAbstractNavigatorItem) aSelection
				.getFirstElement();
		View navigatorView = null;
		if (abstractNavigatorItem instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) {
			navigatorView = ((hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) abstractNavigatorItem)
					.getView();
		} else if (abstractNavigatorItem instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorGroup) {
			hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorGroup navigatorGroup = (hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorGroup) abstractNavigatorItem;
			if (navigatorGroup.getParent() instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) {
				navigatorView = ((hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) navigatorGroup
						.getParent()).getView();
			}
		}
		if (navigatorView == null) {
			return;
		}
		IEditorInput editorInput = getEditorInput(navigatorView.getDiagram());
		IEditorPart editor = aPage.findEditor(editorInput);
		if (editor == null) {
			return;
		}
		aPage.bringToTop(editor);
		if (editor instanceof DiagramEditor) {
			DiagramEditor diagramEditor = (DiagramEditor) editor;
			ResourceSet diagramEditorResourceSet = diagramEditor.getEditingDomain()
					.getResourceSet();
			EObject selectedView = diagramEditorResourceSet.getEObject(EcoreUtil
					.getURI(navigatorView), true);
			if (selectedView == null) {
				return;
			}
			GraphicalViewer graphicalViewer = (GraphicalViewer) diagramEditor
					.getAdapter(GraphicalViewer.class);
			EditPart selectedEditPart = (EditPart) graphicalViewer
					.getEditPartRegistry().get(selectedView);
			if (selectedEditPart != null) {
				graphicalViewer.select(selectedEditPart);
			}
		}
	}

}

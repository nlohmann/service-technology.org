package hub.top.editor.ptnetLoLA.diagram.part;

import java.util.HashMap;

import hub.top.editor.eclipse.EditorUtil;
import hub.top.editor.ptnetLoLA.transaction.PtnetLoLAEditingDomainFactory;
import hub.top.editor.ptnetLoLA.util.PtnetLoLAValidator;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.emf.common.ui.URIEditorInput;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.xmi.XMLResource;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.emf.workspace.util.WorkspaceSynchronizer;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gmf.runtime.common.ui.resources.IBookmark;
import org.eclipse.gmf.runtime.common.ui.services.marker.MarkerNavigationService;
import org.eclipse.gmf.runtime.diagram.core.preferences.PreferencesHint;
import org.eclipse.gmf.runtime.diagram.ui.actions.ActionIds;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.document.IDiagramDocument;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.document.IDocument;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.document.IDocumentProvider;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.parts.DiagramDocumentEditor;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IMessageProvider;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.osgi.util.NLS;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorMatchingStrategy;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.SaveAsDialog;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.navigator.resources.ProjectExplorer;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.IShowInTargetList;
import org.eclipse.ui.part.ShowInContext;

/**
 * @generated
 */
public class PtnetLoLADiagramEditor extends DiagramDocumentEditor implements
		IGotoMarker, hub.top.editor.eclipse.IFrameWorkEditor {

	/**
	 * @generated
	 */
	public static final String ID = "hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorID"; //$NON-NLS-1$

	/**
	 * @generated
	 */
	public static final String CONTEXT_ID = "hub.top.editor.ptnetLoLA.diagram.ui.diagramContext"; //$NON-NLS-1$

	/**
	 * @generated
	 */
	public PtnetLoLADiagramEditor() {
		super(true);
	}

	/**
	 * @generated
	 */
	protected String getContextID() {
		return CONTEXT_ID;
	}

	/**
	 * @generated
	 */
	protected PaletteRoot createPaletteRoot(PaletteRoot existingPaletteRoot) {
		PaletteRoot root = super.createPaletteRoot(existingPaletteRoot);
		new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAPaletteFactory()
				.fillPalette(root);
		return root;
	}

	/**
	 * @generated
	 */
	protected PreferencesHint getPreferencesHint() {
		return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin.DIAGRAM_PREFERENCES_HINT;
	}

	/**
	 * @generated
	 */
	public String getContributorId() {
		return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin.ID;
	}

	/**
	 * @generated
	 */
	public Object getAdapter(Class type) {
		if (type == IShowInTargetList.class) {
			return new IShowInTargetList() {
				public String[] getShowInTargetIds() {
					return new String[] { ProjectExplorer.VIEW_ID };
				}
			};
		}
		return super.getAdapter(type);
	}

	/**
	 * @generated
	 */
	protected IDocumentProvider getDocumentProvider(IEditorInput input) {
		if (input instanceof IFileEditorInput || input instanceof URIEditorInput) {
			return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
					.getInstance().getDocumentProvider();
		}
		return super.getDocumentProvider(input);
	}

	/**
	 * this editor operates on the ptnetlola editing domain
	 * @return id for the ptnetlola editing domain
	 * @generated not 
	 */
	@Override
	protected String getEditingDomainID() {
		return PtnetLoLAEditingDomainFactory.EDITING_DOMAIN_ID;
	}

	/**
	 * @generated
	 */
	public TransactionalEditingDomain getEditingDomain() {
		IDocument document = getEditorInput() != null ? getDocumentProvider()
				.getDocument(getEditorInput()) : null;
		if (document instanceof IDiagramDocument) {
			return ((IDiagramDocument) document).getEditingDomain();
		}
		return super.getEditingDomain();
	}

	/**
	 * @generated
	 */
	protected void setDocumentProvider(IEditorInput input) {
		if (input instanceof IFileEditorInput || input instanceof URIEditorInput) {
			setDocumentProvider(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
					.getInstance().getDocumentProvider());
		} else {
			super.setDocumentProvider(input);
		}
	}

	/**
	 * @generated
	 */
	public void gotoMarker(IMarker marker) {
		MarkerNavigationService.getInstance().gotoMarker(this, marker);
	}

	/**
	 * @generated
	 */
	public boolean isSaveAsAllowed() {
		return true;
	}

	@Override
	public void doSave(IProgressMonitor progressMonitor) {
		super.doSave(progressMonitor);
		

	}
	
	/**
	 * @generated
	 */
	public void doSaveAs() {
		performSaveAs(new NullProgressMonitor());
	}

	/**
	 * @generated
	 */
	protected void performSaveAs(IProgressMonitor progressMonitor) {
		Shell shell = getSite().getShell();
		IEditorInput input = getEditorInput();
		SaveAsDialog dialog = new SaveAsDialog(shell);
		IFile original = input instanceof IFileEditorInput ? ((IFileEditorInput) input)
				.getFile()
				: null;
		if (original != null) {
			dialog.setOriginalFile(original);
		}
		dialog.create();
		IDocumentProvider provider = getDocumentProvider();
		if (provider == null) {
			// editor has been programmatically closed while the dialog was open
			return;
		}
		if (provider.isDeleted(input) && original != null) {
			String message = NLS
					.bind(
							hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLADiagramEditor_SavingDeletedFile,
							original.getName());
			dialog.setErrorMessage(null);
			dialog.setMessage(message, IMessageProvider.WARNING);
		}
		if (dialog.open() == Window.CANCEL) {
			if (progressMonitor != null) {
				progressMonitor.setCanceled(true);
			}
			return;
		}
		IPath filePath = dialog.getResult();
		if (filePath == null) {
			if (progressMonitor != null) {
				progressMonitor.setCanceled(true);
			}
			return;
		}
		IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
		IFile file = workspaceRoot.getFile(filePath);
		final IEditorInput newInput = new FileEditorInput(file);
		// Check if the editor is already open
		IEditorMatchingStrategy matchingStrategy = getEditorDescriptor()
				.getEditorMatchingStrategy();
		IEditorReference[] editorRefs = PlatformUI.getWorkbench()
				.getActiveWorkbenchWindow().getActivePage().getEditorReferences();
		for (int i = 0; i < editorRefs.length; i++) {
			if (matchingStrategy.matches(editorRefs[i], newInput)) {
				MessageDialog
						.openWarning(
								shell,
								hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLADiagramEditor_SaveAsErrorTitle,
								hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLADiagramEditor_SaveAsErrorMessage);
				return;
			}
		}
		boolean success = false;
		try {
			provider.aboutToChange(newInput);
			getDocumentProvider(newInput).saveDocument(progressMonitor, newInput,
					getDocumentProvider().getDocument(getEditorInput()), true);
			success = true;
		} catch (CoreException x) {
			IStatus status = x.getStatus();
			if (status == null || status.getSeverity() != IStatus.CANCEL) {
				ErrorDialog
						.openError(
								shell,
								hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLADiagramEditor_SaveErrorTitle,
								hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLADiagramEditor_SaveErrorMessage,
								x.getStatus());
			}
		} finally {
			provider.changed(newInput);
			if (success) {
				setInput(newInput);
			}
		}
		if (progressMonitor != null) {
			progressMonitor.setCanceled(!success);
		}
	}
	
	/**
	 * Extended setInput method with validation upon loading
	 * (non-Javadoc)
	 * @see org.eclipse.gmf.runtime.diagram.ui.resources.editor.parts.DiagramDocumentEditor#setInput(org.eclipse.ui.IEditorInput)
	 */
	@Override
	public void setInput(IEditorInput input) {
		super.setInput(input);
		validateDocument();
	}
	
	/**
	 * Validate document after successful saving.
	 * 
	 * @see org.eclipse.gmf.runtime.diagram.ui.resources.editor.parts.DiagramDocumentEditor#editorSaved()
	 */
	@Override
	protected void editorSaved() {
		super.editorSaved();
		validateDocument();
	}


	/**
	 * @generated
	 */
	public ShowInContext getShowInContext() {
		return new ShowInContext(getEditorInput(), getNavigatorSelection());
	}

	/**
	 * @generated NOT
	 */
	private ISelection getNavigatorSelection() {
		IDiagramDocument document = getDiagramDocument();
		if (document == null) {
			return StructuredSelection.EMPTY;
		}
		Diagram diagram = document.getDiagram();
// <Dirk.F> added check for empty eResource
		if (diagram == null || diagram.eResource() == null)
			return StructuredSelection.EMPTY;
// </Dirk.F>
		
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
	protected void configureGraphicalViewer() {
		super.configureGraphicalViewer();
		hub.top.editor.ptnetLoLA.diagram.part.DiagramEditorContextMenuProvider provider = new hub.top.editor.ptnetLoLA.diagram.part.DiagramEditorContextMenuProvider(
				this, getDiagramGraphicalViewer());
		getDiagramGraphicalViewer().setContextMenu(provider);
		getSite().registerContextMenu(ActionIds.DIAGRAM_EDITOR_CONTEXT_MENU,
				provider, getDiagramGraphicalViewer());
	}

	public EditorUtil getEditorUtil() {
		// TODO Auto-generated method stub
		return new hub.top.editor.petrinets.diagram.eclipse.PtnetLoLADiagramEditorUtil(this);
	}
	
	/**
	 * Validate current document of the editor
	 */
	public void validateDocument () {
		IResource resource = (IResource)getEditorInput().getAdapter(IResource.class);

		try {
			resource.deleteMarkers(IMarker.PROBLEM, true, IResource.DEPTH_INFINITE);
			resource.deleteMarkers(IBookmark.TYPE, true, IResource.DEPTH_INFINITE);
			
			if (getDiagram() != null) {
				Diagram d = getDiagram();
				
				// Validate individual nodes of the diagram
				for (Object o : d.getChildren()) {
					View childView = (View)o;

					XMLResource res = null;
					if (childView.eResource() instanceof XMLResource)
						res = (XMLResource)childView.eResource();

					HashMap attribMap = new HashMap();

					// validate places
					if (childView.getElement() instanceof hub.top.editor.ptnetLoLA.Place) {
						hub.top.editor.ptnetLoLA.Place p = (hub.top.editor.ptnetLoLA.Place)childView.getElement();
						if (!PtnetLoLAValidator.validate(p)) {
							
							/* -- bookmark marker pointing to the node
							marker = resource.createMarker(IBookmark.TYPE);
							attribMap.put(IBookmark.ELEMENT_ID, res.getID(v));
							marker.setAttributes(attribMap);
							*/
							
							attribMap.put(IMarker.MESSAGE,  "Place name incompatible with LoLA file format.");
							attribMap.put(IMarker.SEVERITY, IMarker.SEVERITY_WARNING);
							if (res != null) attribMap.put(IMarker.SOURCE_ID, res.getID(childView));
							attribMap.put(IMarker.LOCATION, "place '"+p.getName()+"'");
						}
					}
					
					// validate transitions
					if (childView.getElement() instanceof hub.top.editor.ptnetLoLA.Transition) {
						hub.top.editor.ptnetLoLA.Transition t = (hub.top.editor.ptnetLoLA.Transition)childView.getElement();
						if (!PtnetLoLAValidator.validate(t)) {
							
							/* -- bookmark marker pointing to the node
							marker = resource.createMarker(IBookmark.TYPE);
							attribMap.put(IBookmark.ELEMENT_ID, res.getID(v));
							marker.setAttributes(attribMap);
							*/
							
							attribMap.put(IMarker.MESSAGE,  "Transition name incompatible with LoLA file format.");
							attribMap.put(IMarker.SEVERITY, IMarker.SEVERITY_WARNING);
							if (res != null) attribMap.put(IMarker.SOURCE_ID, res.getID(childView));
							attribMap.put(IMarker.LOCATION, "transition '"+t.getName()+"'");
						}
					}
					// collected attributes for a marker, so create one
					if (attribMap.size() > 0) {
						IMarker marker;
						marker = resource.createMarker(IMarker.PROBLEM);
						marker.setAttributes(attribMap);
					}
				} // end of validating individual nodes
			}
		} catch (Exception e) {
			PtnetLoLADiagramEditorPlugin.getInstance().logError("Error", e);
		}
	}

}

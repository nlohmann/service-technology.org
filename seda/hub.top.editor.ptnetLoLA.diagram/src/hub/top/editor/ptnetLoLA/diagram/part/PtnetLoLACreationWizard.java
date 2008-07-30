package hub.top.editor.ptnetLoLA.diagram.part;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.WorkspaceModifyOperation;

/**
 * @generated
 */
public class PtnetLoLACreationWizard extends Wizard implements INewWizard {

	/**
	 * @generated
	 */
	private IWorkbench workbench;

	/**
	 * @generated
	 */
	protected IStructuredSelection selection;

	/**
	 * @generated
	 */
	protected hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLACreationWizardPage diagramModelFilePage;

	/**
	 * @generated
	 */
	protected hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLACreationWizardPage domainModelFilePage;

	/**
	 * @generated
	 */
	protected Resource diagram;

	/**
	 * @generated
	 */
	private boolean openNewlyCreatedDiagramEditor = true;

	/**
	 * @generated
	 */
	public IWorkbench getWorkbench() {
		return workbench;
	}

	/**
	 * @generated
	 */
	public IStructuredSelection getSelection() {
		return selection;
	}

	/**
	 * @generated
	 */
	public final Resource getDiagram() {
		return diagram;
	}

	/**
	 * @generated
	 */
	public final boolean isOpenNewlyCreatedDiagramEditor() {
		return openNewlyCreatedDiagramEditor;
	}

	/**
	 * @generated
	 */
	public void setOpenNewlyCreatedDiagramEditor(
			boolean openNewlyCreatedDiagramEditor) {
		this.openNewlyCreatedDiagramEditor = openNewlyCreatedDiagramEditor;
	}

	/**
	 * @generated
	 */
	public void init(IWorkbench workbench, IStructuredSelection selection) {
		this.workbench = workbench;
		this.selection = selection;
		setWindowTitle(hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLACreationWizardTitle);
		setDefaultPageImageDescriptor(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
				.getBundledImageDescriptor("icons/wizban/NewPtnetLoLAWizard.gif")); //$NON-NLS-1$
		setNeedsProgressMonitor(true);
	}

	/**
	 * @generated
	 */
	public void addPages() {
		diagramModelFilePage = new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLACreationWizardPage(
				"DiagramModelFile", getSelection(), "ptnetlola_diagram"); //$NON-NLS-1$ //$NON-NLS-2$
		diagramModelFilePage
				.setTitle(hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLACreationWizard_DiagramModelFilePageTitle);
		diagramModelFilePage
				.setDescription(hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLACreationWizard_DiagramModelFilePageDescription);
		addPage(diagramModelFilePage);

		domainModelFilePage = new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLACreationWizardPage(
				"DomainModelFile", getSelection(), "ptnetlola"); //$NON-NLS-1$ //$NON-NLS-2$
		domainModelFilePage
				.setTitle(hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLACreationWizard_DomainModelFilePageTitle);
		domainModelFilePage
				.setDescription(hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLACreationWizard_DomainModelFilePageDescription);
		addPage(domainModelFilePage);
	}

	/**
	 * @generated
	 */
	public boolean performFinish() {
		IRunnableWithProgress op = new WorkspaceModifyOperation(null) {

			protected void execute(IProgressMonitor monitor)
					throws CoreException, InterruptedException {
				diagram = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorUtil
						.createDiagram(diagramModelFilePage.getURI(),
								domainModelFilePage.getURI(), monitor);
				if (isOpenNewlyCreatedDiagramEditor() && diagram != null) {
					try {
						hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorUtil
								.openDiagram(diagram);
					} catch (PartInitException e) {
						ErrorDialog
								.openError(
										getContainer().getShell(),
										hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLACreationWizardOpenEditorError,
										null, e.getStatus());
					}
				}
			}
		};
		try {
			getContainer().run(false, true, op);
		} catch (InterruptedException e) {
			return false;
		} catch (InvocationTargetException e) {
			if (e.getTargetException() instanceof CoreException) {
				ErrorDialog
						.openError(
								getContainer().getShell(),
								hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLACreationWizardCreationError,
								null, ((CoreException) e.getTargetException())
										.getStatus());
			} else {
				hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
						.getInstance()
						.logError(
								"Error creating diagram", e.getTargetException()); //$NON-NLS-1$
			}
			return false;
		}
		return diagram != null;
	}
}

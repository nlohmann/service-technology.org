package hub.top.adaptiveSystem.diagram.part;

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
public class AdaptiveSystemCreationWizard extends Wizard implements INewWizard {

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
	protected hub.top.adaptiveSystem.diagram.part.AdaptiveSystemCreationWizardPage diagramModelFilePage;

	/**
	 * @generated
	 */
	protected hub.top.adaptiveSystem.diagram.part.AdaptiveSystemCreationWizardPage domainModelFilePage;

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
		setWindowTitle(hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveSystemCreationWizardTitle);
		setDefaultPageImageDescriptor(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
				.getBundledImageDescriptor("icons/wizban/NewAdaptiveSystemWizard.gif")); //$NON-NLS-1$
		setNeedsProgressMonitor(true);
	}

	/**
	 * @generated
	 */
	public void addPages() {
		diagramModelFilePage = new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemCreationWizardPage(
				"DiagramModelFile", getSelection(), "adaptivesystem_diagram"); //$NON-NLS-1$ //$NON-NLS-2$
		diagramModelFilePage
				.setTitle(hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveSystemCreationWizard_DiagramModelFilePageTitle);
		diagramModelFilePage
				.setDescription(hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveSystemCreationWizard_DiagramModelFilePageDescription);
		addPage(diagramModelFilePage);

		domainModelFilePage = new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemCreationWizardPage(
				"DomainModelFile", getSelection(), "adaptivesystem") { //$NON-NLS-1$ //$NON-NLS-2$

			public void setVisible(boolean visible) {
				if (visible) {
					String fileName = diagramModelFilePage.getFileName();
					fileName = fileName.substring(0, fileName.length()
							- ".adaptivesystem_diagram".length()); //$NON-NLS-1$
					setFileName(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorUtil
							.getUniqueFileName(getContainerFullPath(),
									fileName, "adaptivesystem")); //$NON-NLS-1$
				}
				super.setVisible(visible);
			}
		};
		domainModelFilePage
				.setTitle(hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveSystemCreationWizard_DomainModelFilePageTitle);
		domainModelFilePage
				.setDescription(hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveSystemCreationWizard_DomainModelFilePageDescription);
		addPage(domainModelFilePage);
	}

	/**
	 * @generated
	 */
	public boolean performFinish() {
		IRunnableWithProgress op = new WorkspaceModifyOperation(null) {

			protected void execute(IProgressMonitor monitor)
					throws CoreException, InterruptedException {
				diagram = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorUtil
						.createDiagram(diagramModelFilePage.getURI(),
								domainModelFilePage.getURI(), monitor);
				if (isOpenNewlyCreatedDiagramEditor() && diagram != null) {
					try {
						hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorUtil
								.openDiagram(diagram);
					} catch (PartInitException e) {
						ErrorDialog
								.openError(
										getContainer().getShell(),
										hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveSystemCreationWizardOpenEditorError,
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
								hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveSystemCreationWizardCreationError,
								null, ((CoreException) e.getTargetException())
										.getStatus());
			} else {
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.getInstance()
						.logError(
								"Error creating diagram", e.getTargetException()); //$NON-NLS-1$
			}
			return false;
		}
		return diagram != null;
	}
}

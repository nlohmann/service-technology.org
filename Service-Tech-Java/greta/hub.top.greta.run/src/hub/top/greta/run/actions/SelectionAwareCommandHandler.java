package hub.top.greta.run.actions;

import java.util.Map;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.commands.ICommandService;
import org.eclipse.ui.commands.IElementUpdater;
import org.eclipse.ui.menus.UIElement;

public abstract class SelectionAwareCommandHandler extends AbstractHandler
	implements ISelectionListener, IElementUpdater {

	/**
	 * The constructor.
	 */
	public SelectionAwareCommandHandler() {
		getWorkbenchWindow().getSelectionService().addSelectionListener(this);
	}

	private IWorkbenchWindow fWindow;
	
	/**
	 * Returns the active workbench window.
	 *
	 * @return the active workbench window or <code>null</code> if not available
	 */
	protected final IWorkbenchWindow getWorkbenchWindow() {
		if (fWindow == null)
			fWindow= PlatformUI.getWorkbench().getActiveWorkbenchWindow();
		return fWindow;
	}
	
	/**
	 * Returns the selection of the active workbench window.
	 *
	 * @return the current selection in the active workbench window or <code>null</code>
	 */
	protected final ISelection getSelection() {
		IWorkbenchWindow window = getWorkbenchWindow();
		if (window != null) {
			return window.getSelectionService().getSelection();
		}
		return null;
	}
	
	@Override
	public void selectionChanged(IWorkbenchPart part, ISelection selection) {
	}
	
	@Override
	public void updateElement(UIElement element, @SuppressWarnings("rawtypes") Map parameters) {
	}

	protected abstract String getID();
	
	protected void requestRefresh() {
		ICommandService commandService = (ICommandService) getWorkbenchWindow().getService(ICommandService.class);
		if (commandService != null) {
			commandService.refreshElements(getID(), null);
		}
	}
}

package hub.top.greta.run.actions;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart;
import hub.top.greta.verification.BuildBP;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSys_AdaptiveSystem;
import hub.top.uma.InvalidModelException;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IActionDelegate;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;

public class CheckExecutableEvent implements IObjectActionDelegate {

	private Shell shell;
	private Event selectedEvent = null;
	private AdaptiveSystem selectedSystem = null;
	
	/**
	 * Constructor for Action1.
	 */
	public CheckExecutableEvent() {
		super();
	}

	/**
	 * @see IObjectActionDelegate#setActivePart(IAction, IWorkbenchPart)
	 */
	public void setActivePart(IAction action, IWorkbenchPart targetPart) {
		shell = targetPart.getSite().getShell();
	}

	/**
	 * @see IActionDelegate#run(IAction)
	 */
	public void run(IAction action) {
		if (selectedSystem == null || selectedEvent == null)
		  return;
		
		DNodeRefold bp;
		try {
		  bp = BuildBP.init(selectedSystem);
		} catch (InvalidModelException e) {
		  ActionHelper.showMessageDialogue(MessageDialog.ERROR,
		      "Check executable event", "Failed to check executability of "+selectedEvent.getName()+". "+e.getMessage());
		  return;
		}
		DNode eventToCheck = ((DNodeSys_AdaptiveSystem)bp.getSystem()).getResultNode(selectedEvent);
		bp.getOptions().configure_checkExecutable(eventToCheck);
		
		final BuildBP build = new BuildBP(bp);
		
    Job bpBuildJob = new Job("Checking executability of '"+selectedEvent.getName()+"'") 
    {
      @Override
      protected IStatus run(IProgressMonitor monitor) {
        
        monitor.beginTask("constructing branching process", IProgressMonitor.UNKNOWN);
        
        // build branching process
        boolean interrupted = !build.run(monitor, System.out);
        if (!interrupted) {
          if (build.getBranchingProcess().canExecuteEvent()) {
            ActionHelper.showMessageDialogue(MessageDialog.INFORMATION, 
                "Greta Adaptive System Runtime Environment",
                "Event "+selectedEvent.getName()+" is executable");             
            
          } else {
            ActionHelper.showMessageDialogue(MessageDialog.INFORMATION, 
                "Greta Adaptive System Runtime Environment",
                "Event "+selectedEvent.getName()+" cannot be executed");          }
        }

        monitor.done();
        
        /*
        if (interrupted)
          return Status.CANCEL_STATUS;
        else*/
          return Status.OK_STATUS;
      }
    };
    bpBuildJob.setUser(true);
    bpBuildJob.schedule();
	}

	/**
	 * @see IActionDelegate#selectionChanged(IAction, ISelection)
	 */
	public void selectionChanged(IAction action, ISelection selection) {
    if (selection instanceof IStructuredSelection == false
        || selection.isEmpty()) {
      return;
    }
    Object o = ((IStructuredSelection) selection).getFirstElement();
    if (o instanceof EventDoNetEditPart) {
      EventDoNetEditPart ep = (EventDoNetEditPart)o;
      if (ep.getModel() != null && ep.getModel() instanceof org.eclipse.gmf.runtime.notation.Node) {
        org.eclipse.gmf.runtime.notation.Node node = (org.eclipse.gmf.runtime.notation.Node)ep.getModel();
        if (node.getElement() != null && node.getElement() instanceof Event) {
          Event e = (Event)node.getElement();
          selectedEvent = e;
          
          if (   e.eContainer() != null // net
              && e.eContainer().eContainer() != null // oclet
              && e.eContainer().eContainer().eContainer() != null // system
              && e.eContainer().eContainer().eContainer() instanceof AdaptiveSystem
              )
          {
            selectedSystem = (AdaptiveSystem)e.eContainer().eContainer().eContainer();
          } else {
            selectedSystem = null;
          }
          return;
        }
      }
    }
    selectedEvent = null;
    selectedSystem = null;
	}

}

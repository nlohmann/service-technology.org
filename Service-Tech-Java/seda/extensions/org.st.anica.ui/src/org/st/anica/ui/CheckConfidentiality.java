package org.st.anica.ui;

import hub.top.editor.eclipse.EditorUtil;
import hub.top.editor.eclipse.IFrameWorkEditor;
import hub.top.editor.eclipse.emf.EMFHelper;
import hub.top.editor.lola.text.ModelEditor;
import hub.top.editor.ptnetLoLA.Confidence;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.editor.ptnetLoLA.TransitionExt;

import java.io.IOException;

import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.IHandler;
import org.eclipse.core.commands.IHandlerListener;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.util.EContentAdapter;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;
import org.json.JSONException;
import org.json.JSONObject;
import org.st.anica.ui.preferences.PreferenceConstants;

import sunlabs.brazil.util.Base64;

public class CheckConfidentiality implements IHandler {

  @Override
  public void addHandlerListener(IHandlerListener handlerListener) {
  }

  @Override
  public void dispose() {
  }
  
  public static class BridgeToReasonerState {
    private PtNet previousNet = null;
    private boolean confidenceChanged = false;
    
    {
      System.out.println("new state");
    }
    
    public synchronized PtNet getPreviousNet() {
      return previousNet;
    }
    public synchronized void setPreviousNet(PtNet previousNet) {
      this.previousNet = previousNet;
    }
    public synchronized boolean isConfidenceChanged() {
      return confidenceChanged;
    }
    public synchronized void setConfidenceChanged(boolean confidenceChanged) {
      this.confidenceChanged = confidenceChanged;
    }
  }
  
  private final BridgeToReasonerState state = new BridgeToReasonerState();
  
  private void checkNet(PtNet net, IWorkbenchWindow window, EditingDomain domain) {
    
    CheckConfidentiality_Job job = new CheckConfidentiality_Job("Check Confidentiality of Transitions", state, net, domain);
    job.setUser(true);
    job.schedule();
  }
  
  @Override
  public Object execute(ExecutionEvent event) throws ExecutionException {

    IWorkbenchWindow window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
    
    if (window.getActivePage().getActiveEditor() instanceof hub.top.editor.eclipse.IFrameWorkEditor ) {
      
      EditorUtil util = ((IFrameWorkEditor)window.getActivePage().getActiveEditor()).getEditorUtil();
      System.out.println(util);
      for (EObject o : util.getCurrentModel()) {
        if (o instanceof org.eclipse.gmf.runtime.notation.Diagram) {
          Diagram d = (Diagram)o;
          if (d.getElement() instanceof PtNet) {
            PtNet net = (PtNet)d.getElement();

            EditingDomain domain = TransactionalEditingDomain.Factory.INSTANCE.getEditingDomain(net.eResource().getResourceSet());
            checkNet(net, window, domain);
            util.refreshEditorView();
          }
        } else if (o instanceof PtNet) {
          
          PtNet net = (PtNet)o;
          
          // get an editing domain to execute the commands in
          EditingDomain domain;
          domain = TransactionalEditingDomain.Registry.INSTANCE.getEditingDomain("hub.top.editor.ptnetLoLA.editingDomain");
          if (domain == null) domain = TransactionalEditingDomain.Factory.INSTANCE.getEditingDomain(net.eResource().getResourceSet());
          if (domain == null) domain = EMFHelper.getEditingDomainFor(net);
          if (domain == null) domain = TransactionalEditingDomain.Factory.INSTANCE.createEditingDomain(net.eResource().getResourceSet());
          //if (domain == null) domain = TransactionalEditingDomain.Factory.INSTANCE.createEditingDomain(net.eResource().getResourceSet());

          checkNet((PtNet)o, window, domain);
          util.refreshEditorView();
        }
      }

    }
    
    
    return null;
  }

  @Override
  public boolean isEnabled() {
    // TODO Auto-generated method stub
    return true;
  }

  @Override
  public boolean isHandled() {
    // TODO Auto-generated method stub
    return true;
  }

  @Override
  public void removeHandlerListener(IHandlerListener handlerListener) {
    // TODO Auto-generated method stub

  }

}

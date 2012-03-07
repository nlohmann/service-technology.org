package org.st.anica.ui;

import hub.top.editor.eclipse.EditorUtil;
import hub.top.editor.eclipse.IFrameWorkEditor;
import hub.top.editor.lola.text.ModelEditor;
import hub.top.editor.ptnetLoLA.Confidence;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.editor.ptnetLoLA.TransitionExt;

import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.IHandler;
import org.eclipse.core.commands.IHandlerListener;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EContentAdapter;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;
import org.json.JSONException;
import org.json.JSONObject;

import sunlabs.brazil.util.Base64;

public class CheckConfidence implements IHandler {

  @Override
  public void addHandlerListener(IHandlerListener handlerListener) {
    // TODO Auto-generated method stub

  }

  @Override
  public void dispose() {
    // TODO Auto-generated method stub

  }
  
  private PtNet petriNet = null;
  private boolean confidenceChanged = false;
  
  private void checkNet(PtNet net) {
    
    if (petriNet == null) {

      petriNet = net;
      EContentAdapter  adapter = new EContentAdapter () {
        
        public void notifyChanged(Notification notification) {
          
          if (notification.getFeatureID(TransitionExt.class) == PtnetLoLAPackage.TRANSITION_EXT__CONFIDENCE) {
            //System.out.println("set confidence");
            confidenceChanged = true;
          } else {
            //System.out.println("Net on server invalid");
            petriNet = null;
          }
        }
      };
      petriNet.eAdapters().add(adapter);
      
      System.out.println("send new net");
      
      String lola_src = ModelEditor.getText(petriNet);
      String lola_base64 = Base64.encode(lola_src);
      
      JSONObject j_net = new JSONObject();
      try {
        j_net.put("command", "net");
        j_net.put("net", lola_base64);
      } catch (JSONException e) {
        // TODO Auto-generated catch block
        e.printStackTrace();
      }
      
      System.out.println(j_net.toString());
      
      confidenceChanged = true;
    }
    
    if (confidenceChanged) {
      
      JSONObject j_assignment = new JSONObject();
      try {
        j_assignment.put("command", "assignment");
        for (Transition t : petriNet.getTransitions()) {
          TransitionExt t_ext = (TransitionExt)t;
          if (t_ext.getConfidence() == Confidence.HIGH) {
            j_assignment.put(t_ext.getName(), "HIGH");
          } else if (t_ext.getConfidence() == Confidence.LOW) {
            j_assignment.put(t_ext.getName(), "LOW");
          } else {
            j_assignment.put(t_ext.getName(), "");
          }

        }
      } catch (JSONException e) {
        // TODO Auto-generated catch block
        e.printStackTrace();
      }
      
      System.out.println(j_assignment.toString());
      
      confidenceChanged = false;
    }
    
  }

  @Override
  public Object execute(ExecutionEvent event) throws ExecutionException {

    IWorkbenchWindow window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
    
    if (window.getActivePage().getActiveEditor() instanceof hub.top.editor.eclipse.IFrameWorkEditor ) {
      
      EditorUtil util = ((IFrameWorkEditor)window.getActivePage().getActiveEditor()).getEditorUtil();
      for (EObject o : util.getCurrentModel()) {
        if (o instanceof org.eclipse.gmf.runtime.notation.Diagram) {
          Diagram d = (Diagram)o;
          if (d.getElement() instanceof PtNet) {
            PtNet net = (PtNet)d.getElement();
            
            checkNet(net);
          }
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

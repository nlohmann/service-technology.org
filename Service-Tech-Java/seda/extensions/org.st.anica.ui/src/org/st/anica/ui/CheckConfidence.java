package org.st.anica.ui;

import java.io.IOException;

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
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.util.EContentAdapter;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
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
  
  public static int MY_PORT = 5555;
  public static int SERVER_PORT = 5556;
  public static String SERVER_URL = "localhost";
  
  private PtNet previousNet = null;
  private boolean confidenceChanged = false;
  
  private void checkNet(PtNet net) {
    
    try {
      UDPClient client = null;

      // see if the current net has been checked before (net is still on the server)
      // no: set to null and resend net
      if (previousNet != null && net != previousNet) previousNet = null;
      if (previousNet == null) {
        // send a new net to the server
        client = new UDPClient(SERVER_URL, SERVER_PORT);

        // give the current net an adapter to watch for changes the require to send the net again
        previousNet = net;
        EContentAdapter  adapter = new EContentAdapter () {
          public void notifyChanged(Notification notification) {
            if (notification.getFeatureID(TransitionExt.class) == PtnetLoLAPackage.TRANSITION_EXT__CONFIDENCE) {
              // confidence changed: check confidence again
              confidenceChanged = true;
            } else {
              // net changed: load net again
              previousNet = null;
            }
          }
        };
        previousNet.eAdapters().add(adapter);
        
        System.out.println("send new net");
        
        // encode new net in Base-64
        String lola_src = ModelEditor.getText(previousNet);
        String lola_base64 = Base64.encode(lola_src);
        // and put it in a JSON object
        JSONObject j_net = new JSONObject();
        try {
          j_net.put("command", "net");
          j_net.put("net", lola_base64);
        } catch (JSONException e) {
          Activator.getPluginHelper().logError("Could not create command to server.", e);
          return;
        }
        // send the JSON object to the server
        System.out.println(j_net.toString());
        client.send(j_net.toString());
        
        // and also check confidence of transitions 
        confidenceChanged = true;
      }
      
      if (confidenceChanged) {
        // confidence has changed or net is new: check consistency of current confidence values
        // we need to send an assignment to the server
        if (client == null) client = new UDPClient(SERVER_URL, SERVER_PORT);
        // get current assignment from the net and put it into a JSON object
        JSONObject j_assignment = new JSONObject();
        try {
          j_assignment.put("command", "assignment");
          for (Transition t : previousNet.getTransitions()) {
            if (t instanceof TransitionExt) {
              // transitions with a confidence value
              TransitionExt t_ext = (TransitionExt)t;
              if (t_ext.getConfidence() == Confidence.HIGH) {
                j_assignment.put(t_ext.getName(), "HIGH");
              } else if (t_ext.getConfidence() == Confidence.LOW) {
                j_assignment.put(t_ext.getName(), "LOW");
              } else {
                j_assignment.put(t_ext.getName(), "");
              }
            } else {
              // transitions without a confidence value
              j_assignment.put(t.getName(), "");
            }
  
          }
        } catch (JSONException e) {
          Activator.getPluginHelper().logError("Could not create command to server.", e);
          return;
        }
        // send the assignment
        System.out.println(j_assignment.toString());
        client.send(j_assignment.toString());
        
        // and open a socket to wait for the reply: a new assignment
        UDPServer server = new UDPServer(MY_PORT);
        String reply = server.receive();
        
        System.out.println("reply: "+reply);
        
        try {
          // if the reply is a new assignment
          JSONObject j_reply = new JSONObject(reply);
          if (j_reply.has("command") && j_reply.get("command").equals("assignment")) {
            // update confidence values of transitions by the new assignment
            updateConfidenceInNet(net, j_reply);
          }
          
        } catch (JSONException e) {
          Activator.getPluginHelper().logError("Could not read message from server.", e);
          return;
        }
        server.finalize(); // done
        
        // and we are done until someone changes confidence values again
        confidenceChanged = false;
      }
      // close the client
      if (client != null) client.finalize();
      
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Could not connects to server.", e);
    }
  }
  
  /**
   * Update the confidence values in the net to the given assignment.
   * 
   * @param net
   * @param assignment
   */
  private void updateConfidenceInNet(PtNet net, JSONObject assignment) {
    
    // to update transition attributes, we need to execute commands
    // list of all commands, each command sets one transition attribute
    EList<org.eclipse.emf.common.command.Command> cmdList = new BasicEList<org.eclipse.emf.common.command.Command>();
    // the feature that command will change: transition confidence
    EStructuralFeature featConfidence = PtnetLoLAPackage.eINSTANCE.getTransitionExt_Confidence();
    // get an editing domain to execute the commands in
    EditingDomain domain = TransactionalEditingDomain.Factory.INSTANCE.getEditingDomain(net.eResource().getResourceSet());
    
    // for each transition of the net
    for (Transition t : net.getTransitions()) {
      if (t instanceof TransitionExt) {
        TransitionExt t_ext = (TransitionExt)t;
        // get its new confidence value from the assignment
        try {
          String newVal = assignment.getString(t_ext.getName());
          
          // and create a command to set the value
          org.eclipse.emf.edit.command.SetCommand cmd = new org.eclipse.emf.edit.command.SetCommand(
              domain, t_ext, featConfidence, Confidence.get(newVal));
          cmd.setLabel("set confidence on " + t_ext.getName());
          cmdList.add(cmd);
          
        } catch (JSONException e) {
          Activator.getPluginHelper().logError("Could not read value for "+t.getName(), e);
        }
      }
    }
    
    // join all set commands in a compond command and execute this
    org.eclipse.emf.common.command.CompoundCommand fireCmd = new org.eclipse.emf.common.command.CompoundCommand(
        cmdList);
    fireCmd.setLabel("update confidences");
    domain.getCommandStack().execute(fireCmd);
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

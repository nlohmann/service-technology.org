/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
 * The Original Code is this file as it was released on June 6, 2009.
 * The Initial Developer of the Original Code are
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package hub.top.greta.run.actions;

import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemFactory;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Oclet;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class InstantiateSystem implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.GRETA.run.instantiateSystem";
	
	private IWorkbenchWindow workbenchWindow;

	private AdaptiveSystem adaptiveSystem;
	
	public void dispose() {
		// TODO Auto-generated method stub

	}

	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}
	
	/**
	 * Parse 'name' for a list of parameters (Param1,...,ParamK) and
	 * return Param1 .. ParamK in a list.
	 * 
	 * @param name
	 * @return
	 */
	public static LinkedList<String> getParameters(String name) {
	  LinkedList<String> names = new LinkedList<String>();
    int paramsStart = name.indexOf('(')+1;
    int paramsEnd = name.indexOf(')');
    
    if (paramsStart == 0 || paramsEnd == -1)
      return names; // this system cannot be instantiated

    int pBegin = paramsStart;
    for (int pEnd=paramsStart; pEnd<paramsEnd; pEnd++) {
      if (name.charAt(pEnd) != ',') continue;
      names.add(name.substring(pBegin,pEnd));
      pBegin = pEnd + 1;
    }
    // don't forget the last parameter in the list
    names.add(name.substring(pBegin,paramsEnd));
    
    return names;
	}
	
	/**
	 * Find all parameters in a system.
	 * 
	 * @param system
	 * @return
	 */
	public static HashSet<String> getParameters(AdaptiveSystem system) {
	  HashSet<String> names = new HashSet<String>();
	  // collect all parameters in all oclet names
	  for (Oclet o : system.getOclets()) {
	    String name = o.getName();
	    names.addAll(getParameters(name));
	  }
	  
	  return names;
	}

	public void run(IAction action) {
		if (!action.getId().equals(InstantiateSystem.ID))
			return;
		
		IEditorPart editor = workbenchWindow.getActivePage().getActiveEditor();
		adaptiveSystem = ActionHelper.getAdaptiveSystem(editor);
		
		if (adaptiveSystem == null)
			return;
			
		if (!(editor.getEditorInput() instanceof IFileEditorInput))
			return;
		
		HashSet<String> parameters = getParameters(adaptiveSystem);
		System.out.println(parameters);
		
		
		IFile in = ((IFileEditorInput)editor.getEditorInput()).getFile();
		
		String fileName = in.getName();
		 /*
		int paramStart = fileName.indexOf('(')+1;
		int paramEnd = fileName.indexOf(')');
		
		if (paramStart == 0 || paramEnd == -1)
			return;	// this system cannot be instantiated
		
		String parameter = fileName.substring(paramStart,paramEnd);
		//System.out.println("instantiating "+fileName+", parameter: "+parameter);
		 */
		
    IInputValidator intValidator = new IInputValidator() {
      
      public String isValid(String newText) {
        if (newText == null || newText.length() == 0)
          return " ";
        try {
          int val = Integer.parseInt(newText);
          if (val < Integer.MIN_VALUE) return "Number out of bounds.";
          if (val > Integer.MAX_VALUE) return "Number out of bounds.";
          return null;
        } catch (NumberFormatException e) {
          return "Please enter an integer number.";
        }
      }
    };

		
		HashMap<String, Integer> paramMin = new HashMap<String, Integer>();
		HashMap<String, Integer> paramMax = new HashMap<String, Integer>();
		
		for (String parameter : parameters) {
	
  		InputDialog minDiag = new InputDialog(workbenchWindow.getShell(),
  				"Instantiate parameter '"+parameter+"' in "+fileName,
  				"minimal value for '"+parameter+"'", "1", intValidator);
  		if (minDiag.open() == InputDialog.CANCEL)
  			return;
  		
  		InputDialog maxDiag = new InputDialog(workbenchWindow.getShell(), 
  				"Instantiate parameter '"+parameter+"' in "+fileName,
  				"maximal value for '"+parameter+"'", "1", intValidator);
  		if (maxDiag.open() == InputDialog.CANCEL)
  			return;
  		
  		paramMin.put(parameter, Integer.parseInt(minDiag.getValue()));
  		paramMax.put(parameter, Integer.parseInt(maxDiag.getValue()));
		}
	
		// instantiate system for all specified parameters
		AdaptiveSystem inst = instantiateSystem(adaptiveSystem, parameters, paramMin, paramMax);

		// stored instantiated system, generate appropriate file name
		// try to instantiate file name from parameters
		LinkedList<String> paramList = new LinkedList<String>(parameters);
		String modelName = in.getFullPath().removeFileExtension().toString();
		String targetPathInst = instantiate(modelName, paramList, getMaxVal(paramList, paramMax), paramMin, paramMax);

		// if name was not instantiated by parameter values 
		if (targetPathInst.equals(modelName)) {
		  // add generic name by time
		  targetPathInst += "_inst_"+(int)(System.currentTimeMillis()/1000);
		}
		
		targetPathInst += ".adaptivesystem";
		System.out.println("write to "+targetPathInst);
		ActionHelper.writeEcoreResourceToFile(workbenchWindow, URI.createFileURI(targetPathInst), inst);

	}
	
	/**
	 * @param adaptiveSystem
	 * @param parameter      set of declared parameters
	 * @param paramRange_min mapping assigning each declared parameter its minimal value
	 * @param paramRange_max mapping assigning each declared parameter its maximal value
	 * @return
	 */
	public static AdaptiveSystem instantiateSystem(AdaptiveSystem adaptiveSystem, HashSet<String> parameter, Map<String, Integer> paramRange_min, Map<String, Integer> paramRange_max) {
    AdaptiveSystem inst = AdaptiveSystemFactory.eINSTANCE.createAdaptiveSystem();
    AdaptiveProcess ap = AdaptiveSystemFactory.eINSTANCE.createAdaptiveProcess();
    ap.setName("ap");

    // copy all nodes of the adaptive process (modeling the initial state of the system)
    if (adaptiveSystem.getAdaptiveProcess() != null) {
      for (Node n : adaptiveSystem.getAdaptiveProcess().getNodes()) {
        LinkedList<String> nParameters = getParameters(n.getName());
        nParameters.retainAll(parameter);   // only those parameters that are declared
        
        if (!nParameters.isEmpty()) {
          
          // generate all parameter combinations
          int paramValues[][] = generateParamValues(nParameters, paramRange_min, paramRange_max);
          
          // create an instance of each node for each parameter value
          for (int i = 0; i < paramValues.length; i++) {
            EcoreUtil.Copier copier = new EcoreUtil.Copier();
            Node nInst = (Node)copier.copy(n);
            copier.copyReferences();
            
            nInst.setName(instantiate(nInst.getName(), nParameters, paramValues[i], paramRange_min, paramRange_max));
            
            ap.getNodes().add(nInst);
          }

        } else {
          // create a single copy of the node
          EcoreUtil.Copier copier = new EcoreUtil.Copier();
          Node nInst = (Node)copier.copy(n);
          copier.copyReferences();
          ap.getNodes().add(nInst);
        }
      }
      inst.setAdaptiveProcess(ap);
    }
    
    // copy all oclets of the adaptive system
    for (Oclet o : adaptiveSystem.getOclets()) {
      LinkedList<String> oParameters = getParameters(o.getName());
      oParameters.retainAll(parameter);   // only those parameters that are declared
      
      if (!oParameters.isEmpty()) {
        
        System.out.println("oclet "+o.getName());
        
        // generate all parameter combinations
        int paramValues[][] = generateParamValues(oParameters, paramRange_min, paramRange_max);

        // create an instance of each oclet for each parameter value
        for (int i = 0; i < paramValues.length; i++) {
          EcoreUtil.Copier copier = new EcoreUtil.Copier();
          Oclet oInst = (Oclet)copier.copy(o);
          copier.copyReferences();

          oInst.setName(instantiate(oInst.getName(), oParameters, paramValues[i], paramRange_min, paramRange_max));
          
          // instantiate all nodes by instantiating their names
          for (Node n : oInst.getPreNet().getNodes()) {
            n.setName(instantiate(n.getName(), oParameters, paramValues[i], paramRange_min, paramRange_max));
          }
          for (Node n : oInst.getDoNet().getNodes()) {
            n.setName(instantiate(n.getName(), oParameters, paramValues[i], paramRange_min, paramRange_max));
          }
          
          inst.getOclets().add(oInst);
        }
      } else {
        // create a single copy of the oclet without instantiating
        EcoreUtil.Copier copier = new EcoreUtil.Copier();
        Oclet oInst = (Oclet)copier.copy(o);
        copier.copyReferences();
        inst.getOclets().add(oInst);
      }
    }
    return inst;
	}
	
	 /**
   * @param adaptiveSystem
   * @param parameter
   * @param paramRange_min
   * @param paramRange_max
   * @return
   */
  public static AdaptiveSystem instantiateSystem(AdaptiveSystem adaptiveSystem, String parameter, int paramRange_min, int paramRange_max) {
    
    HashSet<String> parameters = new HashSet<String>();
    HashMap<String, Integer> min = new HashMap<String, Integer>(); 
    HashMap<String, Integer> max = new HashMap<String, Integer>();
    parameters.add(parameter);
    min.put(parameter, paramRange_min);
    max.put(parameter, paramRange_max);
    
    return instantiateSystem(adaptiveSystem, parameters, min, max);
    
    /*
    AdaptiveSystem inst = AdaptiveSystemFactory.eINSTANCE.createAdaptiveSystem();
    AdaptiveProcess ap = AdaptiveSystemFactory.eINSTANCE.createAdaptiveProcess();
    ap.setName("ap");

    // copy all nodes of the adaptive process (modeling the initial state of the system)
    for (Node n : adaptiveSystem.getAdaptiveProcess().getNodes()) {
      
      if (isParameterized(n.getName(), parameter)) {
        
        // create an instance of each node for each parameter value
        for (int p = paramRange_min; p <= paramRange_max; p++) {
          EcoreUtil.Copier copier = new EcoreUtil.Copier();
          Node nInst = (Node)copier.copy(n);
          copier.copyReferences();
          
          nInst.setName(instantiate(nInst.getName(), parameter, p, paramRange_min, paramRange_max));
          
          ap.getNodes().add(nInst);
        }
      } else {
        // create a single copy of the node
        EcoreUtil.Copier copier = new EcoreUtil.Copier();
        Node nInst = (Node)copier.copy(n);
        copier.copyReferences();
        ap.getNodes().add(nInst);
      }
    }
    inst.setAdaptiveProcess(ap);
    
    // copy all oclets of the adaptive system
    for (Oclet o : adaptiveSystem.getOclets()) {
      if (isParameterized(o.getName(), parameter)) {
        
        // create an instance of each oclet for each parameter value
        for (int p = paramRange_min; p <= paramRange_max; p++) {
          EcoreUtil.Copier copier = new EcoreUtil.Copier();
          Oclet oInst = (Oclet)copier.copy(o);
          copier.copyReferences();
          
          oInst.setName(instantiate(oInst.getName(), parameter, p, paramRange_min, paramRange_max));

          // instantiate all nodes by instantiating their names
          for (Node n : oInst.getPreNet().getNodes()) {
            String name = n.getName();
            if (!isParameterized(name, parameter))
              continue;
            
            n.setName(instantiate(name, parameter, p, paramRange_min, paramRange_max));
          }
          for (Node n : oInst.getDoNet().getNodes()) {
            String name = n.getName();
            if (!isParameterized(name, parameter))
              continue;
            
            n.setName(instantiate(name, parameter, p, paramRange_min, paramRange_max));
          }
          
          inst.getOclets().add(oInst);
        }
      } else {
        // create a single copy of the oclet without instantiating
        EcoreUtil.Copier copier = new EcoreUtil.Copier();
        Oclet oInst = (Oclet)copier.copy(o);
        copier.copyReferences();
        inst.getOclets().add(oInst);
      }
    }
    return inst;
    */
  }
  
  private static boolean isParameterized(String pattern, String param) {
    int paramStart = pattern.indexOf('(')+1;
    int paramEnd = pattern.indexOf(')');
    if (paramStart == 0 || paramEnd == -1)
      return false;
    
    String paramUse = pattern.substring(paramStart, paramEnd);

    return (paramUse.indexOf(param) != -1);
  }
	
	private static boolean moreToGo(int counter[], LinkedList<String> params, Map<String, Integer> paramRange_max) {
    for (int i=0;i<params.size(); i++) {
      if (counter[i] > paramRange_max.get(params.get(i)))
        return false;
    }
    return true;
	}
	
	/**
	 * @param params
	 * @param paramRange_max
	 * @return array of maximal values for the given parameters
	 */
	private static int[] getMaxVal (LinkedList<String> params, Map<String, Integer> paramRange_max) {
	  int paramValues[] = new int[params.size()];
	  for (int i=0; i<params.size();i++) {
	    paramValues[i] = paramRange_max.get(params.get(i));
	  }
	  return paramValues;
	}
	
	/**
	 * @param params
	 * @param paramRange_min
	 * @param paramRange_max
	 * @return array of all valid parameter combinations of the given parameters
	 */
	private static int[][] generateParamValues(LinkedList<String> params, Map<String, Integer> paramRange_min, Map<String, Integer> paramRange_max) {
	  
    int size = 1;
    // get number of all combinations for the given parameters
    for (String p : params) {
      size *= (paramRange_max.get(p)-paramRange_min.get(p)+1);
    }
    int paramValues[][] = new int[size][params.size()];
	  
    // initialize counter for parameter combination
	  int counter[] = new int[params.size()];
	  for (int i=0;i<params.size(); i++) {
	    counter[i] = paramRange_min.get(params.get(i));
	  }

	  // run counter from min to max for all parameter combinations
	  int valueIndex = 0;
	  while (true) {
	    // store next parameter value
	    for (int i=0;i<params.size(); i++) {
	      paramValues[valueIndex][i] = counter[i];
	    }
	    valueIndex++;
	    
	    // advance counter
	    int counterIndex = counter.length-1;
	    for (; counterIndex >= 0; counterIndex--) {
	      counter[counterIndex]++;
	      // counter at counterIndex still within range, done
	      if (counter[counterIndex] <= paramRange_max.get(params.get(counterIndex))) break;
	    }

	    // first counter iterated over all values, we are done
	    if (counterIndex == -1 && counter[0] > paramRange_max.get(params.get(0))) 
	      break;
	    
	    // not done yet: reset all counters before the incremented counter
	    else {
	      for (int i=counterIndex+1; i < counter.length; i++) {
	        counter[i] = paramRange_min.get(params.get(i));
	      }
	    }
	  }
	  return paramValues;
	}
	
	 private static String instantiate(String pattern, LinkedList<String> params, int values[], Map<String,Integer> min, Map<String,Integer> max) {
	    for (int i=0; i<params.size(); i++) {
	      pattern = instantiate(pattern, params.get(i), values[i], min.get(params.get(i)), max.get(params.get(i)));
	    }
	    return pattern;
	  }

	private static String instantiate(String pattern, String param, int value, int min, int max) {
		
	  // firstly, locate the list 'param1,..,paramK' of parameters in
	  // the pattern 'abc(param1,..,paramK)def'
	  int paramListStart = pattern.indexOf('(')+1;
	  int paramListEnd = pattern.indexOf(')');
	  
	  if (paramListStart == -1 || paramListEnd == -1)
	    return pattern;  // nothing to instantiate
		
		// secondly, locate 'param op val' in the parameterList
		// beginning with param and ending with a comma ',' or the end of the parameterList
		int paramRefBegin = pattern.indexOf(param, paramListStart);
		int paramRefEnd = pattern.indexOf(',', paramRefBegin);
		if (paramRefEnd == -1) paramRefEnd = paramListEnd;
		
		if (paramRefBegin == -1 || paramRefEnd == -1)
		  return pattern;  // 'param' not found in list
		
    System.out.print("instantiate: "+pattern);

		String paramUse = pattern.substring(paramRefBegin, paramRefEnd);
		
    // thirdly, identify use of '+' or '-' operators in the parameter use
		int addIndex = paramUse.indexOf('+'); 
		int subIndex = paramUse.indexOf('-');
		int opIndex = -1;
		if (addIndex != -1) opIndex = addIndex;
		else if (subIndex != -1) opIndex = subIndex;
		
		// if an operation is used, extract the second operand
		int diff = 0;
		if (opIndex != -1) {
			String first = paramUse.substring(0,opIndex);
			String second = paramUse.substring(opIndex+1);
			if (first.equals(param))
				diff = Integer.parseInt(second);
			else
				diff = Integer.parseInt(first);
		}
		
		// fourthly, instantiate parameter with proper value
		
    int a = value-min;  // shift value for proper modulo operation

    System.out.print(" "+paramUse+", "+param+":="+value+", diff="+diff);
		System.out.print(", a="+a);
		
		// and add/subtract modulo parameter range the value for instantiation
		if (diff != 0) {
			if (addIndex != -1) a = (a + diff) % max;
			if (subIndex != -1) {
				a = (a - diff);
				while (a < 0) a += max;
				a = a % max;
			}
		}
		System.out.print(", a'="+a);
		
		int instValue = a+min;	// shift result value back
		System.out.println(", "+paramUse+"="+instValue);
		
		// instValue contains proper value for 'parameter'
		// update pattern
		return pattern.substring(0, paramRefBegin) + instValue + pattern.substring(paramRefEnd);
	}

	public void selectionChanged(IAction action, ISelection selection) {
		// TODO Auto-generated method stub

	}

}

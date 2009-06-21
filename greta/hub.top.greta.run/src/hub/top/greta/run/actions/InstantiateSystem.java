/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on January 17, 2009.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

package hub.top.greta.run.actions;

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

	public void run(IAction action) {
		if (!action.getId().equals(InstantiateSystem.ID))
			return;
		
		IEditorPart editor = workbenchWindow.getActivePage().getActiveEditor();
		adaptiveSystem = ActionHelper.getAdaptiveSystem(editor);
		
		if (adaptiveSystem == null)
			return;
			
		if (!(editor.getEditorInput() instanceof IFileEditorInput))
			return;
		
		IFile in = ((IFileEditorInput)editor.getEditorInput()).getFile();
		
		String fileName = in.getName();
		int paramStart = fileName.indexOf('(')+1;
		int paramEnd = fileName.indexOf(')');
		
		if (paramStart == 0 || paramEnd == -1)
			return;	// this system cannot be instantiated
		
		String parameter = fileName.substring(paramStart,paramEnd);
		System.out.println("instantiating "+fileName+", parameter: "+parameter);
		
		
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
		
		int paramRange_min = Integer.parseInt(minDiag.getValue());
		int paramRange_max = Integer.parseInt(maxDiag.getValue());
	
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

		String modelName = in.getFullPath().removeFileExtension().toString();
		String targetPathInst = instantiate(modelName, parameter, paramRange_max, paramRange_min, paramRange_max);
		targetPathInst += ".adaptivesystem";
		System.out.println("write to "+targetPathInst);
		ActionHelper.writeEcoreResourceToFile(workbenchWindow, URI.createFileURI(targetPathInst), inst);
	}

	private boolean isParameterized(String pattern, String param) {
		int paramStart = pattern.indexOf('(')+1;
		int paramEnd = pattern.indexOf(')');
		if (paramStart == 0 || paramEnd == -1)
			return false;
		
		String paramUse = pattern.substring(paramStart, paramEnd);

		return (paramUse.indexOf(param) != -1);
	}
	
	private String instantiate(String pattern, String param, int value, int min, int max) {
		int paramStart = pattern.indexOf('(')+1;
		int paramEnd = pattern.indexOf(')');
		
		int a = value-min;	// shift value for proper module operation
		
		System.out.print("instantiate: "+pattern);
		
		// identify use of '+' or '-' operators in the parameter use
		String paramUse = pattern.substring(paramStart, paramEnd);
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
		
		// System.out.print(" "+paramUse+", "+param+":="+value+", diff="+diff);
		// System.out.print(", a="+a);
		
		// and add/subtract modulo parameter range the value for instantiation
		if (diff != 0) {
			if (addIndex != -1) a = (a + diff) % max;
			if (subIndex != -1) {
				a = (a - diff);
				while (a < 0) a += max;
				a = a % max;
			}
		}
		
		// System.out.print(", a'="+a);
		
		int instValue = a+min;	// shift result value back
		// System.out.println(", "+paramUse+"="+instValue);
		return pattern.substring(0, paramStart-1) + instValue + pattern.substring(paramEnd+1);
	}

	public void selectionChanged(IAction action, ISelection selection) {
		// TODO Auto-generated method stub

	}

}

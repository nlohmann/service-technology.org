/*
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 *  		Dirk Fahland
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
 */
package hub.top.lang.flowcharts.diagram.part;

import hub.top.lang.flowcharts.FlowPackage;

import org.eclipse.core.expressions.PropertyTester;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;

/**
 * @generated
 */
public class FlowchartDomainModelElementTester extends PropertyTester {

  /**
   * @generated
   */
  public boolean test(Object receiver, String method, Object[] args,
      Object expectedValue) {
    if (false == receiver instanceof EObject) {
      return false;
    }
    EObject eObject = (EObject) receiver;
    EClass eClass = eObject.eClass();
    if (eClass == FlowPackage.eINSTANCE.getWorkflowDiagram()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getDiagramNode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getControlFlowNode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getActivityNode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getResourceNode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getStartNode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getEndnode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getSimpleNode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getStructuredNode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getSplitNode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getMergeNode()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getFlow()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getSimpleActivity()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getSubprocess()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getWorkflow()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getPersistentResource()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getTransientResource()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getDocument()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getEvent()) {
      return true;
    }
    if (eClass == FlowPackage.eINSTANCE.getDiagramArc()) {
      return true;
    }
    return false;
  }

}

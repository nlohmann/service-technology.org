/**
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
 *
 * $Id$
 */
package hub.top.lang.flowcharts;

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see hub.top.lang.flowcharts.FlowPackage
 * @generated
 */
public interface FlowFactory extends EFactory {
  /**
	 * The singleton instance of the factory.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  FlowFactory eINSTANCE = hub.top.lang.flowcharts.impl.FlowFactoryImpl.init();

  /**
	 * Returns a new object of class '<em>Workflow Diagram</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Workflow Diagram</em>'.
	 * @generated
	 */
  WorkflowDiagram createWorkflowDiagram();

  /**
	 * Returns a new object of class '<em>Start Node</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Start Node</em>'.
	 * @generated
	 */
  StartNode createStartNode();

  /**
	 * Returns a new object of class '<em>Endnode</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Endnode</em>'.
	 * @generated
	 */
  Endnode createEndnode();

  /**
	 * Returns a new object of class '<em>Split Node</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Split Node</em>'.
	 * @generated
	 */
  SplitNode createSplitNode();

  /**
	 * Returns a new object of class '<em>Merge Node</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Merge Node</em>'.
	 * @generated
	 */
  MergeNode createMergeNode();

  /**
	 * Returns a new object of class '<em>Flow</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Flow</em>'.
	 * @generated
	 */
  Flow createFlow();

  /**
	 * Returns a new object of class '<em>Simple Activity</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Simple Activity</em>'.
	 * @generated
	 */
  SimpleActivity createSimpleActivity();

  /**
	 * Returns a new object of class '<em>Subprocess</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Subprocess</em>'.
	 * @generated
	 */
  Subprocess createSubprocess();

  /**
	 * Returns a new object of class '<em>Workflow</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Workflow</em>'.
	 * @generated
	 */
  Workflow createWorkflow();

  /**
	 * Returns a new object of class '<em>Persistent Resource</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Persistent Resource</em>'.
	 * @generated
	 */
  PersistentResource createPersistentResource();

  /**
	 * Returns a new object of class '<em>Transient Resource</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Transient Resource</em>'.
	 * @generated
	 */
  TransientResource createTransientResource();

  /**
	 * Returns a new object of class '<em>Document</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Document</em>'.
	 * @generated
	 */
  Document createDocument();

  /**
	 * Returns a new object of class '<em>Event</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Event</em>'.
	 * @generated
	 */
  Event createEvent();

  /**
	 * Returns a new object of class '<em>Diagram Arc</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Diagram Arc</em>'.
	 * @generated
	 */
  DiagramArc createDiagramArc();

  /**
	 * Returns the package supported by this factory.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the package supported by this factory.
	 * @generated
	 */
  FlowPackage getFlowPackage();

} //FlowFactory

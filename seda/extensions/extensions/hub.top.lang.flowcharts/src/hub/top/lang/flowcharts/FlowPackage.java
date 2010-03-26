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

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model.
 * It contains accessors for the meta objects to represent
 * <ul>
 *   <li>each class,</li>
 *   <li>each feature of each class,</li>
 *   <li>each enum,</li>
 *   <li>and each data type</li>
 * </ul>
 * <!-- end-user-doc -->
 * @see hub.top.lang.flowcharts.FlowFactory
 * @model kind="package"
 * @generated
 */
public interface FlowPackage extends EPackage {
  /**
	 * The package name.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  String eNAME = "flowcharts";

  /**
	 * The package namespace URI.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  String eNS_URI = "http://hu-berlin.de/flowchart";

  /**
	 * The package namespace name.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  String eNS_PREFIX = "flow";

  /**
	 * The singleton instance of the package.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  FlowPackage eINSTANCE = hub.top.lang.flowcharts.impl.FlowPackageImpl.init();

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl <em>Workflow Diagram</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.WorkflowDiagramImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getWorkflowDiagram()
	 * @generated
	 */
  int WORKFLOW_DIAGRAM = 0;

  /**
	 * The feature id for the '<em><b>Diagram Nodes</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__DIAGRAM_NODES = 0;

  /**
	 * The feature id for the '<em><b>Start Node</b></em>' reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__START_NODE = 1;

  /**
	 * The feature id for the '<em><b>End Node</b></em>' reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__END_NODE = 2;

  /**
	 * The feature id for the '<em><b>Process Output</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__PROCESS_OUTPUT = 3;

  /**
	 * The feature id for the '<em><b>Process Output Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__PROCESS_OUTPUT_OPT = 4;

  /**
	 * The feature id for the '<em><b>Diagram Arcs</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__DIAGRAM_ARCS = 5;

  /**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__NAME = 6;

  /**
	 * The feature id for the '<em><b>Process Input</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__PROCESS_INPUT = 7;

  /**
	 * The feature id for the '<em><b>Process Input Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__PROCESS_INPUT_OPT = 8;

  /**
	 * The feature id for the '<em><b>Process Calls</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM__PROCESS_CALLS = 9;

  /**
	 * The number of structural features of the '<em>Workflow Diagram</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_DIAGRAM_FEATURE_COUNT = 10;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.DiagramNodeImpl <em>Diagram Node</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.DiagramNodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getDiagramNode()
	 * @generated
	 */
  int DIAGRAM_NODE = 1;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DIAGRAM_NODE__LABEL = 0;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DIAGRAM_NODE__INCOMING = 1;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DIAGRAM_NODE__OUTGOING = 2;

  /**
	 * The number of structural features of the '<em>Diagram Node</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DIAGRAM_NODE_FEATURE_COUNT = 3;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.ControlFlowNodeImpl <em>Control Flow Node</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.ControlFlowNodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getControlFlowNode()
	 * @generated
	 */
  int CONTROL_FLOW_NODE = 2;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int CONTROL_FLOW_NODE__LABEL = DIAGRAM_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int CONTROL_FLOW_NODE__INCOMING = DIAGRAM_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int CONTROL_FLOW_NODE__OUTGOING = DIAGRAM_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Control Flow Node</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int CONTROL_FLOW_NODE_FEATURE_COUNT = DIAGRAM_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl <em>Activity Node</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.ActivityNodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getActivityNode()
	 * @generated
	 */
  int ACTIVITY_NODE = 3;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__LABEL = DIAGRAM_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__INCOMING = DIAGRAM_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__OUTGOING = DIAGRAM_NODE__OUTGOING;

  /**
	 * The feature id for the '<em><b>Activity Output</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__ACTIVITY_OUTPUT = DIAGRAM_NODE_FEATURE_COUNT + 0;

  /**
	 * The feature id for the '<em><b>Activity Output Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT = DIAGRAM_NODE_FEATURE_COUNT + 1;

  /**
	 * The feature id for the '<em><b>Optional</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__OPTIONAL = DIAGRAM_NODE_FEATURE_COUNT + 2;

  /**
	 * The feature id for the '<em><b>Activity Input Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__ACTIVITY_INPUT_OPT = DIAGRAM_NODE_FEATURE_COUNT + 3;

  /**
	 * The feature id for the '<em><b>Activity Input</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__ACTIVITY_INPUT = DIAGRAM_NODE_FEATURE_COUNT + 4;

  /**
	 * The feature id for the '<em><b>Min Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ACTIVITY_NODE__MIN_TIME = DIAGRAM_NODE_FEATURE_COUNT + 5;

		/**
	 * The feature id for the '<em><b>Cost</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__COST = DIAGRAM_NODE_FEATURE_COUNT + 6;

  /**
	 * The feature id for the '<em><b>Probability</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE__PROBABILITY = DIAGRAM_NODE_FEATURE_COUNT + 7;

  /**
	 * The feature id for the '<em><b>Max Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ACTIVITY_NODE__MAX_TIME = DIAGRAM_NODE_FEATURE_COUNT + 8;

		/**
	 * The number of structural features of the '<em>Activity Node</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ACTIVITY_NODE_FEATURE_COUNT = DIAGRAM_NODE_FEATURE_COUNT + 9;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.ResourceNodeImpl <em>Resource Node</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.ResourceNodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getResourceNode()
	 * @generated
	 */
  int RESOURCE_NODE = 4;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int RESOURCE_NODE__LABEL = DIAGRAM_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int RESOURCE_NODE__INCOMING = DIAGRAM_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int RESOURCE_NODE__OUTGOING = DIAGRAM_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Resource Node</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int RESOURCE_NODE_FEATURE_COUNT = DIAGRAM_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.SimpleNodeImpl <em>Simple Node</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.SimpleNodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getSimpleNode()
	 * @generated
	 */
  int SIMPLE_NODE = 7;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_NODE__LABEL = CONTROL_FLOW_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_NODE__INCOMING = CONTROL_FLOW_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_NODE__OUTGOING = CONTROL_FLOW_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Simple Node</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_NODE_FEATURE_COUNT = CONTROL_FLOW_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.StartNodeImpl <em>Start Node</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.StartNodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getStartNode()
	 * @generated
	 */
  int START_NODE = 5;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int START_NODE__LABEL = SIMPLE_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int START_NODE__INCOMING = SIMPLE_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int START_NODE__OUTGOING = SIMPLE_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Start Node</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int START_NODE_FEATURE_COUNT = SIMPLE_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.EndnodeImpl <em>Endnode</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.EndnodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getEndnode()
	 * @generated
	 */
  int ENDNODE = 6;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ENDNODE__LABEL = SIMPLE_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ENDNODE__INCOMING = SIMPLE_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ENDNODE__OUTGOING = SIMPLE_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Endnode</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ENDNODE_FEATURE_COUNT = SIMPLE_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.StructuredNodeImpl <em>Structured Node</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.StructuredNodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getStructuredNode()
	 * @generated
	 */
  int STRUCTURED_NODE = 8;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int STRUCTURED_NODE__LABEL = CONTROL_FLOW_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int STRUCTURED_NODE__INCOMING = CONTROL_FLOW_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int STRUCTURED_NODE__OUTGOING = CONTROL_FLOW_NODE__OUTGOING;

  /**
	 * The feature id for the '<em><b>Child Nodes</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int STRUCTURED_NODE__CHILD_NODES = CONTROL_FLOW_NODE_FEATURE_COUNT + 0;

  /**
	 * The number of structural features of the '<em>Structured Node</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int STRUCTURED_NODE_FEATURE_COUNT = CONTROL_FLOW_NODE_FEATURE_COUNT + 1;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.SplitNodeImpl <em>Split Node</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.SplitNodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getSplitNode()
	 * @generated
	 */
  int SPLIT_NODE = 9;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SPLIT_NODE__LABEL = SIMPLE_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SPLIT_NODE__INCOMING = SIMPLE_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SPLIT_NODE__OUTGOING = SIMPLE_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Split Node</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SPLIT_NODE_FEATURE_COUNT = SIMPLE_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.MergeNodeImpl <em>Merge Node</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.MergeNodeImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getMergeNode()
	 * @generated
	 */
  int MERGE_NODE = 10;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int MERGE_NODE__LABEL = SIMPLE_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int MERGE_NODE__INCOMING = SIMPLE_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int MERGE_NODE__OUTGOING = SIMPLE_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Merge Node</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int MERGE_NODE_FEATURE_COUNT = SIMPLE_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.FlowImpl <em>Flow</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.FlowImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getFlow()
	 * @generated
	 */
  int FLOW = 11;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int FLOW__LABEL = STRUCTURED_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int FLOW__INCOMING = STRUCTURED_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int FLOW__OUTGOING = STRUCTURED_NODE__OUTGOING;

  /**
	 * The feature id for the '<em><b>Child Nodes</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int FLOW__CHILD_NODES = STRUCTURED_NODE__CHILD_NODES;

  /**
	 * The feature id for the '<em><b>Start Node</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int FLOW__START_NODE = STRUCTURED_NODE_FEATURE_COUNT + 0;

  /**
	 * The feature id for the '<em><b>End Node</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int FLOW__END_NODE = STRUCTURED_NODE_FEATURE_COUNT + 1;

  /**
	 * The number of structural features of the '<em>Flow</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int FLOW_FEATURE_COUNT = STRUCTURED_NODE_FEATURE_COUNT + 2;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.SimpleActivityImpl <em>Simple Activity</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.SimpleActivityImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getSimpleActivity()
	 * @generated
	 */
  int SIMPLE_ACTIVITY = 12;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__LABEL = ACTIVITY_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__INCOMING = ACTIVITY_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__OUTGOING = ACTIVITY_NODE__OUTGOING;

  /**
	 * The feature id for the '<em><b>Activity Output</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__ACTIVITY_OUTPUT = ACTIVITY_NODE__ACTIVITY_OUTPUT;

  /**
	 * The feature id for the '<em><b>Activity Output Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__ACTIVITY_OUTPUT_OPT = ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT;

  /**
	 * The feature id for the '<em><b>Optional</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__OPTIONAL = ACTIVITY_NODE__OPTIONAL;

  /**
	 * The feature id for the '<em><b>Activity Input Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__ACTIVITY_INPUT_OPT = ACTIVITY_NODE__ACTIVITY_INPUT_OPT;

  /**
	 * The feature id for the '<em><b>Activity Input</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__ACTIVITY_INPUT = ACTIVITY_NODE__ACTIVITY_INPUT;

  /**
	 * The feature id for the '<em><b>Min Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SIMPLE_ACTIVITY__MIN_TIME = ACTIVITY_NODE__MIN_TIME;

		/**
	 * The feature id for the '<em><b>Cost</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__COST = ACTIVITY_NODE__COST;

  /**
	 * The feature id for the '<em><b>Probability</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY__PROBABILITY = ACTIVITY_NODE__PROBABILITY;

  /**
	 * The feature id for the '<em><b>Max Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SIMPLE_ACTIVITY__MAX_TIME = ACTIVITY_NODE__MAX_TIME;

		/**
	 * The number of structural features of the '<em>Simple Activity</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SIMPLE_ACTIVITY_FEATURE_COUNT = ACTIVITY_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.SubprocessImpl <em>Subprocess</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.SubprocessImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getSubprocess()
	 * @generated
	 */
  int SUBPROCESS = 13;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__LABEL = ACTIVITY_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__INCOMING = ACTIVITY_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__OUTGOING = ACTIVITY_NODE__OUTGOING;

  /**
	 * The feature id for the '<em><b>Activity Output</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__ACTIVITY_OUTPUT = ACTIVITY_NODE__ACTIVITY_OUTPUT;

  /**
	 * The feature id for the '<em><b>Activity Output Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__ACTIVITY_OUTPUT_OPT = ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT;

  /**
	 * The feature id for the '<em><b>Optional</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__OPTIONAL = ACTIVITY_NODE__OPTIONAL;

  /**
	 * The feature id for the '<em><b>Activity Input Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__ACTIVITY_INPUT_OPT = ACTIVITY_NODE__ACTIVITY_INPUT_OPT;

  /**
	 * The feature id for the '<em><b>Activity Input</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__ACTIVITY_INPUT = ACTIVITY_NODE__ACTIVITY_INPUT;

  /**
	 * The feature id for the '<em><b>Min Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SUBPROCESS__MIN_TIME = ACTIVITY_NODE__MIN_TIME;

		/**
	 * The feature id for the '<em><b>Cost</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__COST = ACTIVITY_NODE__COST;

  /**
	 * The feature id for the '<em><b>Probability</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__PROBABILITY = ACTIVITY_NODE__PROBABILITY;

  /**
	 * The feature id for the '<em><b>Max Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SUBPROCESS__MAX_TIME = ACTIVITY_NODE__MAX_TIME;

		/**
	 * The feature id for the '<em><b>Subprocess</b></em>' reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS__SUBPROCESS = ACTIVITY_NODE_FEATURE_COUNT + 0;

  /**
	 * The feature id for the '<em><b>Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int SUBPROCESS__WEIGHT = ACTIVITY_NODE_FEATURE_COUNT + 1;

		/**
	 * The number of structural features of the '<em>Subprocess</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int SUBPROCESS_FEATURE_COUNT = ACTIVITY_NODE_FEATURE_COUNT + 2;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.WorkflowImpl <em>Workflow</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.WorkflowImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getWorkflow()
	 * @generated
	 */
  int WORKFLOW = 14;

  /**
	 * The feature id for the '<em><b>Processes</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW__PROCESSES = 0;

  /**
	 * The number of structural features of the '<em>Workflow</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int WORKFLOW_FEATURE_COUNT = 1;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.PersistentResourceImpl <em>Persistent Resource</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.PersistentResourceImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getPersistentResource()
	 * @generated
	 */
  int PERSISTENT_RESOURCE = 15;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PERSISTENT_RESOURCE__LABEL = RESOURCE_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PERSISTENT_RESOURCE__INCOMING = RESOURCE_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PERSISTENT_RESOURCE__OUTGOING = RESOURCE_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Persistent Resource</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PERSISTENT_RESOURCE_FEATURE_COUNT = RESOURCE_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.TransientResourceImpl <em>Transient Resource</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.TransientResourceImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getTransientResource()
	 * @generated
	 */
  int TRANSIENT_RESOURCE = 16;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSIENT_RESOURCE__LABEL = RESOURCE_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSIENT_RESOURCE__INCOMING = RESOURCE_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSIENT_RESOURCE__OUTGOING = RESOURCE_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Transient Resource</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSIENT_RESOURCE_FEATURE_COUNT = RESOURCE_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.DocumentImpl <em>Document</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.DocumentImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getDocument()
	 * @generated
	 */
  int DOCUMENT = 17;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DOCUMENT__LABEL = RESOURCE_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DOCUMENT__INCOMING = RESOURCE_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DOCUMENT__OUTGOING = RESOURCE_NODE__OUTGOING;

  /**
	 * The number of structural features of the '<em>Document</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DOCUMENT_FEATURE_COUNT = RESOURCE_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.EventImpl <em>Event</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.EventImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getEvent()
	 * @generated
	 */
  int EVENT = 18;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__LABEL = ACTIVITY_NODE__LABEL;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__INCOMING = ACTIVITY_NODE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__OUTGOING = ACTIVITY_NODE__OUTGOING;

  /**
	 * The feature id for the '<em><b>Activity Output</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__ACTIVITY_OUTPUT = ACTIVITY_NODE__ACTIVITY_OUTPUT;

  /**
	 * The feature id for the '<em><b>Activity Output Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__ACTIVITY_OUTPUT_OPT = ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT;

  /**
	 * The feature id for the '<em><b>Optional</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__OPTIONAL = ACTIVITY_NODE__OPTIONAL;

  /**
	 * The feature id for the '<em><b>Activity Input Opt</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__ACTIVITY_INPUT_OPT = ACTIVITY_NODE__ACTIVITY_INPUT_OPT;

  /**
	 * The feature id for the '<em><b>Activity Input</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__ACTIVITY_INPUT = ACTIVITY_NODE__ACTIVITY_INPUT;

  /**
	 * The feature id for the '<em><b>Min Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__MIN_TIME = ACTIVITY_NODE__MIN_TIME;

		/**
	 * The feature id for the '<em><b>Cost</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__COST = ACTIVITY_NODE__COST;

  /**
	 * The feature id for the '<em><b>Probability</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT__PROBABILITY = ACTIVITY_NODE__PROBABILITY;

  /**
	 * The feature id for the '<em><b>Max Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__MAX_TIME = ACTIVITY_NODE__MAX_TIME;

		/**
	 * The number of structural features of the '<em>Event</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int EVENT_FEATURE_COUNT = ACTIVITY_NODE_FEATURE_COUNT + 0;

  /**
	 * The meta object id for the '{@link hub.top.lang.flowcharts.impl.DiagramArcImpl <em>Diagram Arc</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.lang.flowcharts.impl.DiagramArcImpl
	 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getDiagramArc()
	 * @generated
	 */
  int DIAGRAM_ARC = 19;

  /**
	 * The feature id for the '<em><b>Src</b></em>' reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DIAGRAM_ARC__SRC = 0;

  /**
	 * The feature id for the '<em><b>Target</b></em>' reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DIAGRAM_ARC__TARGET = 1;

  /**
	 * The feature id for the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DIAGRAM_ARC__LABEL = 2;

  /**
	 * The feature id for the '<em><b>Optional</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DIAGRAM_ARC__OPTIONAL = 3;

		/**
	 * The feature id for the '<em><b>Probability</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DIAGRAM_ARC__PROBABILITY = 4;

		/**
	 * The feature id for the '<em><b>Min Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DIAGRAM_ARC__MIN_WEIGHT = 5;

  /**
	 * The feature id for the '<em><b>Max Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DIAGRAM_ARC__MAX_WEIGHT = 6;

		/**
	 * The number of structural features of the '<em>Diagram Arc</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int DIAGRAM_ARC_FEATURE_COUNT = 7;


  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.WorkflowDiagram <em>Workflow Diagram</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Workflow Diagram</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram
	 * @generated
	 */
  EClass getWorkflowDiagram();

  /**
	 * Returns the meta object for the containment reference list '{@link hub.top.lang.flowcharts.WorkflowDiagram#getDiagramNodes <em>Diagram Nodes</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Diagram Nodes</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getDiagramNodes()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EReference getWorkflowDiagram_DiagramNodes();

  /**
	 * Returns the meta object for the reference '{@link hub.top.lang.flowcharts.WorkflowDiagram#getStartNode <em>Start Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Start Node</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getStartNode()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EReference getWorkflowDiagram_StartNode();

  /**
	 * Returns the meta object for the reference '{@link hub.top.lang.flowcharts.WorkflowDiagram#getEndNode <em>End Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>End Node</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getEndNode()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EReference getWorkflowDiagram_EndNode();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessOutput <em>Process Output</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Process Output</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getProcessOutput()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EReference getWorkflowDiagram_ProcessOutput();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessOutputOpt <em>Process Output Opt</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Process Output Opt</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getProcessOutputOpt()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EReference getWorkflowDiagram_ProcessOutputOpt();

  /**
	 * Returns the meta object for the containment reference list '{@link hub.top.lang.flowcharts.WorkflowDiagram#getDiagramArcs <em>Diagram Arcs</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Diagram Arcs</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getDiagramArcs()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EReference getWorkflowDiagram_DiagramArcs();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.WorkflowDiagram#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getName()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EAttribute getWorkflowDiagram_Name();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessInput <em>Process Input</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Process Input</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getProcessInput()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EReference getWorkflowDiagram_ProcessInput();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessInputOpt <em>Process Input Opt</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Process Input Opt</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getProcessInputOpt()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EReference getWorkflowDiagram_ProcessInputOpt();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessCalls <em>Process Calls</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Process Calls</em>'.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getProcessCalls()
	 * @see #getWorkflowDiagram()
	 * @generated
	 */
  EReference getWorkflowDiagram_ProcessCalls();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.DiagramNode <em>Diagram Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Diagram Node</em>'.
	 * @see hub.top.lang.flowcharts.DiagramNode
	 * @generated
	 */
  EClass getDiagramNode();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.DiagramNode#getLabel <em>Label</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Label</em>'.
	 * @see hub.top.lang.flowcharts.DiagramNode#getLabel()
	 * @see #getDiagramNode()
	 * @generated
	 */
  EAttribute getDiagramNode_Label();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.DiagramNode#getIncoming <em>Incoming</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Incoming</em>'.
	 * @see hub.top.lang.flowcharts.DiagramNode#getIncoming()
	 * @see #getDiagramNode()
	 * @generated
	 */
  EReference getDiagramNode_Incoming();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.DiagramNode#getOutgoing <em>Outgoing</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Outgoing</em>'.
	 * @see hub.top.lang.flowcharts.DiagramNode#getOutgoing()
	 * @see #getDiagramNode()
	 * @generated
	 */
  EReference getDiagramNode_Outgoing();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.ControlFlowNode <em>Control Flow Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Control Flow Node</em>'.
	 * @see hub.top.lang.flowcharts.ControlFlowNode
	 * @generated
	 */
  EClass getControlFlowNode();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.ActivityNode <em>Activity Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Activity Node</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode
	 * @generated
	 */
  EClass getActivityNode();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.ActivityNode#getActivityOutput <em>Activity Output</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Activity Output</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode#getActivityOutput()
	 * @see #getActivityNode()
	 * @generated
	 */
  EReference getActivityNode_ActivityOutput();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.ActivityNode#getActivityOutputOpt <em>Activity Output Opt</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Activity Output Opt</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode#getActivityOutputOpt()
	 * @see #getActivityNode()
	 * @generated
	 */
  EReference getActivityNode_ActivityOutputOpt();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.ActivityNode#isOptional <em>Optional</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Optional</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode#isOptional()
	 * @see #getActivityNode()
	 * @generated
	 */
  EAttribute getActivityNode_Optional();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.ActivityNode#getActivityInputOpt <em>Activity Input Opt</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Activity Input Opt</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode#getActivityInputOpt()
	 * @see #getActivityNode()
	 * @generated
	 */
  EReference getActivityNode_ActivityInputOpt();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.ActivityNode#getActivityInput <em>Activity Input</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Activity Input</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode#getActivityInput()
	 * @see #getActivityNode()
	 * @generated
	 */
  EReference getActivityNode_ActivityInput();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.ActivityNode#getMinTime <em>Min Time</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Min Time</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode#getMinTime()
	 * @see #getActivityNode()
	 * @generated
	 */
	EAttribute getActivityNode_MinTime();

		/**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.ActivityNode#getCost <em>Cost</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Cost</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode#getCost()
	 * @see #getActivityNode()
	 * @generated
	 */
  EAttribute getActivityNode_Cost();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.ActivityNode#getProbability <em>Probability</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Probability</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode#getProbability()
	 * @see #getActivityNode()
	 * @generated
	 */
  EAttribute getActivityNode_Probability();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.ActivityNode#getMaxTime <em>Max Time</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Max Time</em>'.
	 * @see hub.top.lang.flowcharts.ActivityNode#getMaxTime()
	 * @see #getActivityNode()
	 * @generated
	 */
	EAttribute getActivityNode_MaxTime();

		/**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.ResourceNode <em>Resource Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Resource Node</em>'.
	 * @see hub.top.lang.flowcharts.ResourceNode
	 * @generated
	 */
  EClass getResourceNode();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.StartNode <em>Start Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Start Node</em>'.
	 * @see hub.top.lang.flowcharts.StartNode
	 * @generated
	 */
  EClass getStartNode();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.Endnode <em>Endnode</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Endnode</em>'.
	 * @see hub.top.lang.flowcharts.Endnode
	 * @generated
	 */
  EClass getEndnode();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.SimpleNode <em>Simple Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Simple Node</em>'.
	 * @see hub.top.lang.flowcharts.SimpleNode
	 * @generated
	 */
  EClass getSimpleNode();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.StructuredNode <em>Structured Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Structured Node</em>'.
	 * @see hub.top.lang.flowcharts.StructuredNode
	 * @generated
	 */
  EClass getStructuredNode();

  /**
	 * Returns the meta object for the containment reference list '{@link hub.top.lang.flowcharts.StructuredNode#getChildNodes <em>Child Nodes</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Child Nodes</em>'.
	 * @see hub.top.lang.flowcharts.StructuredNode#getChildNodes()
	 * @see #getStructuredNode()
	 * @generated
	 */
  EReference getStructuredNode_ChildNodes();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.SplitNode <em>Split Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Split Node</em>'.
	 * @see hub.top.lang.flowcharts.SplitNode
	 * @generated
	 */
  EClass getSplitNode();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.MergeNode <em>Merge Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Merge Node</em>'.
	 * @see hub.top.lang.flowcharts.MergeNode
	 * @generated
	 */
  EClass getMergeNode();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.Flow <em>Flow</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Flow</em>'.
	 * @see hub.top.lang.flowcharts.Flow
	 * @generated
	 */
  EClass getFlow();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.Flow#getStartNode <em>Start Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Start Node</em>'.
	 * @see hub.top.lang.flowcharts.Flow#getStartNode()
	 * @see #getFlow()
	 * @generated
	 */
  EReference getFlow_StartNode();

  /**
	 * Returns the meta object for the reference list '{@link hub.top.lang.flowcharts.Flow#getEndNode <em>End Node</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>End Node</em>'.
	 * @see hub.top.lang.flowcharts.Flow#getEndNode()
	 * @see #getFlow()
	 * @generated
	 */
  EReference getFlow_EndNode();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.SimpleActivity <em>Simple Activity</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Simple Activity</em>'.
	 * @see hub.top.lang.flowcharts.SimpleActivity
	 * @generated
	 */
  EClass getSimpleActivity();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.Subprocess <em>Subprocess</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Subprocess</em>'.
	 * @see hub.top.lang.flowcharts.Subprocess
	 * @generated
	 */
  EClass getSubprocess();

  /**
	 * Returns the meta object for the reference '{@link hub.top.lang.flowcharts.Subprocess#getSubprocess <em>Subprocess</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Subprocess</em>'.
	 * @see hub.top.lang.flowcharts.Subprocess#getSubprocess()
	 * @see #getSubprocess()
	 * @generated
	 */
  EReference getSubprocess_Subprocess();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.Subprocess#getWeight <em>Weight</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Weight</em>'.
	 * @see hub.top.lang.flowcharts.Subprocess#getWeight()
	 * @see #getSubprocess()
	 * @generated
	 */
	EAttribute getSubprocess_Weight();

		/**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.Workflow <em>Workflow</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Workflow</em>'.
	 * @see hub.top.lang.flowcharts.Workflow
	 * @generated
	 */
  EClass getWorkflow();

  /**
	 * Returns the meta object for the containment reference list '{@link hub.top.lang.flowcharts.Workflow#getProcesses <em>Processes</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Processes</em>'.
	 * @see hub.top.lang.flowcharts.Workflow#getProcesses()
	 * @see #getWorkflow()
	 * @generated
	 */
  EReference getWorkflow_Processes();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.PersistentResource <em>Persistent Resource</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Persistent Resource</em>'.
	 * @see hub.top.lang.flowcharts.PersistentResource
	 * @generated
	 */
  EClass getPersistentResource();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.TransientResource <em>Transient Resource</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Transient Resource</em>'.
	 * @see hub.top.lang.flowcharts.TransientResource
	 * @generated
	 */
  EClass getTransientResource();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.Document <em>Document</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Document</em>'.
	 * @see hub.top.lang.flowcharts.Document
	 * @generated
	 */
  EClass getDocument();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.Event <em>Event</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Event</em>'.
	 * @see hub.top.lang.flowcharts.Event
	 * @generated
	 */
  EClass getEvent();

  /**
	 * Returns the meta object for class '{@link hub.top.lang.flowcharts.DiagramArc <em>Diagram Arc</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Diagram Arc</em>'.
	 * @see hub.top.lang.flowcharts.DiagramArc
	 * @generated
	 */
  EClass getDiagramArc();

  /**
	 * Returns the meta object for the reference '{@link hub.top.lang.flowcharts.DiagramArc#getSrc <em>Src</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Src</em>'.
	 * @see hub.top.lang.flowcharts.DiagramArc#getSrc()
	 * @see #getDiagramArc()
	 * @generated
	 */
  EReference getDiagramArc_Src();

  /**
	 * Returns the meta object for the reference '{@link hub.top.lang.flowcharts.DiagramArc#getTarget <em>Target</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Target</em>'.
	 * @see hub.top.lang.flowcharts.DiagramArc#getTarget()
	 * @see #getDiagramArc()
	 * @generated
	 */
  EReference getDiagramArc_Target();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.DiagramArc#getLabel <em>Label</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Label</em>'.
	 * @see hub.top.lang.flowcharts.DiagramArc#getLabel()
	 * @see #getDiagramArc()
	 * @generated
	 */
  EAttribute getDiagramArc_Label();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.DiagramArc#isOptional <em>Optional</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Optional</em>'.
	 * @see hub.top.lang.flowcharts.DiagramArc#isOptional()
	 * @see #getDiagramArc()
	 * @generated
	 */
	EAttribute getDiagramArc_Optional();

		/**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.DiagramArc#getMinWeight <em>Min Weight</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Min Weight</em>'.
	 * @see hub.top.lang.flowcharts.DiagramArc#getMinWeight()
	 * @see #getDiagramArc()
	 * @generated
	 */
	EAttribute getDiagramArc_MinWeight();

		/**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.DiagramArc#getProbability <em>Probability</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Probability</em>'.
	 * @see hub.top.lang.flowcharts.DiagramArc#getProbability()
	 * @see #getDiagramArc()
	 * @generated
	 */
  EAttribute getDiagramArc_Probability();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.lang.flowcharts.DiagramArc#getMaxWeight <em>Max Weight</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Max Weight</em>'.
	 * @see hub.top.lang.flowcharts.DiagramArc#getMaxWeight()
	 * @see #getDiagramArc()
	 * @generated
	 */
	EAttribute getDiagramArc_MaxWeight();

		/**
	 * Returns the factory that creates the instances of the model.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the factory that creates the instances of the model.
	 * @generated
	 */
  FlowFactory getFlowFactory();

  /**
	 * <!-- begin-user-doc -->
   * Defines literals for the meta objects that represent
   * <ul>
   *   <li>each class,</li>
   *   <li>each feature of each class,</li>
   *   <li>each enum,</li>
   *   <li>and each data type</li>
   * </ul>
   * <!-- end-user-doc -->
	 * @generated
	 */
  interface Literals {
    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl <em>Workflow Diagram</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.WorkflowDiagramImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getWorkflowDiagram()
		 * @generated
		 */
    EClass WORKFLOW_DIAGRAM = eINSTANCE.getWorkflowDiagram();

    /**
		 * The meta object literal for the '<em><b>Diagram Nodes</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW_DIAGRAM__DIAGRAM_NODES = eINSTANCE.getWorkflowDiagram_DiagramNodes();

    /**
		 * The meta object literal for the '<em><b>Start Node</b></em>' reference feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW_DIAGRAM__START_NODE = eINSTANCE.getWorkflowDiagram_StartNode();

    /**
		 * The meta object literal for the '<em><b>End Node</b></em>' reference feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW_DIAGRAM__END_NODE = eINSTANCE.getWorkflowDiagram_EndNode();

    /**
		 * The meta object literal for the '<em><b>Process Output</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW_DIAGRAM__PROCESS_OUTPUT = eINSTANCE.getWorkflowDiagram_ProcessOutput();

    /**
		 * The meta object literal for the '<em><b>Process Output Opt</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW_DIAGRAM__PROCESS_OUTPUT_OPT = eINSTANCE.getWorkflowDiagram_ProcessOutputOpt();

    /**
		 * The meta object literal for the '<em><b>Diagram Arcs</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW_DIAGRAM__DIAGRAM_ARCS = eINSTANCE.getWorkflowDiagram_DiagramArcs();

    /**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute WORKFLOW_DIAGRAM__NAME = eINSTANCE.getWorkflowDiagram_Name();

    /**
		 * The meta object literal for the '<em><b>Process Input</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW_DIAGRAM__PROCESS_INPUT = eINSTANCE.getWorkflowDiagram_ProcessInput();

    /**
		 * The meta object literal for the '<em><b>Process Input Opt</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW_DIAGRAM__PROCESS_INPUT_OPT = eINSTANCE.getWorkflowDiagram_ProcessInputOpt();

    /**
		 * The meta object literal for the '<em><b>Process Calls</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW_DIAGRAM__PROCESS_CALLS = eINSTANCE.getWorkflowDiagram_ProcessCalls();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.DiagramNodeImpl <em>Diagram Node</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.DiagramNodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getDiagramNode()
		 * @generated
		 */
    EClass DIAGRAM_NODE = eINSTANCE.getDiagramNode();

    /**
		 * The meta object literal for the '<em><b>Label</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute DIAGRAM_NODE__LABEL = eINSTANCE.getDiagramNode_Label();

    /**
		 * The meta object literal for the '<em><b>Incoming</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference DIAGRAM_NODE__INCOMING = eINSTANCE.getDiagramNode_Incoming();

    /**
		 * The meta object literal for the '<em><b>Outgoing</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference DIAGRAM_NODE__OUTGOING = eINSTANCE.getDiagramNode_Outgoing();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.ControlFlowNodeImpl <em>Control Flow Node</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.ControlFlowNodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getControlFlowNode()
		 * @generated
		 */
    EClass CONTROL_FLOW_NODE = eINSTANCE.getControlFlowNode();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl <em>Activity Node</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.ActivityNodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getActivityNode()
		 * @generated
		 */
    EClass ACTIVITY_NODE = eINSTANCE.getActivityNode();

    /**
		 * The meta object literal for the '<em><b>Activity Output</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference ACTIVITY_NODE__ACTIVITY_OUTPUT = eINSTANCE.getActivityNode_ActivityOutput();

    /**
		 * The meta object literal for the '<em><b>Activity Output Opt</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT = eINSTANCE.getActivityNode_ActivityOutputOpt();

    /**
		 * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute ACTIVITY_NODE__OPTIONAL = eINSTANCE.getActivityNode_Optional();

    /**
		 * The meta object literal for the '<em><b>Activity Input Opt</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference ACTIVITY_NODE__ACTIVITY_INPUT_OPT = eINSTANCE.getActivityNode_ActivityInputOpt();

    /**
		 * The meta object literal for the '<em><b>Activity Input</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference ACTIVITY_NODE__ACTIVITY_INPUT = eINSTANCE.getActivityNode_ActivityInput();

    /**
		 * The meta object literal for the '<em><b>Min Time</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute ACTIVITY_NODE__MIN_TIME = eINSTANCE.getActivityNode_MinTime();

				/**
		 * The meta object literal for the '<em><b>Cost</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute ACTIVITY_NODE__COST = eINSTANCE.getActivityNode_Cost();

    /**
		 * The meta object literal for the '<em><b>Probability</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute ACTIVITY_NODE__PROBABILITY = eINSTANCE.getActivityNode_Probability();

    /**
		 * The meta object literal for the '<em><b>Max Time</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute ACTIVITY_NODE__MAX_TIME = eINSTANCE.getActivityNode_MaxTime();

				/**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.ResourceNodeImpl <em>Resource Node</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.ResourceNodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getResourceNode()
		 * @generated
		 */
    EClass RESOURCE_NODE = eINSTANCE.getResourceNode();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.StartNodeImpl <em>Start Node</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.StartNodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getStartNode()
		 * @generated
		 */
    EClass START_NODE = eINSTANCE.getStartNode();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.EndnodeImpl <em>Endnode</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.EndnodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getEndnode()
		 * @generated
		 */
    EClass ENDNODE = eINSTANCE.getEndnode();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.SimpleNodeImpl <em>Simple Node</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.SimpleNodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getSimpleNode()
		 * @generated
		 */
    EClass SIMPLE_NODE = eINSTANCE.getSimpleNode();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.StructuredNodeImpl <em>Structured Node</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.StructuredNodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getStructuredNode()
		 * @generated
		 */
    EClass STRUCTURED_NODE = eINSTANCE.getStructuredNode();

    /**
		 * The meta object literal for the '<em><b>Child Nodes</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference STRUCTURED_NODE__CHILD_NODES = eINSTANCE.getStructuredNode_ChildNodes();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.SplitNodeImpl <em>Split Node</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.SplitNodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getSplitNode()
		 * @generated
		 */
    EClass SPLIT_NODE = eINSTANCE.getSplitNode();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.MergeNodeImpl <em>Merge Node</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.MergeNodeImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getMergeNode()
		 * @generated
		 */
    EClass MERGE_NODE = eINSTANCE.getMergeNode();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.FlowImpl <em>Flow</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.FlowImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getFlow()
		 * @generated
		 */
    EClass FLOW = eINSTANCE.getFlow();

    /**
		 * The meta object literal for the '<em><b>Start Node</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference FLOW__START_NODE = eINSTANCE.getFlow_StartNode();

    /**
		 * The meta object literal for the '<em><b>End Node</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference FLOW__END_NODE = eINSTANCE.getFlow_EndNode();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.SimpleActivityImpl <em>Simple Activity</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.SimpleActivityImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getSimpleActivity()
		 * @generated
		 */
    EClass SIMPLE_ACTIVITY = eINSTANCE.getSimpleActivity();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.SubprocessImpl <em>Subprocess</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.SubprocessImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getSubprocess()
		 * @generated
		 */
    EClass SUBPROCESS = eINSTANCE.getSubprocess();

    /**
		 * The meta object literal for the '<em><b>Subprocess</b></em>' reference feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference SUBPROCESS__SUBPROCESS = eINSTANCE.getSubprocess_Subprocess();

    /**
		 * The meta object literal for the '<em><b>Weight</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute SUBPROCESS__WEIGHT = eINSTANCE.getSubprocess_Weight();

				/**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.WorkflowImpl <em>Workflow</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.WorkflowImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getWorkflow()
		 * @generated
		 */
    EClass WORKFLOW = eINSTANCE.getWorkflow();

    /**
		 * The meta object literal for the '<em><b>Processes</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference WORKFLOW__PROCESSES = eINSTANCE.getWorkflow_Processes();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.PersistentResourceImpl <em>Persistent Resource</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.PersistentResourceImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getPersistentResource()
		 * @generated
		 */
    EClass PERSISTENT_RESOURCE = eINSTANCE.getPersistentResource();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.TransientResourceImpl <em>Transient Resource</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.TransientResourceImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getTransientResource()
		 * @generated
		 */
    EClass TRANSIENT_RESOURCE = eINSTANCE.getTransientResource();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.DocumentImpl <em>Document</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.DocumentImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getDocument()
		 * @generated
		 */
    EClass DOCUMENT = eINSTANCE.getDocument();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.EventImpl <em>Event</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.EventImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getEvent()
		 * @generated
		 */
    EClass EVENT = eINSTANCE.getEvent();

    /**
		 * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.DiagramArcImpl <em>Diagram Arc</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.lang.flowcharts.impl.DiagramArcImpl
		 * @see hub.top.lang.flowcharts.impl.FlowPackageImpl#getDiagramArc()
		 * @generated
		 */
    EClass DIAGRAM_ARC = eINSTANCE.getDiagramArc();

    /**
		 * The meta object literal for the '<em><b>Src</b></em>' reference feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference DIAGRAM_ARC__SRC = eINSTANCE.getDiagramArc_Src();

    /**
		 * The meta object literal for the '<em><b>Target</b></em>' reference feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EReference DIAGRAM_ARC__TARGET = eINSTANCE.getDiagramArc_Target();

    /**
		 * The meta object literal for the '<em><b>Label</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute DIAGRAM_ARC__LABEL = eINSTANCE.getDiagramArc_Label();

    /**
		 * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute DIAGRAM_ARC__OPTIONAL = eINSTANCE.getDiagramArc_Optional();

				/**
		 * The meta object literal for the '<em><b>Min Weight</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute DIAGRAM_ARC__MIN_WEIGHT = eINSTANCE.getDiagramArc_MinWeight();

				/**
		 * The meta object literal for the '<em><b>Probability</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute DIAGRAM_ARC__PROBABILITY = eINSTANCE.getDiagramArc_Probability();

				/**
		 * The meta object literal for the '<em><b>Max Weight</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute DIAGRAM_ARC__MAX_WEIGHT = eINSTANCE.getDiagramArc_MaxWeight();

  }

} //FlowPackage

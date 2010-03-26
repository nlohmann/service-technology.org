/**
 * <copyright>
 * </copyright>
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
 * @see hub.top.lang.flowcharts.FlowChartFactory
 * @model kind="package"
 * @generated
 */
public interface FlowChartPackage extends EPackage {
	/**
   * The package name.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	String eNAME = "flow";

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
	FlowChartPackage eINSTANCE = hub.top.lang.flowcharts.impl.FlowChartPackageImpl.init();

	/**
   * The meta object id for the '{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl <em>Workflow Diagram</em>}' class.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see hub.top.lang.flowcharts.impl.WorkflowDiagramImpl
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getWorkflowDiagram()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getDiagramNode()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getControlFlowNode()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getActivityNode()
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
   * The number of structural features of the '<em>Activity Node</em>' class.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
	int ACTIVITY_NODE_FEATURE_COUNT = DIAGRAM_NODE_FEATURE_COUNT + 5;

	/**
   * The meta object id for the '{@link hub.top.lang.flowcharts.impl.ResourceNodeImpl <em>Resource Node</em>}' class.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see hub.top.lang.flowcharts.impl.ResourceNodeImpl
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getResourceNode()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getSimpleNode()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getStartNode()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getEndnode()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getStructuredNode()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getSplitNode()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getMergeNode()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getFlow()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getSimpleActivity()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getSubprocess()
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
   * The feature id for the '<em><b>Subprocess</b></em>' reference.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
	int SUBPROCESS__SUBPROCESS = ACTIVITY_NODE_FEATURE_COUNT + 0;

	/**
   * The number of structural features of the '<em>Subprocess</em>' class.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
	int SUBPROCESS_FEATURE_COUNT = ACTIVITY_NODE_FEATURE_COUNT + 1;

	/**
   * The meta object id for the '{@link hub.top.lang.flowcharts.impl.WorkflowImpl <em>Workflow</em>}' class.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see hub.top.lang.flowcharts.impl.WorkflowImpl
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getWorkflow()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getPersistentResource()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getTransientResource()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getDocument()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getEvent()
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
   * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getDiagramArc()
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
   * The number of structural features of the '<em>Diagram Arc</em>' class.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   * @ordered
   */
	int DIAGRAM_ARC_FEATURE_COUNT = 4;


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
   * Returns the factory that creates the instances of the model.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return the factory that creates the instances of the model.
   * @generated
   */
	FlowChartFactory getFCFactory();

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
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getWorkflowDiagram()
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
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getDiagramNode()
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
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getControlFlowNode()
     * @generated
     */
		EClass CONTROL_FLOW_NODE = eINSTANCE.getControlFlowNode();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl <em>Activity Node</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.ActivityNodeImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getActivityNode()
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
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.ResourceNodeImpl <em>Resource Node</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.ResourceNodeImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getResourceNode()
     * @generated
     */
		EClass RESOURCE_NODE = eINSTANCE.getResourceNode();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.StartNodeImpl <em>Start Node</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.StartNodeImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getStartNode()
     * @generated
     */
		EClass START_NODE = eINSTANCE.getStartNode();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.EndnodeImpl <em>Endnode</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.EndnodeImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getEndnode()
     * @generated
     */
		EClass ENDNODE = eINSTANCE.getEndnode();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.SimpleNodeImpl <em>Simple Node</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.SimpleNodeImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getSimpleNode()
     * @generated
     */
		EClass SIMPLE_NODE = eINSTANCE.getSimpleNode();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.StructuredNodeImpl <em>Structured Node</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.StructuredNodeImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getStructuredNode()
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
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getSplitNode()
     * @generated
     */
		EClass SPLIT_NODE = eINSTANCE.getSplitNode();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.MergeNodeImpl <em>Merge Node</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.MergeNodeImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getMergeNode()
     * @generated
     */
		EClass MERGE_NODE = eINSTANCE.getMergeNode();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.FlowImpl <em>Flow</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.FlowImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getFlow()
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
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getSimpleActivity()
     * @generated
     */
		EClass SIMPLE_ACTIVITY = eINSTANCE.getSimpleActivity();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.SubprocessImpl <em>Subprocess</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.SubprocessImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getSubprocess()
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
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.WorkflowImpl <em>Workflow</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.WorkflowImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getWorkflow()
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
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getPersistentResource()
     * @generated
     */
		EClass PERSISTENT_RESOURCE = eINSTANCE.getPersistentResource();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.TransientResourceImpl <em>Transient Resource</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.TransientResourceImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getTransientResource()
     * @generated
     */
		EClass TRANSIENT_RESOURCE = eINSTANCE.getTransientResource();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.DocumentImpl <em>Document</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.DocumentImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getDocument()
     * @generated
     */
		EClass DOCUMENT = eINSTANCE.getDocument();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.EventImpl <em>Event</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.EventImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getEvent()
     * @generated
     */
		EClass EVENT = eINSTANCE.getEvent();

		/**
     * The meta object literal for the '{@link hub.top.lang.flowcharts.impl.DiagramArcImpl <em>Diagram Arc</em>}' class.
     * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
     * @see hub.top.lang.flowcharts.impl.DiagramArcImpl
     * @see hub.top.lang.flowcharts.impl.FlowChartPackageImpl#getDiagramArc()
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

	}

} //FlowChartPackage

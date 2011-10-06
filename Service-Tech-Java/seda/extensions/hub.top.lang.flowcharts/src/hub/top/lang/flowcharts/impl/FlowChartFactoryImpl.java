/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.impl;

import hub.top.lang.flowcharts.*;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.impl.EFactoryImpl;

import org.eclipse.emf.ecore.plugin.EcorePlugin;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class FlowChartFactoryImpl extends EFactoryImpl implements FlowChartFactory {
	/**
   * Creates the default factory implementation.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public static FlowChartFactory init() {
    try {
      FlowChartFactory theFCFactory = (FlowChartFactory)EPackage.Registry.INSTANCE.getEFactory("http://hu-berlin.de/flowchart"); 
      if (theFCFactory != null) {
        return theFCFactory;
      }
    }
    catch (Exception exception) {
      EcorePlugin.INSTANCE.log(exception);
    }
    return new FlowChartFactoryImpl();
  }

	/**
   * Creates an instance of the factory.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public FlowChartFactoryImpl() {
    super();
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	@Override
	public EObject create(EClass eClass) {
    switch (eClass.getClassifierID()) {
      case FlowChartPackage.WORKFLOW_DIAGRAM: return createWorkflowDiagram();
      case FlowChartPackage.START_NODE: return createStartNode();
      case FlowChartPackage.ENDNODE: return createEndnode();
      case FlowChartPackage.SPLIT_NODE: return createSplitNode();
      case FlowChartPackage.MERGE_NODE: return createMergeNode();
      case FlowChartPackage.FLOW: return createFlow();
      case FlowChartPackage.SIMPLE_ACTIVITY: return createSimpleActivity();
      case FlowChartPackage.SUBPROCESS: return createSubprocess();
      case FlowChartPackage.WORKFLOW: return createWorkflow();
      case FlowChartPackage.PERSISTENT_RESOURCE: return createPersistentResource();
      case FlowChartPackage.TRANSIENT_RESOURCE: return createTransientResource();
      case FlowChartPackage.DOCUMENT: return createDocument();
      case FlowChartPackage.EVENT: return createEvent();
      case FlowChartPackage.DIAGRAM_ARC: return createDiagramArc();
      default:
        throw new IllegalArgumentException("The class '" + eClass.getName() + "' is not a valid classifier");
    }
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public WorkflowDiagram createWorkflowDiagram() {
    WorkflowDiagramImpl workflowDiagram = new WorkflowDiagramImpl();
    return workflowDiagram;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public StartNode createStartNode() {
    StartNodeImpl startNode = new StartNodeImpl();
    return startNode;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public Endnode createEndnode() {
    EndnodeImpl endnode = new EndnodeImpl();
    return endnode;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public SplitNode createSplitNode() {
    SplitNodeImpl splitNode = new SplitNodeImpl();
    return splitNode;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public MergeNode createMergeNode() {
    MergeNodeImpl mergeNode = new MergeNodeImpl();
    return mergeNode;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public Flow createFlow() {
    FlowImpl flow = new FlowImpl();
    return flow;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public SimpleActivity createSimpleActivity() {
    SimpleActivityImpl simpleActivity = new SimpleActivityImpl();
    return simpleActivity;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public Subprocess createSubprocess() {
    SubprocessImpl subprocess = new SubprocessImpl();
    return subprocess;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public Workflow createWorkflow() {
    WorkflowImpl workflow = new WorkflowImpl();
    return workflow;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public PersistentResource createPersistentResource() {
    PersistentResourceImpl persistentResource = new PersistentResourceImpl();
    return persistentResource;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public TransientResource createTransientResource() {
    TransientResourceImpl transientResource = new TransientResourceImpl();
    return transientResource;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public Document createDocument() {
    DocumentImpl document = new DocumentImpl();
    return document;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public Event createEvent() {
    EventImpl event = new EventImpl();
    return event;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public DiagramArc createDiagramArc() {
    DiagramArcImpl diagramArc = new DiagramArcImpl();
    return diagramArc;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public FlowChartPackage getFCPackage() {
    return (FlowChartPackage)getEPackage();
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @deprecated
   * @generated
   */
	@Deprecated
	public static FlowChartPackage getPackage() {
    return FlowChartPackage.eINSTANCE;
  }

} //FlowChartFactoryImpl

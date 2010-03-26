/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.impl;

import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.ControlFlowNode;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Document;
import hub.top.lang.flowcharts.Endnode;
import hub.top.lang.flowcharts.Event;
import hub.top.lang.flowcharts.Flow;
import hub.top.lang.flowcharts.MergeNode;
import hub.top.lang.flowcharts.PersistentResource;
import hub.top.lang.flowcharts.ResourceNode;
import hub.top.lang.flowcharts.SimpleActivity;
import hub.top.lang.flowcharts.SimpleNode;
import hub.top.lang.flowcharts.SplitNode;
import hub.top.lang.flowcharts.StartNode;
import hub.top.lang.flowcharts.StructuredNode;
import hub.top.lang.flowcharts.Subprocess;
import hub.top.lang.flowcharts.FlowChartFactory;
import hub.top.lang.flowcharts.FlowChartPackage;
import hub.top.lang.flowcharts.TransientResource;
import hub.top.lang.flowcharts.Workflow;
import hub.top.lang.flowcharts.WorkflowDiagram;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

import org.eclipse.emf.ecore.impl.EPackageImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Package</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class FlowChartPackageImpl extends EPackageImpl implements FlowChartPackage {
	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass workflowDiagramEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass diagramNodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass controlFlowNodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass activityNodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass resourceNodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass startNodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass endnodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass simpleNodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass structuredNodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass splitNodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass mergeNodeEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass flowEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass simpleActivityEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass subprocessEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass workflowEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass persistentResourceEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass transientResourceEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass documentEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass eventEClass = null;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private EClass diagramArcEClass = null;

	/**
   * Creates an instance of the model <b>Package</b>, registered with
   * {@link org.eclipse.emf.ecore.EPackage.Registry EPackage.Registry} by the package
   * package URI value.
   * <p>Note: the correct way to create the package is via the static
   * factory method {@link #init init()}, which also performs
   * initialization of the package, or returns the registered package,
   * if one already exists.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see org.eclipse.emf.ecore.EPackage.Registry
   * @see hub.top.lang.flowcharts.FlowChartPackage#eNS_URI
   * @see #init()
   * @generated
   */
	private FlowChartPackageImpl() {
    super(eNS_URI, FlowChartFactory.eINSTANCE);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private static boolean isInited = false;

	/**
   * Creates, registers, and initializes the <b>Package</b> for this
   * model, and for any others upon which it depends.  Simple
   * dependencies are satisfied by calling this method on all
   * dependent packages before doing anything else.  This method drives
   * initialization for interdependent packages directly, in parallel
   * with this package, itself.
   * <p>Of this package and its interdependencies, all packages which
   * have not yet been registered by their URI values are first created
   * and registered.  The packages are then initialized in two steps:
   * meta-model objects for all of the packages are created before any
   * are initialized, since one package's meta-model objects may refer to
   * those of another.
   * <p>Invocation of this method will not affect any packages that have
   * already been initialized.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see #eNS_URI
   * @see #createPackageContents()
   * @see #initializePackageContents()
   * @generated
   */
	public static FlowChartPackage init() {
    if (isInited) return (FlowChartPackage)EPackage.Registry.INSTANCE.getEPackage(FlowChartPackage.eNS_URI);

    // Obtain or create and register package
    FlowChartPackageImpl theFCPackage = (FlowChartPackageImpl)(EPackage.Registry.INSTANCE.getEPackage(eNS_URI) instanceof FlowChartPackageImpl ? EPackage.Registry.INSTANCE.getEPackage(eNS_URI) : new FlowChartPackageImpl());

    isInited = true;

    // Create package meta-data objects
    theFCPackage.createPackageContents();

    // Initialize created meta-data
    theFCPackage.initializePackageContents();

    // Mark meta-data to indicate it can't be changed
    theFCPackage.freeze();

    return theFCPackage;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getWorkflowDiagram() {
    return workflowDiagramEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getWorkflowDiagram_DiagramNodes() {
    return (EReference)workflowDiagramEClass.getEStructuralFeatures().get(0);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getWorkflowDiagram_StartNode() {
    return (EReference)workflowDiagramEClass.getEStructuralFeatures().get(1);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getWorkflowDiagram_EndNode() {
    return (EReference)workflowDiagramEClass.getEStructuralFeatures().get(2);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getWorkflowDiagram_ProcessOutput() {
    return (EReference)workflowDiagramEClass.getEStructuralFeatures().get(3);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getWorkflowDiagram_ProcessOutputOpt() {
    return (EReference)workflowDiagramEClass.getEStructuralFeatures().get(4);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getWorkflowDiagram_DiagramArcs() {
    return (EReference)workflowDiagramEClass.getEStructuralFeatures().get(5);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EAttribute getWorkflowDiagram_Name() {
    return (EAttribute)workflowDiagramEClass.getEStructuralFeatures().get(6);
  }

	/**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWorkflowDiagram_ProcessInput() {
    return (EReference)workflowDiagramEClass.getEStructuralFeatures().get(7);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWorkflowDiagram_ProcessInputOpt() {
    return (EReference)workflowDiagramEClass.getEStructuralFeatures().get(8);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EReference getWorkflowDiagram_ProcessCalls() {
    return (EReference)workflowDiagramEClass.getEStructuralFeatures().get(9);
  }

  /**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getDiagramNode() {
    return diagramNodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EAttribute getDiagramNode_Label() {
    return (EAttribute)diagramNodeEClass.getEStructuralFeatures().get(0);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getDiagramNode_Incoming() {
    return (EReference)diagramNodeEClass.getEStructuralFeatures().get(1);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getDiagramNode_Outgoing() {
    return (EReference)diagramNodeEClass.getEStructuralFeatures().get(2);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getControlFlowNode() {
    return controlFlowNodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getActivityNode() {
    return activityNodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getActivityNode_ActivityOutput() {
    return (EReference)activityNodeEClass.getEStructuralFeatures().get(0);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getActivityNode_ActivityOutputOpt() {
    return (EReference)activityNodeEClass.getEStructuralFeatures().get(1);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EAttribute getActivityNode_Optional() {
    return (EAttribute)activityNodeEClass.getEStructuralFeatures().get(2);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getActivityNode_ActivityInputOpt() {
    return (EReference)activityNodeEClass.getEStructuralFeatures().get(3);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getActivityNode_ActivityInput() {
    return (EReference)activityNodeEClass.getEStructuralFeatures().get(4);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getResourceNode() {
    return resourceNodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getStartNode() {
    return startNodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getEndnode() {
    return endnodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getSimpleNode() {
    return simpleNodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getStructuredNode() {
    return structuredNodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getStructuredNode_ChildNodes() {
    return (EReference)structuredNodeEClass.getEStructuralFeatures().get(0);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getSplitNode() {
    return splitNodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getMergeNode() {
    return mergeNodeEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getFlow() {
    return flowEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getFlow_StartNode() {
    return (EReference)flowEClass.getEStructuralFeatures().get(0);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getFlow_EndNode() {
    return (EReference)flowEClass.getEStructuralFeatures().get(1);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getSimpleActivity() {
    return simpleActivityEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getSubprocess() {
    return subprocessEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getSubprocess_Subprocess() {
    return (EReference)subprocessEClass.getEStructuralFeatures().get(0);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getWorkflow() {
    return workflowEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getWorkflow_Processes() {
    return (EReference)workflowEClass.getEStructuralFeatures().get(0);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getPersistentResource() {
    return persistentResourceEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getTransientResource() {
    return transientResourceEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getDocument() {
    return documentEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getEvent() {
    return eventEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EClass getDiagramArc() {
    return diagramArcEClass;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getDiagramArc_Src() {
    return (EReference)diagramArcEClass.getEStructuralFeatures().get(0);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EReference getDiagramArc_Target() {
    return (EReference)diagramArcEClass.getEStructuralFeatures().get(1);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EAttribute getDiagramArc_Label() {
    return (EAttribute)diagramArcEClass.getEStructuralFeatures().get(2);
  }

	/**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public EAttribute getDiagramArc_Optional() {
    return (EAttribute)diagramArcEClass.getEStructuralFeatures().get(3);
  }

  /**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public FlowChartFactory getFCFactory() {
    return (FlowChartFactory)getEFactoryInstance();
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private boolean isCreated = false;

	/**
   * Creates the meta-model objects for the package.  This method is
   * guarded to have no affect on any invocation but its first.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public void createPackageContents() {
    if (isCreated) return;
    isCreated = true;

    // Create classes and their features
    workflowDiagramEClass = createEClass(WORKFLOW_DIAGRAM);
    createEReference(workflowDiagramEClass, WORKFLOW_DIAGRAM__DIAGRAM_NODES);
    createEReference(workflowDiagramEClass, WORKFLOW_DIAGRAM__START_NODE);
    createEReference(workflowDiagramEClass, WORKFLOW_DIAGRAM__END_NODE);
    createEReference(workflowDiagramEClass, WORKFLOW_DIAGRAM__PROCESS_OUTPUT);
    createEReference(workflowDiagramEClass, WORKFLOW_DIAGRAM__PROCESS_OUTPUT_OPT);
    createEReference(workflowDiagramEClass, WORKFLOW_DIAGRAM__DIAGRAM_ARCS);
    createEAttribute(workflowDiagramEClass, WORKFLOW_DIAGRAM__NAME);
    createEReference(workflowDiagramEClass, WORKFLOW_DIAGRAM__PROCESS_INPUT);
    createEReference(workflowDiagramEClass, WORKFLOW_DIAGRAM__PROCESS_INPUT_OPT);
    createEReference(workflowDiagramEClass, WORKFLOW_DIAGRAM__PROCESS_CALLS);

    diagramNodeEClass = createEClass(DIAGRAM_NODE);
    createEAttribute(diagramNodeEClass, DIAGRAM_NODE__LABEL);
    createEReference(diagramNodeEClass, DIAGRAM_NODE__INCOMING);
    createEReference(diagramNodeEClass, DIAGRAM_NODE__OUTGOING);

    controlFlowNodeEClass = createEClass(CONTROL_FLOW_NODE);

    activityNodeEClass = createEClass(ACTIVITY_NODE);
    createEReference(activityNodeEClass, ACTIVITY_NODE__ACTIVITY_OUTPUT);
    createEReference(activityNodeEClass, ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT);
    createEAttribute(activityNodeEClass, ACTIVITY_NODE__OPTIONAL);
    createEReference(activityNodeEClass, ACTIVITY_NODE__ACTIVITY_INPUT_OPT);
    createEReference(activityNodeEClass, ACTIVITY_NODE__ACTIVITY_INPUT);

    resourceNodeEClass = createEClass(RESOURCE_NODE);

    startNodeEClass = createEClass(START_NODE);

    endnodeEClass = createEClass(ENDNODE);

    simpleNodeEClass = createEClass(SIMPLE_NODE);

    structuredNodeEClass = createEClass(STRUCTURED_NODE);
    createEReference(structuredNodeEClass, STRUCTURED_NODE__CHILD_NODES);

    splitNodeEClass = createEClass(SPLIT_NODE);

    mergeNodeEClass = createEClass(MERGE_NODE);

    flowEClass = createEClass(FLOW);
    createEReference(flowEClass, FLOW__START_NODE);
    createEReference(flowEClass, FLOW__END_NODE);

    simpleActivityEClass = createEClass(SIMPLE_ACTIVITY);

    subprocessEClass = createEClass(SUBPROCESS);
    createEReference(subprocessEClass, SUBPROCESS__SUBPROCESS);

    workflowEClass = createEClass(WORKFLOW);
    createEReference(workflowEClass, WORKFLOW__PROCESSES);

    persistentResourceEClass = createEClass(PERSISTENT_RESOURCE);

    transientResourceEClass = createEClass(TRANSIENT_RESOURCE);

    documentEClass = createEClass(DOCUMENT);

    eventEClass = createEClass(EVENT);

    diagramArcEClass = createEClass(DIAGRAM_ARC);
    createEReference(diagramArcEClass, DIAGRAM_ARC__SRC);
    createEReference(diagramArcEClass, DIAGRAM_ARC__TARGET);
    createEAttribute(diagramArcEClass, DIAGRAM_ARC__LABEL);
    createEAttribute(diagramArcEClass, DIAGRAM_ARC__OPTIONAL);
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	private boolean isInitialized = false;

	/**
   * Complete the initialization of the package and its meta-model.  This
   * method is guarded to have no affect on any invocation but its first.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public void initializePackageContents() {
    if (isInitialized) return;
    isInitialized = true;

    // Initialize package
    setName(eNAME);
    setNsPrefix(eNS_PREFIX);
    setNsURI(eNS_URI);

    // Create type parameters

    // Set bounds for type parameters

    // Add supertypes to classes
    controlFlowNodeEClass.getESuperTypes().add(this.getDiagramNode());
    activityNodeEClass.getESuperTypes().add(this.getDiagramNode());
    resourceNodeEClass.getESuperTypes().add(this.getDiagramNode());
    startNodeEClass.getESuperTypes().add(this.getSimpleNode());
    endnodeEClass.getESuperTypes().add(this.getSimpleNode());
    simpleNodeEClass.getESuperTypes().add(this.getControlFlowNode());
    structuredNodeEClass.getESuperTypes().add(this.getControlFlowNode());
    splitNodeEClass.getESuperTypes().add(this.getSimpleNode());
    mergeNodeEClass.getESuperTypes().add(this.getSimpleNode());
    flowEClass.getESuperTypes().add(this.getStructuredNode());
    simpleActivityEClass.getESuperTypes().add(this.getActivityNode());
    subprocessEClass.getESuperTypes().add(this.getActivityNode());
    persistentResourceEClass.getESuperTypes().add(this.getResourceNode());
    transientResourceEClass.getESuperTypes().add(this.getResourceNode());
    documentEClass.getESuperTypes().add(this.getResourceNode());
    eventEClass.getESuperTypes().add(this.getActivityNode());

    // Initialize classes and features; add operations and parameters
    initEClass(workflowDiagramEClass, WorkflowDiagram.class, "WorkflowDiagram", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getWorkflowDiagram_DiagramNodes(), this.getDiagramNode(), null, "diagramNodes", null, 0, -1, WorkflowDiagram.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWorkflowDiagram_StartNode(), this.getStartNode(), null, "startNode", null, 1, 1, WorkflowDiagram.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
    initEReference(getWorkflowDiagram_EndNode(), this.getEndnode(), null, "endNode", null, 1, 1, WorkflowDiagram.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
    initEReference(getWorkflowDiagram_ProcessOutput(), this.getResourceNode(), null, "processOutput", null, 0, -1, WorkflowDiagram.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWorkflowDiagram_ProcessOutputOpt(), this.getResourceNode(), null, "processOutputOpt", null, 0, -1, WorkflowDiagram.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWorkflowDiagram_DiagramArcs(), this.getDiagramArc(), null, "diagramArcs", null, 0, -1, WorkflowDiagram.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getWorkflowDiagram_Name(), ecorePackage.getEString(), "name", null, 0, 1, WorkflowDiagram.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWorkflowDiagram_ProcessInput(), this.getResourceNode(), null, "processInput", null, 0, -1, WorkflowDiagram.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWorkflowDiagram_ProcessInputOpt(), this.getResourceNode(), null, "processInputOpt", null, 0, -1, WorkflowDiagram.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getWorkflowDiagram_ProcessCalls(), this.getSubprocess(), this.getSubprocess_Subprocess(), "processCalls", null, 0, -1, WorkflowDiagram.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(diagramNodeEClass, DiagramNode.class, "DiagramNode", IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(getDiagramNode_Label(), ecorePackage.getEString(), "label", null, 0, 1, DiagramNode.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getDiagramNode_Incoming(), this.getDiagramArc(), this.getDiagramArc_Target(), "incoming", null, 0, -1, DiagramNode.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getDiagramNode_Outgoing(), this.getDiagramArc(), this.getDiagramArc_Src(), "outgoing", null, 0, -1, DiagramNode.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(controlFlowNodeEClass, ControlFlowNode.class, "ControlFlowNode", IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(activityNodeEClass, ActivityNode.class, "ActivityNode", IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getActivityNode_ActivityOutput(), this.getResourceNode(), null, "activityOutput", null, 0, -1, ActivityNode.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
    initEReference(getActivityNode_ActivityOutputOpt(), this.getResourceNode(), null, "activityOutputOpt", null, 0, -1, ActivityNode.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
    initEAttribute(getActivityNode_Optional(), ecorePackage.getEBoolean(), "optional", null, 0, 1, ActivityNode.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getActivityNode_ActivityInputOpt(), this.getResourceNode(), null, "activityInputOpt", null, 0, -1, ActivityNode.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
    initEReference(getActivityNode_ActivityInput(), this.getResourceNode(), null, "activityInput", null, 0, -1, ActivityNode.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);

    initEClass(resourceNodeEClass, ResourceNode.class, "ResourceNode", IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(startNodeEClass, StartNode.class, "StartNode", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(endnodeEClass, Endnode.class, "Endnode", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(simpleNodeEClass, SimpleNode.class, "SimpleNode", IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(structuredNodeEClass, StructuredNode.class, "StructuredNode", IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getStructuredNode_ChildNodes(), this.getDiagramNode(), null, "childNodes", null, 1, -1, StructuredNode.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(splitNodeEClass, SplitNode.class, "SplitNode", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(mergeNodeEClass, MergeNode.class, "MergeNode", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(flowEClass, Flow.class, "Flow", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getFlow_StartNode(), this.getStartNode(), null, "startNode", null, 1, -1, Flow.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);
    initEReference(getFlow_EndNode(), this.getEndnode(), null, "endNode", null, 1, -1, Flow.class, IS_TRANSIENT, IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, IS_DERIVED, IS_ORDERED);

    initEClass(simpleActivityEClass, SimpleActivity.class, "SimpleActivity", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(subprocessEClass, Subprocess.class, "Subprocess", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getSubprocess_Subprocess(), this.getWorkflowDiagram(), this.getWorkflowDiagram_ProcessCalls(), "subprocess", null, 1, 1, Subprocess.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(workflowEClass, Workflow.class, "Workflow", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getWorkflow_Processes(), this.getWorkflowDiagram(), null, "processes", null, 0, -1, Workflow.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, IS_COMPOSITE, !IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    initEClass(persistentResourceEClass, PersistentResource.class, "PersistentResource", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(transientResourceEClass, TransientResource.class, "TransientResource", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(documentEClass, Document.class, "Document", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(eventEClass, Event.class, "Event", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);

    initEClass(diagramArcEClass, DiagramArc.class, "DiagramArc", !IS_ABSTRACT, !IS_INTERFACE, IS_GENERATED_INSTANCE_CLASS);
    initEReference(getDiagramArc_Src(), this.getDiagramNode(), this.getDiagramNode_Outgoing(), "src", null, 1, 1, DiagramArc.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEReference(getDiagramArc_Target(), this.getDiagramNode(), this.getDiagramNode_Incoming(), "target", null, 1, 1, DiagramArc.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_COMPOSITE, IS_RESOLVE_PROXIES, !IS_UNSETTABLE, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getDiagramArc_Label(), ecorePackage.getEString(), "label", null, 0, 1, DiagramArc.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);
    initEAttribute(getDiagramArc_Optional(), ecorePackage.getEBoolean(), "optional", "false", 1, 1, DiagramArc.class, !IS_TRANSIENT, !IS_VOLATILE, IS_CHANGEABLE, !IS_UNSETTABLE, !IS_ID, IS_UNIQUE, !IS_DERIVED, IS_ORDERED);

    // Create resource
    createResource(eNS_URI);
  }

} //FlowChartPackageImpl

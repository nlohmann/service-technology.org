/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.util;

import hub.top.lang.flowcharts.*;

import java.util.List;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy.
 * It supports the call {@link #doSwitch(EObject) doSwitch(object)}
 * to invoke the <code>caseXXX</code> method for each class of the model,
 * starting with the actual class of the object
 * and proceeding up the inheritance hierarchy
 * until a non-null result is returned,
 * which is the result of the switch.
 * <!-- end-user-doc -->
 * @see hub.top.lang.flowcharts.FlowChartPackage
 * @generated
 */
public class FlowChartSwitch<T> {
	/**
   * The cached model package
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	protected static FlowChartPackage modelPackage;

	/**
   * Creates an instance of the switch.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public FlowChartSwitch() {
    if (modelPackage == null) {
      modelPackage = FlowChartPackage.eINSTANCE;
    }
  }

	/**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
	public T doSwitch(EObject theEObject) {
    return doSwitch(theEObject.eClass(), theEObject);
  }

	/**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
	protected T doSwitch(EClass theEClass, EObject theEObject) {
    if (theEClass.eContainer() == modelPackage) {
      return doSwitch(theEClass.getClassifierID(), theEObject);
    }
    else {
      List<EClass> eSuperTypes = theEClass.getESuperTypes();
      return
        eSuperTypes.isEmpty() ?
          defaultCase(theEObject) :
          doSwitch(eSuperTypes.get(0), theEObject);
    }
  }

	/**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
	protected T doSwitch(int classifierID, EObject theEObject) {
    switch (classifierID) {
      case FlowChartPackage.WORKFLOW_DIAGRAM: {
        WorkflowDiagram workflowDiagram = (WorkflowDiagram)theEObject;
        T result = caseWorkflowDiagram(workflowDiagram);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.DIAGRAM_NODE: {
        DiagramNode diagramNode = (DiagramNode)theEObject;
        T result = caseDiagramNode(diagramNode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.CONTROL_FLOW_NODE: {
        ControlFlowNode controlFlowNode = (ControlFlowNode)theEObject;
        T result = caseControlFlowNode(controlFlowNode);
        if (result == null) result = caseDiagramNode(controlFlowNode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.ACTIVITY_NODE: {
        ActivityNode activityNode = (ActivityNode)theEObject;
        T result = caseActivityNode(activityNode);
        if (result == null) result = caseDiagramNode(activityNode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.RESOURCE_NODE: {
        ResourceNode resourceNode = (ResourceNode)theEObject;
        T result = caseResourceNode(resourceNode);
        if (result == null) result = caseDiagramNode(resourceNode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.START_NODE: {
        StartNode startNode = (StartNode)theEObject;
        T result = caseStartNode(startNode);
        if (result == null) result = caseSimpleNode(startNode);
        if (result == null) result = caseControlFlowNode(startNode);
        if (result == null) result = caseDiagramNode(startNode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.ENDNODE: {
        Endnode endnode = (Endnode)theEObject;
        T result = caseEndnode(endnode);
        if (result == null) result = caseSimpleNode(endnode);
        if (result == null) result = caseControlFlowNode(endnode);
        if (result == null) result = caseDiagramNode(endnode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.SIMPLE_NODE: {
        SimpleNode simpleNode = (SimpleNode)theEObject;
        T result = caseSimpleNode(simpleNode);
        if (result == null) result = caseControlFlowNode(simpleNode);
        if (result == null) result = caseDiagramNode(simpleNode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.STRUCTURED_NODE: {
        StructuredNode structuredNode = (StructuredNode)theEObject;
        T result = caseStructuredNode(structuredNode);
        if (result == null) result = caseControlFlowNode(structuredNode);
        if (result == null) result = caseDiagramNode(structuredNode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.SPLIT_NODE: {
        SplitNode splitNode = (SplitNode)theEObject;
        T result = caseSplitNode(splitNode);
        if (result == null) result = caseSimpleNode(splitNode);
        if (result == null) result = caseControlFlowNode(splitNode);
        if (result == null) result = caseDiagramNode(splitNode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.MERGE_NODE: {
        MergeNode mergeNode = (MergeNode)theEObject;
        T result = caseMergeNode(mergeNode);
        if (result == null) result = caseSimpleNode(mergeNode);
        if (result == null) result = caseControlFlowNode(mergeNode);
        if (result == null) result = caseDiagramNode(mergeNode);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.FLOW: {
        Flow flow = (Flow)theEObject;
        T result = caseFlow(flow);
        if (result == null) result = caseStructuredNode(flow);
        if (result == null) result = caseControlFlowNode(flow);
        if (result == null) result = caseDiagramNode(flow);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.SIMPLE_ACTIVITY: {
        SimpleActivity simpleActivity = (SimpleActivity)theEObject;
        T result = caseSimpleActivity(simpleActivity);
        if (result == null) result = caseActivityNode(simpleActivity);
        if (result == null) result = caseDiagramNode(simpleActivity);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.SUBPROCESS: {
        Subprocess subprocess = (Subprocess)theEObject;
        T result = caseSubprocess(subprocess);
        if (result == null) result = caseActivityNode(subprocess);
        if (result == null) result = caseDiagramNode(subprocess);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.WORKFLOW: {
        Workflow workflow = (Workflow)theEObject;
        T result = caseWorkflow(workflow);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.PERSISTENT_RESOURCE: {
        PersistentResource persistentResource = (PersistentResource)theEObject;
        T result = casePersistentResource(persistentResource);
        if (result == null) result = caseResourceNode(persistentResource);
        if (result == null) result = caseDiagramNode(persistentResource);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.TRANSIENT_RESOURCE: {
        TransientResource transientResource = (TransientResource)theEObject;
        T result = caseTransientResource(transientResource);
        if (result == null) result = caseResourceNode(transientResource);
        if (result == null) result = caseDiagramNode(transientResource);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.DOCUMENT: {
        Document document = (Document)theEObject;
        T result = caseDocument(document);
        if (result == null) result = caseResourceNode(document);
        if (result == null) result = caseDiagramNode(document);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.EVENT: {
        Event event = (Event)theEObject;
        T result = caseEvent(event);
        if (result == null) result = caseActivityNode(event);
        if (result == null) result = caseDiagramNode(event);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      case FlowChartPackage.DIAGRAM_ARC: {
        DiagramArc diagramArc = (DiagramArc)theEObject;
        T result = caseDiagramArc(diagramArc);
        if (result == null) result = defaultCase(theEObject);
        return result;
      }
      default: return defaultCase(theEObject);
    }
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Workflow Diagram</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Workflow Diagram</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseWorkflowDiagram(WorkflowDiagram object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Diagram Node</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Diagram Node</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseDiagramNode(DiagramNode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Control Flow Node</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Control Flow Node</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseControlFlowNode(ControlFlowNode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Activity Node</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Activity Node</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseActivityNode(ActivityNode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Resource Node</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Resource Node</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseResourceNode(ResourceNode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Start Node</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Start Node</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseStartNode(StartNode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Endnode</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Endnode</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseEndnode(Endnode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Simple Node</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Simple Node</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseSimpleNode(SimpleNode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Structured Node</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Structured Node</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseStructuredNode(StructuredNode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Split Node</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Split Node</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseSplitNode(SplitNode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Merge Node</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Merge Node</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseMergeNode(MergeNode object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Flow</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Flow</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseFlow(Flow object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Simple Activity</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Simple Activity</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseSimpleActivity(SimpleActivity object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Subprocess</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Subprocess</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseSubprocess(Subprocess object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Workflow</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Workflow</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseWorkflow(Workflow object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Persistent Resource</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Persistent Resource</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T casePersistentResource(PersistentResource object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Transient Resource</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Transient Resource</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseTransientResource(TransientResource object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Document</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Document</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseDocument(Document object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Event</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Event</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseEvent(Event object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>Diagram Arc</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Diagram Arc</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
	public T caseDiagramArc(DiagramArc object) {
    return null;
  }

	/**
   * Returns the result of interpreting the object as an instance of '<em>EObject</em>'.
   * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch, but this is the last case anyway.
	 * <!-- end-user-doc -->
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>EObject</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject)
   * @generated
   */
	public T defaultCase(EObject object) {
    return null;
  }

} //FlowChartSwitch

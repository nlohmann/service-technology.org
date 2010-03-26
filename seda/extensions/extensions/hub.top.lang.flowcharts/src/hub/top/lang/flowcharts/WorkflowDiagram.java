/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Workflow Diagram</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getDiagramNodes <em>Diagram Nodes</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getStartNode <em>Start Node</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getEndNode <em>End Node</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessOutput <em>Process Output</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessOutputOpt <em>Process Output Opt</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getDiagramArcs <em>Diagram Arcs</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getName <em>Name</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessInput <em>Process Input</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessInputOpt <em>Process Input Opt</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessCalls <em>Process Calls</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram()
 * @model
 * @generated
 */
public interface WorkflowDiagram extends EObject {
	/**
	 * Returns the value of the '<em><b>Diagram Nodes</b></em>' containment reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.DiagramNode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Diagram Nodes</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Diagram Nodes</em>' containment reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_DiagramNodes()
	 * @model containment="true"
	 * @generated
	 */
	EList<DiagramNode> getDiagramNodes();

	/**
	 * Returns the value of the '<em><b>Start Node</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Start Node</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Start Node</em>' reference.
	 * @see #setStartNode(StartNode)
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_StartNode()
	 * @model required="true" transient="true" volatile="true" derived="true"
	 * @generated
	 */
	StartNode getStartNode();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.WorkflowDiagram#getStartNode <em>Start Node</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Start Node</em>' reference.
	 * @see #getStartNode()
	 * @generated
	 */
	void setStartNode(StartNode value);

	/**
	 * Returns the value of the '<em><b>End Node</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>End Node</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>End Node</em>' reference.
	 * @see #setEndNode(Endnode)
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_EndNode()
	 * @model required="true" transient="true" volatile="true" derived="true"
	 * @generated
	 */
	Endnode getEndNode();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.WorkflowDiagram#getEndNode <em>End Node</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>End Node</em>' reference.
	 * @see #getEndNode()
	 * @generated
	 */
	void setEndNode(Endnode value);

	/**
	 * Returns the value of the '<em><b>Process Output</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.ResourceNode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Process Output</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Process Output</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_ProcessOutput()
	 * @model
	 * @generated
	 */
	EList<ResourceNode> getProcessOutput();

	/**
	 * Returns the value of the '<em><b>Process Output Opt</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.ResourceNode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Process Output Opt</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Process Output Opt</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_ProcessOutputOpt()
	 * @model
	 * @generated
	 */
	EList<ResourceNode> getProcessOutputOpt();

	/**
	 * Returns the value of the '<em><b>Diagram Arcs</b></em>' containment reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.DiagramArc}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Diagram Arcs</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Diagram Arcs</em>' containment reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_DiagramArcs()
	 * @model containment="true"
	 * @generated
	 */
	EList<DiagramArc> getDiagramArcs();

	/**
	 * Returns the value of the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Name</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Name</em>' attribute.
	 * @see #setName(String)
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_Name()
	 * @model
	 * @generated
	 */
	String getName();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.WorkflowDiagram#getName <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Name</em>' attribute.
	 * @see #getName()
	 * @generated
	 */
	void setName(String value);

  /**
	 * Returns the value of the '<em><b>Process Input</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.ResourceNode}.
	 * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Process Input</em>' reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
	 * @return the value of the '<em>Process Input</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_ProcessInput()
	 * @model
	 * @generated
	 */
  EList<ResourceNode> getProcessInput();

  /**
	 * Returns the value of the '<em><b>Process Input Opt</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.ResourceNode}.
	 * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Process Input Opt</em>' reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
	 * @return the value of the '<em>Process Input Opt</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_ProcessInputOpt()
	 * @model
	 * @generated
	 */
  EList<ResourceNode> getProcessInputOpt();

  /**
	 * Returns the value of the '<em><b>Process Calls</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.Subprocess}.
	 * It is bidirectional and its opposite is '{@link hub.top.lang.flowcharts.Subprocess#getSubprocess <em>Subprocess</em>}'.
	 * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Process Calls</em>' reference list isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
	 * @return the value of the '<em>Process Calls</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflowDiagram_ProcessCalls()
	 * @see hub.top.lang.flowcharts.Subprocess#getSubprocess
	 * @model opposite="subprocess"
	 * @generated
	 */
  EList<Subprocess> getProcessCalls();

} // WorkflowDiagram

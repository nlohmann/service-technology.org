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
 * A representation of the model object '<em><b>Workflow</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.Workflow#getProcesses <em>Processes</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflow()
 * @model
 * @generated
 */
public interface Workflow extends EObject {
	/**
	 * Returns the value of the '<em><b>Processes</b></em>' containment reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.WorkflowDiagram}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Processes</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Processes</em>' containment reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getWorkflow_Processes()
	 * @model containment="true"
	 * @generated
	 */
	EList<WorkflowDiagram> getProcesses();

} // Workflow

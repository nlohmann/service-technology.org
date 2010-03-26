/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Subprocess</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.Subprocess#getSubprocess <em>Subprocess</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.Subprocess#getWeight <em>Weight</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.lang.flowcharts.FlowPackage#getSubprocess()
 * @model
 * @generated
 */
public interface Subprocess extends ActivityNode {
	/**
	 * Returns the value of the '<em><b>Subprocess</b></em>' reference.
	 * It is bidirectional and its opposite is '{@link hub.top.lang.flowcharts.WorkflowDiagram#getProcessCalls <em>Process Calls</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Subprocess</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Subprocess</em>' reference.
	 * @see #setSubprocess(WorkflowDiagram)
	 * @see hub.top.lang.flowcharts.FlowPackage#getSubprocess_Subprocess()
	 * @see hub.top.lang.flowcharts.WorkflowDiagram#getProcessCalls
	 * @model opposite="processCalls" required="true"
	 * @generated
	 */
	WorkflowDiagram getSubprocess();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.Subprocess#getSubprocess <em>Subprocess</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Subprocess</em>' reference.
	 * @see #getSubprocess()
	 * @generated
	 */
	void setSubprocess(WorkflowDiagram value);

	/**
	 * Returns the value of the '<em><b>Weight</b></em>' attribute.
	 * The default value is <code>"1"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Weight</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Weight</em>' attribute.
	 * @see #setWeight(int)
	 * @see hub.top.lang.flowcharts.FlowPackage#getSubprocess_Weight()
	 * @model default="1"
	 * @generated
	 */
	int getWeight();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.Subprocess#getWeight <em>Weight</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Weight</em>' attribute.
	 * @see #getWeight()
	 * @generated
	 */
	void setWeight(int value);

} // Subprocess

/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Flow</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.Flow#getStartNode <em>Start Node</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.Flow#getEndNode <em>End Node</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.lang.flowcharts.FlowPackage#getFlow()
 * @model
 * @generated
 */
public interface Flow extends StructuredNode {
	/**
	 * Returns the value of the '<em><b>Start Node</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.StartNode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Start Node</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Start Node</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getFlow_StartNode()
	 * @model required="true" transient="true" volatile="true" derived="true"
	 * @generated
	 */
	EList<StartNode> getStartNode();

	/**
	 * Returns the value of the '<em><b>End Node</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.Endnode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>End Node</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>End Node</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getFlow_EndNode()
	 * @model required="true" transient="true" volatile="true" derived="true"
	 * @generated
	 */
	EList<Endnode> getEndNode();

} // Flow

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
 * A representation of the model object '<em><b>Activity Node</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.ActivityNode#getActivityOutput <em>Activity Output</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.ActivityNode#getActivityOutputOpt <em>Activity Output Opt</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.ActivityNode#isOptional <em>Optional</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.ActivityNode#getActivityInputOpt <em>Activity Input Opt</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.ActivityNode#getActivityInput <em>Activity Input</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.ActivityNode#getMinTime <em>Min Time</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.ActivityNode#getCost <em>Cost</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.ActivityNode#getProbability <em>Probability</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.ActivityNode#getMaxTime <em>Max Time</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode()
 * @model abstract="true"
 * @generated
 */
public interface ActivityNode extends DiagramNode {
	/**
	 * Returns the value of the '<em><b>Activity Output</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.ResourceNode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Activity Output</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Activity Output</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode_ActivityOutput()
	 * @model transient="true" volatile="true" derived="true"
	 * @generated
	 */
	EList<ResourceNode> getActivityOutput();

	/**
	 * Returns the value of the '<em><b>Activity Output Opt</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.ResourceNode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Activity Output Opt</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Activity Output Opt</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode_ActivityOutputOpt()
	 * @model transient="true" volatile="true" derived="true"
	 * @generated
	 */
	EList<ResourceNode> getActivityOutputOpt();

	/**
	 * Returns the value of the '<em><b>Optional</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Optional</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Optional</em>' attribute.
	 * @see #setOptional(boolean)
	 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode_Optional()
	 * @model
	 * @generated
	 */
	boolean isOptional();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.ActivityNode#isOptional <em>Optional</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Optional</em>' attribute.
	 * @see #isOptional()
	 * @generated
	 */
	void setOptional(boolean value);

	/**
	 * Returns the value of the '<em><b>Activity Input Opt</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.ResourceNode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Activity Input Opt</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Activity Input Opt</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode_ActivityInputOpt()
	 * @model transient="true" volatile="true" derived="true"
	 * @generated
	 */
	EList<ResourceNode> getActivityInputOpt();

	/**
	 * Returns the value of the '<em><b>Activity Input</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.ResourceNode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Activity Input</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Activity Input</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode_ActivityInput()
	 * @model transient="true" volatile="true" derived="true"
	 * @generated
	 */
	EList<ResourceNode> getActivityInput();

  /**
	 * Returns the value of the '<em><b>Min Time</b></em>' attribute.
	 * The default value is <code>"0"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Min Time</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Min Time</em>' attribute.
	 * @see #setMinTime(int)
	 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode_MinTime()
	 * @model default="0"
	 * @generated
	 */
	int getMinTime();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.ActivityNode#getMinTime <em>Min Time</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Min Time</em>' attribute.
	 * @see #getMinTime()
	 * @generated
	 */
	void setMinTime(int value);

		/**
	 * Returns the value of the '<em><b>Cost</b></em>' attribute.
	 * The default value is <code>"0"</code>.
	 * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Cost</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
	 * @return the value of the '<em>Cost</em>' attribute.
	 * @see #setCost(int)
	 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode_Cost()
	 * @model default="0"
	 * @generated
	 */
  int getCost();

  /**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.ActivityNode#getCost <em>Cost</em>}' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Cost</em>' attribute.
	 * @see #getCost()
	 * @generated
	 */
  void setCost(int value);

  /**
	 * Returns the value of the '<em><b>Probability</b></em>' attribute.
	 * The default value is <code>"1.0"</code>.
	 * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Probability</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
	 * @return the value of the '<em>Probability</em>' attribute.
	 * @see #setProbability(double)
	 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode_Probability()
	 * @model default="1.0"
	 * @generated
	 */
  double getProbability();

  /**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.ActivityNode#getProbability <em>Probability</em>}' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Probability</em>' attribute.
	 * @see #getProbability()
	 * @generated
	 */
  void setProbability(double value);

		/**
	 * Returns the value of the '<em><b>Max Time</b></em>' attribute.
	 * The default value is <code>"0"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Max Time</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Max Time</em>' attribute.
	 * @see #setMaxTime(int)
	 * @see hub.top.lang.flowcharts.FlowPackage#getActivityNode_MaxTime()
	 * @model default="0"
	 * @generated
	 */
	int getMaxTime();

		/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.ActivityNode#getMaxTime <em>Max Time</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Max Time</em>' attribute.
	 * @see #getMaxTime()
	 * @generated
	 */
	void setMaxTime(int value);

} // ActivityNode

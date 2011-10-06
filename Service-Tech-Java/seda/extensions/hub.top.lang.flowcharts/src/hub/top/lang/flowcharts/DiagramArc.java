/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Diagram Arc</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.DiagramArc#getSrc <em>Src</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.DiagramArc#getTarget <em>Target</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.DiagramArc#getLabel <em>Label</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.DiagramArc#isOptional <em>Optional</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.DiagramArc#getProbability <em>Probability</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.DiagramArc#getMinWeight <em>Min Weight</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.DiagramArc#getMaxWeight <em>Max Weight</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramArc()
 * @model
 * @generated
 */
public interface DiagramArc extends EObject {
	/**
	 * Returns the value of the '<em><b>Src</b></em>' reference.
	 * It is bidirectional and its opposite is '{@link hub.top.lang.flowcharts.DiagramNode#getOutgoing <em>Outgoing</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Src</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Src</em>' reference.
	 * @see #setSrc(DiagramNode)
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramArc_Src()
	 * @see hub.top.lang.flowcharts.DiagramNode#getOutgoing
	 * @model opposite="outgoing" required="true"
	 * @generated
	 */
	DiagramNode getSrc();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.DiagramArc#getSrc <em>Src</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Src</em>' reference.
	 * @see #getSrc()
	 * @generated
	 */
	void setSrc(DiagramNode value);

	/**
	 * Returns the value of the '<em><b>Target</b></em>' reference.
	 * It is bidirectional and its opposite is '{@link hub.top.lang.flowcharts.DiagramNode#getIncoming <em>Incoming</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Target</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Target</em>' reference.
	 * @see #setTarget(DiagramNode)
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramArc_Target()
	 * @see hub.top.lang.flowcharts.DiagramNode#getIncoming
	 * @model opposite="incoming" required="true"
	 * @generated
	 */
	DiagramNode getTarget();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.DiagramArc#getTarget <em>Target</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Target</em>' reference.
	 * @see #getTarget()
	 * @generated
	 */
	void setTarget(DiagramNode value);

	/**
	 * Returns the value of the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Label</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Label</em>' attribute.
	 * @see #setLabel(String)
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramArc_Label()
	 * @model
	 * @generated
	 */
	String getLabel();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.DiagramArc#getLabel <em>Label</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Label</em>' attribute.
	 * @see #getLabel()
	 * @generated
	 */
	void setLabel(String value);

  /**
	 * Returns the value of the '<em><b>Optional</b></em>' attribute.
	 * The default value is <code>"false"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Optional</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Optional</em>' attribute.
	 * @see #setOptional(boolean)
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramArc_Optional()
	 * @model default="false" required="true"
	 * @generated
	 */
	boolean isOptional();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.DiagramArc#isOptional <em>Optional</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Optional</em>' attribute.
	 * @see #isOptional()
	 * @generated
	 */
	void setOptional(boolean value);

		/**
	 * Returns the value of the '<em><b>Min Weight</b></em>' attribute.
	 * The default value is <code>"1"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Min Weight</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Min Weight</em>' attribute.
	 * @see #setMinWeight(int)
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramArc_MinWeight()
	 * @model default="1"
	 * @generated
	 */
	int getMinWeight();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.DiagramArc#getMinWeight <em>Min Weight</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Min Weight</em>' attribute.
	 * @see #getMinWeight()
	 * @generated
	 */
	void setMinWeight(int value);

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
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramArc_Probability()
	 * @model default="1.0"
	 * @generated
	 */
  double getProbability();

  /**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.DiagramArc#getProbability <em>Probability</em>}' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Probability</em>' attribute.
	 * @see #getProbability()
	 * @generated
	 */
  void setProbability(double value);

		/**
	 * Returns the value of the '<em><b>Max Weight</b></em>' attribute.
	 * The default value is <code>"1"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Max Weight</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Max Weight</em>' attribute.
	 * @see #setMaxWeight(int)
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramArc_MaxWeight()
	 * @model default="1"
	 * @generated
	 */
	int getMaxWeight();

		/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.DiagramArc#getMaxWeight <em>Max Weight</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Max Weight</em>' attribute.
	 * @see #getMaxWeight()
	 * @generated
	 */
	void setMaxWeight(int value);

} // DiagramArc

/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Pt Net</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.editor.ptnetLoLA.PtNet#getPlaces <em>Places</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.PtNet#getTransitions <em>Transitions</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.PtNet#getInitialMarking <em>Initial Marking</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.PtNet#getAnnotation <em>Annotation</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.PtNet#getArcs <em>Arcs</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.PtNet#getFinalMarking <em>Final Marking</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPtNet()
 * @model
 * @generated
 */
public interface PtNet extends EObject {
	/**
	 * Returns the value of the '<em><b>Places</b></em>' containment reference list.
	 * The list contents are of type {@link hub.top.editor.ptnetLoLA.Place}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Places</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Places</em>' containment reference list.
	 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPtNet_Places()
	 * @model containment="true"
	 * @generated
	 */
	EList<Place> getPlaces();

	/**
	 * Returns the value of the '<em><b>Transitions</b></em>' containment reference list.
	 * The list contents are of type {@link hub.top.editor.ptnetLoLA.Transition}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Transitions</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Transitions</em>' containment reference list.
	 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPtNet_Transitions()
	 * @model containment="true"
	 * @generated
	 */
	EList<Transition> getTransitions();

	/**
	 * Returns the value of the '<em><b>Initial Marking</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Initial Marking</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Initial Marking</em>' containment reference.
	 * @see #setInitialMarking(Marking)
	 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPtNet_InitialMarking()
	 * @model containment="true" required="true"
	 * @generated
	 */
	Marking getInitialMarking();

	/**
	 * Sets the value of the '{@link hub.top.editor.ptnetLoLA.PtNet#getInitialMarking <em>Initial Marking</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Initial Marking</em>' containment reference.
	 * @see #getInitialMarking()
	 * @generated
	 */
	void setInitialMarking(Marking value);

	/**
	 * Returns the value of the '<em><b>Annotation</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Annotation</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Annotation</em>' containment reference.
	 * @see #setAnnotation(Annotation)
	 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPtNet_Annotation()
	 * @model containment="true"
	 * @generated
	 */
	Annotation getAnnotation();

	/**
	 * Sets the value of the '{@link hub.top.editor.ptnetLoLA.PtNet#getAnnotation <em>Annotation</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Annotation</em>' containment reference.
	 * @see #getAnnotation()
	 * @generated
	 */
	void setAnnotation(Annotation value);

	/**
	 * Returns the value of the '<em><b>Arcs</b></em>' containment reference list.
	 * The list contents are of type {@link hub.top.editor.ptnetLoLA.Arc}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Arcs</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Arcs</em>' containment reference list.
	 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPtNet_Arcs()
	 * @model containment="true"
	 * @generated
	 */
	EList<Arc> getArcs();

	/**
	 * Returns the value of the '<em><b>Final Marking</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Final Marking</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Final Marking</em>' containment reference.
	 * @see #setFinalMarking(Marking)
	 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPtNet_FinalMarking()
	 * @model containment="true"
	 * @generated
	 */
	Marking getFinalMarking();

	/**
	 * Sets the value of the '{@link hub.top.editor.ptnetLoLA.PtNet#getFinalMarking <em>Final Marking</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Final Marking</em>' containment reference.
	 * @see #getFinalMarking()
	 * @generated
	 */
	void setFinalMarking(Marking value);

} // PtNet

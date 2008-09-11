/**
 * Humboldt Universität zu Berlin, Thoery of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Adaptive Process</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.AdaptiveProcess#getMarkedConditions <em>Marked Conditions</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getAdaptiveProcess()
 * @model
 * @generated
 */
public interface AdaptiveProcess extends OccurrenceNet {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String copyright = "Humboldt Universit\u00e4t zu Berlin, Thoery of Programming";

	/**
	 * Returns the value of the '<em><b>Marked Conditions</b></em>' reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.Condition}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Marked Conditions</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Marked Conditions</em>' reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getAdaptiveProcess_MarkedConditions()
	 * @model transient="true" volatile="true" derived="true"
	 * @generated
	 */
	EList<Condition> getMarkedConditions();

} // AdaptiveProcess

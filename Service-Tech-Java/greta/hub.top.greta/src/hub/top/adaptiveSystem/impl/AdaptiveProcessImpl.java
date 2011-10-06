/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.impl;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Node;

import java.util.Collection;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Adaptive Process</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.impl.AdaptiveProcessImpl#getMarkedConditions <em>Marked Conditions</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class AdaptiveProcessImpl extends OccurrenceNetImpl implements AdaptiveProcess {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected AdaptiveProcessImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return AdaptiveSystemPackage.Literals.ADAPTIVE_PROCESS;
	}

	/**
	 * <!-- begin-user-doc -->
	 * Returns a list of all marked conditions of the adaptive process.
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public EList<Condition> getMarkedConditions() {
		EList<Condition> markedConditions = new BasicEList<Condition>();
		
		for (Node node : this.getNodes()) {
			if(node instanceof Condition) {
				Condition condition = (Condition) node;
				if(condition.isMarked()) markedConditions.add(condition);
			}
		}
		return markedConditions;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case AdaptiveSystemPackage.ADAPTIVE_PROCESS__MARKED_CONDITIONS:
				return getMarkedConditions();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@SuppressWarnings("unchecked")
	@Override
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case AdaptiveSystemPackage.ADAPTIVE_PROCESS__MARKED_CONDITIONS:
				getMarkedConditions().clear();
				getMarkedConditions().addAll((Collection<? extends Condition>)newValue);
				return;
		}
		super.eSet(featureID, newValue);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eUnset(int featureID) {
		switch (featureID) {
			case AdaptiveSystemPackage.ADAPTIVE_PROCESS__MARKED_CONDITIONS:
				getMarkedConditions().clear();
				return;
		}
		super.eUnset(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public boolean eIsSet(int featureID) {
		switch (featureID) {
			case AdaptiveSystemPackage.ADAPTIVE_PROCESS__MARKED_CONDITIONS:
				return !getMarkedConditions().isEmpty();
		}
		return super.eIsSet(featureID);
	}

} //AdaptiveProcessImpl

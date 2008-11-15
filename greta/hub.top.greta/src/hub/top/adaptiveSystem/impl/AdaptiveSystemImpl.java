/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.impl;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.Oclet;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Adaptive System</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.impl.AdaptiveSystemImpl#getOclets <em>Oclets</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.AdaptiveSystemImpl#getAdaptiveProcess <em>Adaptive Process</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.AdaptiveSystemImpl#isSetWellformednessToOclets <em>Set Wellformedness To Oclets</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class AdaptiveSystemImpl extends EObjectImpl implements AdaptiveSystem {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * The cached value of the '{@link #getOclets() <em>Oclets</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getOclets()
	 * @generated
	 * @ordered
	 */
	protected EList<Oclet> oclets;

	/**
	 * The cached value of the '{@link #getAdaptiveProcess() <em>Adaptive Process</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getAdaptiveProcess()
	 * @generated
	 * @ordered
	 */
	protected AdaptiveProcess adaptiveProcess;

	/**
	 * The default value of the '{@link #isSetWellformednessToOclets() <em>Set Wellformedness To Oclets</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetWellformednessToOclets()
	 * @generated
	 * @ordered
	 */
	protected static final boolean SET_WELLFORMEDNESS_TO_OCLETS_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isSetWellformednessToOclets() <em>Set Wellformedness To Oclets</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetWellformednessToOclets()
	 * @generated
	 * @ordered
	 */
	protected boolean setWellformednessToOclets = SET_WELLFORMEDNESS_TO_OCLETS_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected AdaptiveSystemImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return AdaptiveSystemPackage.Literals.ADAPTIVE_SYSTEM;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<Oclet> getOclets() {
		if (oclets == null) {
			oclets = new EObjectContainmentEList<Oclet>(Oclet.class, this, AdaptiveSystemPackage.ADAPTIVE_SYSTEM__OCLETS);
		}
		return oclets;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public AdaptiveProcess getAdaptiveProcess() {
		return adaptiveProcess;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetAdaptiveProcess(AdaptiveProcess newAdaptiveProcess, NotificationChain msgs) {
		AdaptiveProcess oldAdaptiveProcess = adaptiveProcess;
		adaptiveProcess = newAdaptiveProcess;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS, oldAdaptiveProcess, newAdaptiveProcess);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setAdaptiveProcess(AdaptiveProcess newAdaptiveProcess) {
		if (newAdaptiveProcess != adaptiveProcess) {
			NotificationChain msgs = null;
			if (adaptiveProcess != null)
				msgs = ((InternalEObject)adaptiveProcess).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - AdaptiveSystemPackage.ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS, null, msgs);
			if (newAdaptiveProcess != null)
				msgs = ((InternalEObject)newAdaptiveProcess).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - AdaptiveSystemPackage.ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS, null, msgs);
			msgs = basicSetAdaptiveProcess(newAdaptiveProcess, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS, newAdaptiveProcess, newAdaptiveProcess));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean isSetWellformednessToOclets() {
		return setWellformednessToOclets;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setSetWellformednessToOclets(boolean newSetWellformednessToOclets) {
		boolean oldSetWellformednessToOclets = setWellformednessToOclets;
		setWellformednessToOclets = newSetWellformednessToOclets;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ADAPTIVE_SYSTEM__SET_WELLFORMEDNESS_TO_OCLETS, oldSetWellformednessToOclets, setWellformednessToOclets));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__OCLETS:
				return ((InternalEList<?>)getOclets()).basicRemove(otherEnd, msgs);
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS:
				return basicSetAdaptiveProcess(null, msgs);
		}
		return super.eInverseRemove(otherEnd, featureID, msgs);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__OCLETS:
				return getOclets();
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS:
				return getAdaptiveProcess();
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__SET_WELLFORMEDNESS_TO_OCLETS:
				return isSetWellformednessToOclets() ? Boolean.TRUE : Boolean.FALSE;
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
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__OCLETS:
				getOclets().clear();
				getOclets().addAll((Collection<? extends Oclet>)newValue);
				return;
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS:
				setAdaptiveProcess((AdaptiveProcess)newValue);
				return;
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__SET_WELLFORMEDNESS_TO_OCLETS:
				setSetWellformednessToOclets(((Boolean)newValue).booleanValue());
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
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__OCLETS:
				getOclets().clear();
				return;
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS:
				setAdaptiveProcess((AdaptiveProcess)null);
				return;
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__SET_WELLFORMEDNESS_TO_OCLETS:
				setSetWellformednessToOclets(SET_WELLFORMEDNESS_TO_OCLETS_EDEFAULT);
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
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__OCLETS:
				return oclets != null && !oclets.isEmpty();
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS:
				return adaptiveProcess != null;
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM__SET_WELLFORMEDNESS_TO_OCLETS:
				return setWellformednessToOclets != SET_WELLFORMEDNESS_TO_OCLETS_EDEFAULT;
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String toString() {
		if (eIsProxy()) return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (setWellformednessToOclets: ");
		result.append(setWellformednessToOclets);
		result.append(')');
		return result.toString();
	}

} //AdaptiveSystemImpl

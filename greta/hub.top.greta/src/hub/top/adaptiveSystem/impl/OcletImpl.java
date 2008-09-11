/**
 * Humboldt Universität zu Berlin, Thoery of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.impl;

import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.Orientation;
import hub.top.adaptiveSystem.PreNet;
import hub.top.adaptiveSystem.Quantor;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Oclet</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.impl.OcletImpl#getName <em>Name</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.OcletImpl#getOrientation <em>Orientation</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.OcletImpl#getQuantor <em>Quantor</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.OcletImpl#getPreNet <em>Pre Net</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.OcletImpl#getDoNet <em>Do Net</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.OcletImpl#isWellFormed <em>Well Formed</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class OcletImpl extends EObjectImpl implements Oclet {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Thoery of Programming";

	/**
	 * The default value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected static final String NAME_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected String name = NAME_EDEFAULT;

	/**
	 * The default value of the '{@link #getOrientation() <em>Orientation</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getOrientation()
	 * @generated
	 * @ordered
	 */
	protected static final Orientation ORIENTATION_EDEFAULT = Orientation.NORMAL;

	/**
	 * The cached value of the '{@link #getOrientation() <em>Orientation</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getOrientation()
	 * @generated
	 * @ordered
	 */
	protected Orientation orientation = ORIENTATION_EDEFAULT;

	/**
	 * The default value of the '{@link #getQuantor() <em>Quantor</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getQuantor()
	 * @generated
	 * @ordered
	 */
	protected static final Quantor QUANTOR_EDEFAULT = Quantor.UNIVERSAL;

	/**
	 * The cached value of the '{@link #getQuantor() <em>Quantor</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getQuantor()
	 * @generated
	 * @ordered
	 */
	protected Quantor quantor = QUANTOR_EDEFAULT;

	/**
	 * The cached value of the '{@link #getPreNet() <em>Pre Net</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getPreNet()
	 * @generated
	 * @ordered
	 */
	protected PreNet preNet;

	/**
	 * The cached value of the '{@link #getDoNet() <em>Do Net</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getDoNet()
	 * @generated
	 * @ordered
	 */
	protected DoNet doNet;

	/**
	 * The default value of the '{@link #isWellFormed() <em>Well Formed</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isWellFormed()
	 * @generated
	 * @ordered
	 */
	protected static final boolean WELL_FORMED_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isWellFormed() <em>Well Formed</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isWellFormed()
	 * @generated
	 * @ordered
	 */
	protected boolean wellFormed = WELL_FORMED_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected OcletImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return AdaptiveSystemPackage.Literals.OCLET;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setName(String newName) {
		String oldName = name;
		name = newName;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.OCLET__NAME, oldName, name));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Orientation getOrientation() {
		return orientation;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setOrientation(Orientation newOrientation) {
		Orientation oldOrientation = orientation;
		orientation = newOrientation == null ? ORIENTATION_EDEFAULT : newOrientation;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.OCLET__ORIENTATION, oldOrientation, orientation));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Quantor getQuantor() {
		return quantor;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setQuantor(Quantor newQuantor) {
		Quantor oldQuantor = quantor;
		quantor = newQuantor == null ? QUANTOR_EDEFAULT : newQuantor;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.OCLET__QUANTOR, oldQuantor, quantor));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public PreNet getPreNet() {
		return preNet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetPreNet(PreNet newPreNet, NotificationChain msgs) {
		PreNet oldPreNet = preNet;
		preNet = newPreNet;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.OCLET__PRE_NET, oldPreNet, newPreNet);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setPreNet(PreNet newPreNet) {
		if (newPreNet != preNet) {
			NotificationChain msgs = null;
			if (preNet != null)
				msgs = ((InternalEObject)preNet).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - AdaptiveSystemPackage.OCLET__PRE_NET, null, msgs);
			if (newPreNet != null)
				msgs = ((InternalEObject)newPreNet).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - AdaptiveSystemPackage.OCLET__PRE_NET, null, msgs);
			msgs = basicSetPreNet(newPreNet, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.OCLET__PRE_NET, newPreNet, newPreNet));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public DoNet getDoNet() {
		return doNet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetDoNet(DoNet newDoNet, NotificationChain msgs) {
		DoNet oldDoNet = doNet;
		doNet = newDoNet;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.OCLET__DO_NET, oldDoNet, newDoNet);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setDoNet(DoNet newDoNet) {
		if (newDoNet != doNet) {
			NotificationChain msgs = null;
			if (doNet != null)
				msgs = ((InternalEObject)doNet).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - AdaptiveSystemPackage.OCLET__DO_NET, null, msgs);
			if (newDoNet != null)
				msgs = ((InternalEObject)newDoNet).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - AdaptiveSystemPackage.OCLET__DO_NET, null, msgs);
			msgs = basicSetDoNet(newDoNet, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.OCLET__DO_NET, newDoNet, newDoNet));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean isWellFormed() {
		return wellFormed;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setWellFormed(boolean newWellFormed) {
		boolean oldWellFormed = wellFormed;
		wellFormed = newWellFormed;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.OCLET__WELL_FORMED, oldWellFormed, wellFormed));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case AdaptiveSystemPackage.OCLET__PRE_NET:
				return basicSetPreNet(null, msgs);
			case AdaptiveSystemPackage.OCLET__DO_NET:
				return basicSetDoNet(null, msgs);
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
			case AdaptiveSystemPackage.OCLET__NAME:
				return getName();
			case AdaptiveSystemPackage.OCLET__ORIENTATION:
				return getOrientation();
			case AdaptiveSystemPackage.OCLET__QUANTOR:
				return getQuantor();
			case AdaptiveSystemPackage.OCLET__PRE_NET:
				return getPreNet();
			case AdaptiveSystemPackage.OCLET__DO_NET:
				return getDoNet();
			case AdaptiveSystemPackage.OCLET__WELL_FORMED:
				return isWellFormed() ? Boolean.TRUE : Boolean.FALSE;
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case AdaptiveSystemPackage.OCLET__NAME:
				setName((String)newValue);
				return;
			case AdaptiveSystemPackage.OCLET__ORIENTATION:
				setOrientation((Orientation)newValue);
				return;
			case AdaptiveSystemPackage.OCLET__QUANTOR:
				setQuantor((Quantor)newValue);
				return;
			case AdaptiveSystemPackage.OCLET__PRE_NET:
				setPreNet((PreNet)newValue);
				return;
			case AdaptiveSystemPackage.OCLET__DO_NET:
				setDoNet((DoNet)newValue);
				return;
			case AdaptiveSystemPackage.OCLET__WELL_FORMED:
				setWellFormed(((Boolean)newValue).booleanValue());
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
			case AdaptiveSystemPackage.OCLET__NAME:
				setName(NAME_EDEFAULT);
				return;
			case AdaptiveSystemPackage.OCLET__ORIENTATION:
				setOrientation(ORIENTATION_EDEFAULT);
				return;
			case AdaptiveSystemPackage.OCLET__QUANTOR:
				setQuantor(QUANTOR_EDEFAULT);
				return;
			case AdaptiveSystemPackage.OCLET__PRE_NET:
				setPreNet((PreNet)null);
				return;
			case AdaptiveSystemPackage.OCLET__DO_NET:
				setDoNet((DoNet)null);
				return;
			case AdaptiveSystemPackage.OCLET__WELL_FORMED:
				setWellFormed(WELL_FORMED_EDEFAULT);
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
			case AdaptiveSystemPackage.OCLET__NAME:
				return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
			case AdaptiveSystemPackage.OCLET__ORIENTATION:
				return orientation != ORIENTATION_EDEFAULT;
			case AdaptiveSystemPackage.OCLET__QUANTOR:
				return quantor != QUANTOR_EDEFAULT;
			case AdaptiveSystemPackage.OCLET__PRE_NET:
				return preNet != null;
			case AdaptiveSystemPackage.OCLET__DO_NET:
				return doNet != null;
			case AdaptiveSystemPackage.OCLET__WELL_FORMED:
				return wellFormed != WELL_FORMED_EDEFAULT;
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
		result.append(" (name: ");
		result.append(name);
		result.append(", orientation: ");
		result.append(orientation);
		result.append(", quantor: ");
		result.append(quantor);
		result.append(", wellFormed: ");
		result.append(wellFormed);
		result.append(')');
		return result.toString();
	}

} //OcletImpl

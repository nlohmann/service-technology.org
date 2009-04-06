/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA.impl;

import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.RefMarkedPlace;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;


/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Ref Marked Place</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.editor.ptnetLoLA.impl.RefMarkedPlaceImpl#getToken <em>Token</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class RefMarkedPlaceImpl extends PlaceReferenceImpl implements RefMarkedPlace {
	/**
   * The default value of the '{@link #getToken() <em>Token</em>}' attribute.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see #getToken()
   * @generated
   * @ordered
   */
	protected static final int TOKEN_EDEFAULT = 1;

	/**
   * The cached value of the '{@link #getToken() <em>Token</em>}' attribute.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see #getToken()
   * @generated
   * @ordered
   */
	protected int token = TOKEN_EDEFAULT;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	protected RefMarkedPlaceImpl() {
    super();
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	@Override
	protected EClass eStaticClass() {
    return PtnetLoLAPackage.Literals.REF_MARKED_PLACE;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public int getToken() {
    return token;
  }

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public void setToken(int newToken) {
		int oldToken = token;
		token = newToken;
		
		// update token value of the referenced place as the value is stored
		// there permanently as well, the call will do nothing if the value
		// of the actual place's token value is already equal to newToken
		if (this.getPlace() != null)
			this.getPlace().setToken(newToken);
		
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, PtnetLoLAPackage.REF_MARKED_PLACE__TOKEN, oldToken, token));
	}

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
    switch (featureID) {
      case PtnetLoLAPackage.REF_MARKED_PLACE__TOKEN:
        return new Integer(getToken());
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
      case PtnetLoLAPackage.REF_MARKED_PLACE__TOKEN:
        setToken(((Integer)newValue).intValue());
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
      case PtnetLoLAPackage.REF_MARKED_PLACE__TOKEN:
        setToken(TOKEN_EDEFAULT);
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
      case PtnetLoLAPackage.REF_MARKED_PLACE__TOKEN:
        return token != TOKEN_EDEFAULT;
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
    result.append(" (token: ");
    result.append(token);
    result.append(')');
    return result.toString();
  }

} //RefMarkedPlaceImpl

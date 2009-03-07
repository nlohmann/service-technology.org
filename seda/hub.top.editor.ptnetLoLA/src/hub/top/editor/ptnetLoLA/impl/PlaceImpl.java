/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - PetriNet Editor Framework
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

package hub.top.editor.ptnetLoLA.impl;

import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.RefMarkedPlace;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;


/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Place</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.editor.ptnetLoLA.impl.PlaceImpl#getToken <em>Token</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.impl.PlaceImpl#getFinalMarking <em>Final Marking</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class PlaceImpl extends NodeImpl implements Place {
	/**
	 * The default value of the '{@link #getToken() <em>Token</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getToken()
	 * @generated
	 * @ordered
	 */
	protected static final int TOKEN_EDEFAULT = 0;

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
	 * The default value of the '{@link #getFinalMarking() <em>Final Marking</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getFinalMarking()
	 * @generated
	 * @ordered
	 */
	protected static final int FINAL_MARKING_EDEFAULT = 0;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected PlaceImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return PtnetLoLAPackage.Literals.PLACE;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public int getToken() {

		PtNet net = (PtNet)eContainer;
		if (this.token == TOKEN_EDEFAULT
			&& net != null
			&& net.getInitialMarking() != null
			&& net.getInitialMarking().getPlaces() != null)
		{
			// for backwards compatibility: if the default value is stored,
			// check if the initialMarking has a corresponding place reference
			for (RefMarkedPlace p : net.getInitialMarking().getPlaces()) {
				if (p.getPlace() == this) {
					// found, copy value
					this.token = p.getToken();
					// return p.getToken();
				}
			}
			//return 0;
		}
		
		return this.token;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated not
	 */
	public void setToken(int newToken) {
		int oldToken = getToken();
		
		// no change, return to avoid infinite updates in interplay with
		// {@link RefMarkedPlace#setToken()}
		if (newToken == this.token)	
			return;
		
		this.token = newToken;	// write the new value locally
		boolean doNotify = updateNetInitialMarking(newToken);

		if (doNotify && eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, PtnetLoLAPackage.PLACE__TOKEN, oldToken, newToken));
	}

	/**
	 * Update token values in the {@link PtNet#getInitialMarking()} of the net
	 * of this place.
	 * 
	 * @param newToken
	 * @return true iff notification about value updates are required
	 * @generated not
	 */
	private boolean updateNetInitialMarking(int newToken) {
		
		// retrieve place reference from the net's initial marking (if it exists)
		PtNet net = (PtNet)eContainer;
		if (net == null)	// in case there is no containing net yet,
			return true;	// do not update the value, but notify
		
		RefMarkedPlace pMarked = null;
		for (RefMarkedPlace p : net.getInitialMarking().getPlaces()) {
			if (p.getPlace() == this) {
				pMarked = p;
				break;
			}
		}

		// update the value of the reference
		boolean doNotify = true;
		if (newToken == 0) {
			// remove the reference if setting the value to 0
			if (pMarked != null) {
				pMarked.setToken(0);
				pMarked.setPlace(null);
				net.getInitialMarking().getPlaces().remove(pMarked);
			} else {
				// nothing to do
				doNotify = false;
			}
		} else if (newToken > 0) {
			// set the value or create a new reference if the value is > 0
			if (pMarked != null) {
				pMarked.setToken(newToken);
			} else {
				RefMarkedPlace p = PtnetLoLAFactory.eINSTANCE.createRefMarkedPlace();
				p.setPlace(this);
				p.setToken(newToken);
				net.getInitialMarking().getPlaces().add(p);
			}
		}
		
		return doNotify;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated not
	 */
	public int getFinalMarking() {
		PtNet net = (PtNet)eContainer;
		for (RefMarkedPlace p : net.getFinalMarking().getPlaces()) {
			if (p.getPlace() == this)
				return p.getToken();
		}
		return 0;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated not
	 */
	public void setFinalMarking(int newFinalMarking) {
		int oldFinalMarking = getFinalMarking();
		
		PtNet net = (PtNet)eContainer;
		RefMarkedPlace pMarked = null;
		for (RefMarkedPlace p : net.getFinalMarking().getPlaces()) {
			if (p.getPlace() == this) {
				pMarked = p;
				break;
			}
		}

		boolean doNotify = true;
		if (newFinalMarking == 0) {
			if (pMarked != null) {
				pMarked.setToken(0);
				pMarked.setPlace(null);
				net.getFinalMarking().getPlaces().remove(pMarked);
			} else {
				// nothing to do
				doNotify = false;
			}
		} else if (newFinalMarking > 0) {
			if (pMarked != null) {
				pMarked.setToken(newFinalMarking);
			} else {
				RefMarkedPlace p = PtnetLoLAFactory.eINSTANCE.createRefMarkedPlace();
				p.setPlace(this);
				p.setToken(newFinalMarking);
				net.getFinalMarking().getPlaces().add(p);
			}
		} 

		if (doNotify && eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, PtnetLoLAPackage.PLACE__FINAL_MARKING, oldFinalMarking, newFinalMarking));

	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case PtnetLoLAPackage.PLACE__TOKEN:
				return new Integer(getToken());
			case PtnetLoLAPackage.PLACE__FINAL_MARKING:
				return new Integer(getFinalMarking());
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
			case PtnetLoLAPackage.PLACE__TOKEN:
				setToken(((Integer)newValue).intValue());
				return;
			case PtnetLoLAPackage.PLACE__FINAL_MARKING:
				setFinalMarking(((Integer)newValue).intValue());
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
			case PtnetLoLAPackage.PLACE__TOKEN:
				setToken(TOKEN_EDEFAULT);
				return;
			case PtnetLoLAPackage.PLACE__FINAL_MARKING:
				setFinalMarking(FINAL_MARKING_EDEFAULT);
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
			case PtnetLoLAPackage.PLACE__TOKEN:
				return token != TOKEN_EDEFAULT;
			case PtnetLoLAPackage.PLACE__FINAL_MARKING:
				return getFinalMarking() != FINAL_MARKING_EDEFAULT;
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

} //PlaceImpl

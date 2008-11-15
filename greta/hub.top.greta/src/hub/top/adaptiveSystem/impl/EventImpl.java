/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.impl;

import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.ArcToEvent;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;

import java.util.Collection;
import java.util.Iterator;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.util.EObjectWithInverseResolvingEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Event</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.impl.EventImpl#getPreConditions <em>Pre Conditions</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.EventImpl#getPostConditions <em>Post Conditions</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.EventImpl#isSaturated <em>Saturated</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.EventImpl#isEnabled <em>Enabled</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.EventImpl#getIncoming <em>Incoming</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.EventImpl#getOutgoing <em>Outgoing</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class EventImpl extends NodeImpl implements Event {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * The cached value of the '{@link #getPreConditions() <em>Pre Conditions</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getPreConditions()
	 * @generated
	 * @ordered
	 */
	protected EList<Condition> preConditions;

	/**
	 * The cached value of the '{@link #getPostConditions() <em>Post Conditions</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getPostConditions()
	 * @generated
	 * @ordered
	 */
	protected EList<Condition> postConditions;

	/**
	 * The default value of the '{@link #isSaturated() <em>Saturated</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSaturated()
	 * @generated
	 * @ordered
	 */
	protected static final boolean SATURATED_EDEFAULT = false;

	/**
	 * The default value of the '{@link #isEnabled() <em>Enabled</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isEnabled()
	 * @generated
	 * @ordered
	 */
	protected static final boolean ENABLED_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isEnabled() <em>Enabled</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isEnabled()
	 * @generated
	 * @ordered
	 */
	protected boolean enabled = ENABLED_EDEFAULT;

	/**
	 * The cached value of the '{@link #getIncoming() <em>Incoming</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getIncoming()
	 * @generated
	 * @ordered
	 */
	protected EList<ArcToEvent> incoming;

	/**
	 * The cached value of the '{@link #getOutgoing() <em>Outgoing</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getOutgoing()
	 * @generated
	 * @ordered
	 */
	protected EList<ArcToCondition> outgoing;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected EventImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return AdaptiveSystemPackage.Literals.EVENT;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<Condition> getPreConditions() {
		if (preConditions == null) {
			preConditions = new EObjectWithInverseResolvingEList.ManyInverse<Condition>(Condition.class, this, AdaptiveSystemPackage.EVENT__PRE_CONDITIONS, AdaptiveSystemPackage.CONDITION__POST_EVENTS);
		}
		return preConditions;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<Condition> getPostConditions() {
		if (postConditions == null) {
			postConditions = new EObjectWithInverseResolvingEList.ManyInverse<Condition>(Condition.class, this, AdaptiveSystemPackage.EVENT__POST_CONDITIONS, AdaptiveSystemPackage.CONDITION__PRE_EVENTS);
		}
		return postConditions;
	}

	/**
	 * <!-- begin-user-doc -->
	 * An Event is saturated if all PreConditions (isMarked() == true) are marked
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public boolean isSaturated() {
		if(!getPreConditions().iterator().hasNext()) {
			return true;
		}
		Condition condition = null;
		for(Iterator<Condition> preConditions = getPreConditions().iterator(); preConditions.hasNext();  ) {
			condition = preConditions.next();
			if(!condition.isMarked()) {
				return false;
			}
		}
		return true;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean isEnabled() {
		return enabled;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setEnabled(boolean newEnabled) {
		boolean oldEnabled = enabled;
		enabled = newEnabled;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.EVENT__ENABLED, oldEnabled, enabled));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<ArcToEvent> getIncoming() {
		if (incoming == null) {
			incoming = new EObjectWithInverseResolvingEList<ArcToEvent>(ArcToEvent.class, this, AdaptiveSystemPackage.EVENT__INCOMING, AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION);
		}
		return incoming;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<ArcToCondition> getOutgoing() {
		if (outgoing == null) {
			outgoing = new EObjectWithInverseResolvingEList<ArcToCondition>(ArcToCondition.class, this, AdaptiveSystemPackage.EVENT__OUTGOING, AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE);
		}
		return outgoing;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@SuppressWarnings("unchecked")
	@Override
	public NotificationChain eInverseAdd(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case AdaptiveSystemPackage.EVENT__PRE_CONDITIONS:
				return ((InternalEList<InternalEObject>)(InternalEList<?>)getPreConditions()).basicAdd(otherEnd, msgs);
			case AdaptiveSystemPackage.EVENT__POST_CONDITIONS:
				return ((InternalEList<InternalEObject>)(InternalEList<?>)getPostConditions()).basicAdd(otherEnd, msgs);
			case AdaptiveSystemPackage.EVENT__INCOMING:
				return ((InternalEList<InternalEObject>)(InternalEList<?>)getIncoming()).basicAdd(otherEnd, msgs);
			case AdaptiveSystemPackage.EVENT__OUTGOING:
				return ((InternalEList<InternalEObject>)(InternalEList<?>)getOutgoing()).basicAdd(otherEnd, msgs);
		}
		return super.eInverseAdd(otherEnd, featureID, msgs);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case AdaptiveSystemPackage.EVENT__PRE_CONDITIONS:
				return ((InternalEList<?>)getPreConditions()).basicRemove(otherEnd, msgs);
			case AdaptiveSystemPackage.EVENT__POST_CONDITIONS:
				return ((InternalEList<?>)getPostConditions()).basicRemove(otherEnd, msgs);
			case AdaptiveSystemPackage.EVENT__INCOMING:
				return ((InternalEList<?>)getIncoming()).basicRemove(otherEnd, msgs);
			case AdaptiveSystemPackage.EVENT__OUTGOING:
				return ((InternalEList<?>)getOutgoing()).basicRemove(otherEnd, msgs);
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
			case AdaptiveSystemPackage.EVENT__PRE_CONDITIONS:
				return getPreConditions();
			case AdaptiveSystemPackage.EVENT__POST_CONDITIONS:
				return getPostConditions();
			case AdaptiveSystemPackage.EVENT__SATURATED:
				return isSaturated() ? Boolean.TRUE : Boolean.FALSE;
			case AdaptiveSystemPackage.EVENT__ENABLED:
				return isEnabled() ? Boolean.TRUE : Boolean.FALSE;
			case AdaptiveSystemPackage.EVENT__INCOMING:
				return getIncoming();
			case AdaptiveSystemPackage.EVENT__OUTGOING:
				return getOutgoing();
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
			case AdaptiveSystemPackage.EVENT__PRE_CONDITIONS:
				getPreConditions().clear();
				getPreConditions().addAll((Collection<? extends Condition>)newValue);
				return;
			case AdaptiveSystemPackage.EVENT__POST_CONDITIONS:
				getPostConditions().clear();
				getPostConditions().addAll((Collection<? extends Condition>)newValue);
				return;
			case AdaptiveSystemPackage.EVENT__ENABLED:
				setEnabled(((Boolean)newValue).booleanValue());
				return;
			case AdaptiveSystemPackage.EVENT__INCOMING:
				getIncoming().clear();
				getIncoming().addAll((Collection<? extends ArcToEvent>)newValue);
				return;
			case AdaptiveSystemPackage.EVENT__OUTGOING:
				getOutgoing().clear();
				getOutgoing().addAll((Collection<? extends ArcToCondition>)newValue);
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
			case AdaptiveSystemPackage.EVENT__PRE_CONDITIONS:
				getPreConditions().clear();
				return;
			case AdaptiveSystemPackage.EVENT__POST_CONDITIONS:
				getPostConditions().clear();
				return;
			case AdaptiveSystemPackage.EVENT__ENABLED:
				setEnabled(ENABLED_EDEFAULT);
				return;
			case AdaptiveSystemPackage.EVENT__INCOMING:
				getIncoming().clear();
				return;
			case AdaptiveSystemPackage.EVENT__OUTGOING:
				getOutgoing().clear();
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
			case AdaptiveSystemPackage.EVENT__PRE_CONDITIONS:
				return preConditions != null && !preConditions.isEmpty();
			case AdaptiveSystemPackage.EVENT__POST_CONDITIONS:
				return postConditions != null && !postConditions.isEmpty();
			case AdaptiveSystemPackage.EVENT__SATURATED:
				return isSaturated() != SATURATED_EDEFAULT;
			case AdaptiveSystemPackage.EVENT__ENABLED:
				return enabled != ENABLED_EDEFAULT;
			case AdaptiveSystemPackage.EVENT__INCOMING:
				return incoming != null && !incoming.isEmpty();
			case AdaptiveSystemPackage.EVENT__OUTGOING:
				return outgoing != null && !outgoing.isEmpty();
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
		result.append(" (enabled: ");
		result.append(enabled);
		result.append(')');
		return result.toString();
	}

} //EventImpl

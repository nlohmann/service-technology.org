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
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.PreNet;

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
 * An implementation of the model object '<em><b>Condition</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.impl.ConditionImpl#getPreEvents <em>Pre Events</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.ConditionImpl#getPostEvents <em>Post Events</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.ConditionImpl#getToken <em>Token</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.ConditionImpl#isMarked <em>Marked</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.ConditionImpl#isMaximal <em>Maximal</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.ConditionImpl#isMinimal <em>Minimal</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.ConditionImpl#getIncoming <em>Incoming</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.ConditionImpl#getOutgoing <em>Outgoing</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ConditionImpl extends NodeImpl implements Condition {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * The cached value of the '{@link #getPreEvents() <em>Pre Events</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getPreEvents()
	 * @generated
	 * @ordered
	 */
	protected EList<Event> preEvents;

	/**
	 * The cached value of the '{@link #getPostEvents() <em>Post Events</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getPostEvents()
	 * @generated
	 * @ordered
	 */
	protected EList<Event> postEvents;

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
	 * The default value of the '{@link #isMarked() <em>Marked</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isMarked()
	 * @generated
	 * @ordered
	 */
	protected static final boolean MARKED_EDEFAULT = true;

	/**
	 * The default value of the '{@link #isMaximal() <em>Maximal</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isMaximal()
	 * @generated
	 * @ordered
	 */
	protected static final boolean MAXIMAL_EDEFAULT = false;

	/**
	 * The default value of the '{@link #isMinimal() <em>Minimal</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isMinimal()
	 * @generated
	 * @ordered
	 */
	protected static final boolean MINIMAL_EDEFAULT = true;

	/**
	 * The cached value of the '{@link #getIncoming() <em>Incoming</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getIncoming()
	 * @generated
	 * @ordered
	 */
	protected EList<ArcToCondition> incoming;

	/**
	 * The cached value of the '{@link #getOutgoing() <em>Outgoing</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getOutgoing()
	 * @generated
	 * @ordered
	 */
	protected EList<ArcToEvent> outgoing;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ConditionImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return AdaptiveSystemPackage.Literals.CONDITION;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<Event> getPreEvents() {
		if (preEvents == null) {
			preEvents = new EObjectWithInverseResolvingEList.ManyInverse<Event>(Event.class, this, AdaptiveSystemPackage.CONDITION__PRE_EVENTS, AdaptiveSystemPackage.EVENT__POST_CONDITIONS);
		}
		return preEvents;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<Event> getPostEvents() {
		if (postEvents == null) {
			postEvents = new EObjectWithInverseResolvingEList.ManyInverse<Event>(Event.class, this, AdaptiveSystemPackage.CONDITION__POST_EVENTS, AdaptiveSystemPackage.EVENT__PRE_CONDITIONS);
		}
		return postEvents;
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
	 * Only a maxCondition or a condition in mainProcess is allowed to have token > 0
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public void setToken(int newToken) {
		int oldToken = token;
		//START: Manja Wolf
		if(isSetMaximal() && !isMaximal() || isAbstract()) newToken = 0;
		if(eContainer instanceof DoNet) newToken = 0;
		//END: Manja Wolf
		token = newToken;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.CONDITION__TOKEN, oldToken, token));
	}

	/**
	 * <!-- begin-user-doc -->
	 * if token of the Condition is > 0 it is initial marked
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public boolean isMarked() {
		// Ensure that you remove @generated or mark it @generated NOT
		return (getToken() > 0);
	}

	/**
	 * <!-- begin-user-doc -->
	 * Only maximal Conditions (only in PreNets) are allowed to have token > 0 
	 * definition (maximal Condition): 
	 * All ArcsToEvent of these Condition (in postConditions saved) has as parentNet
	 * the DoNet. If a Condition b has a postEvent with parentNet= preNet -> b.Token = 0.
	 * A Condition without ArcToEvent is maximal.
	 * Only a maximal Condition b of a PreNet is allowed to have token > 0.
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public boolean isMaximal() {
		// Ensure that you remove @generated or mark it @generated NOT
		if(!(this.eContainer instanceof PreNet)) {
			if (eNotificationRequired())
				eNotify(new ENotificationImpl(this, Notification.UNSET, AdaptiveSystemPackage.CONDITION__MAXIMAL, MAXIMAL_EDEFAULT, MAXIMAL_EDEFAULT, true));
		}
		if(!getPostEvents().iterator().hasNext()) {
			return true;
		}
		Event event = null;
		for(Iterator<Event> postEvents = getPostEvents().iterator(); postEvents.hasNext();  ) {
			event = postEvents.next();
			if(event.eContainer() instanceof PreNet) {
				//System.out.println("if parentnet Event = preNet");
				return false;
			}
		}
		return true;
	}


	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public boolean isSetMaximal() {
		// Ensure that you remove @generated or mark it @generated NOT
		if(!(this.eContainer instanceof PreNet)) {
			return false;
		}
		return true;
	}

	/**
	 * <!-- begin-user-doc -->
	 * A condition is minimal if it has no event in preSet.
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public boolean isMinimal() {
		if(this.getPreEvents().isEmpty()) {
			return true;
		}
		return false;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<ArcToCondition> getIncoming() {
		if (incoming == null) {
			incoming = new EObjectWithInverseResolvingEList<ArcToCondition>(ArcToCondition.class, this, AdaptiveSystemPackage.CONDITION__INCOMING, AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION);
		}
		return incoming;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<ArcToEvent> getOutgoing() {
		if (outgoing == null) {
			outgoing = new EObjectWithInverseResolvingEList<ArcToEvent>(ArcToEvent.class, this, AdaptiveSystemPackage.CONDITION__OUTGOING, AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE);
		}
		return outgoing;
	}

	/**
	 * <!-- begin-user-doc -->
	 * if condition is in preNet and isMaxCondition() is false then setToken(0)
	 * <!-- end-user-doc -->
	 */
	@Override
	  protected void eBasicSetContainer(InternalEObject newContainer, int newContainerFeatureID)
	  {
	     super.eBasicSetContainer(newContainer, newContainerFeatureID);
	    //
	    if(eOldContainer != null && eOldContainer != newContainer) {
	    	if(	newContainer instanceof DoNet
	    			|| isSetMaximal() && !isMaximal() 
	    			|| isAbstract()
	    			) { 
	    		setToken(0);
	    	} 
	    }
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
			case AdaptiveSystemPackage.CONDITION__PRE_EVENTS:
				return ((InternalEList<InternalEObject>)(InternalEList<?>)getPreEvents()).basicAdd(otherEnd, msgs);
			case AdaptiveSystemPackage.CONDITION__POST_EVENTS:
				return ((InternalEList<InternalEObject>)(InternalEList<?>)getPostEvents()).basicAdd(otherEnd, msgs);
			case AdaptiveSystemPackage.CONDITION__INCOMING:
				return ((InternalEList<InternalEObject>)(InternalEList<?>)getIncoming()).basicAdd(otherEnd, msgs);
			case AdaptiveSystemPackage.CONDITION__OUTGOING:
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
			case AdaptiveSystemPackage.CONDITION__PRE_EVENTS:
				return ((InternalEList<?>)getPreEvents()).basicRemove(otherEnd, msgs);
			case AdaptiveSystemPackage.CONDITION__POST_EVENTS:
				return ((InternalEList<?>)getPostEvents()).basicRemove(otherEnd, msgs);
			case AdaptiveSystemPackage.CONDITION__INCOMING:
				return ((InternalEList<?>)getIncoming()).basicRemove(otherEnd, msgs);
			case AdaptiveSystemPackage.CONDITION__OUTGOING:
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
			case AdaptiveSystemPackage.CONDITION__PRE_EVENTS:
				return getPreEvents();
			case AdaptiveSystemPackage.CONDITION__POST_EVENTS:
				return getPostEvents();
			case AdaptiveSystemPackage.CONDITION__TOKEN:
				return new Integer(getToken());
			case AdaptiveSystemPackage.CONDITION__MARKED:
				return isMarked() ? Boolean.TRUE : Boolean.FALSE;
			case AdaptiveSystemPackage.CONDITION__MAXIMAL:
				return isMaximal() ? Boolean.TRUE : Boolean.FALSE;
			case AdaptiveSystemPackage.CONDITION__MINIMAL:
				return isMinimal() ? Boolean.TRUE : Boolean.FALSE;
			case AdaptiveSystemPackage.CONDITION__INCOMING:
				return getIncoming();
			case AdaptiveSystemPackage.CONDITION__OUTGOING:
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
			case AdaptiveSystemPackage.CONDITION__PRE_EVENTS:
				getPreEvents().clear();
				getPreEvents().addAll((Collection<? extends Event>)newValue);
				return;
			case AdaptiveSystemPackage.CONDITION__POST_EVENTS:
				getPostEvents().clear();
				getPostEvents().addAll((Collection<? extends Event>)newValue);
				return;
			case AdaptiveSystemPackage.CONDITION__TOKEN:
				setToken(((Integer)newValue).intValue());
				return;
			case AdaptiveSystemPackage.CONDITION__INCOMING:
				getIncoming().clear();
				getIncoming().addAll((Collection<? extends ArcToCondition>)newValue);
				return;
			case AdaptiveSystemPackage.CONDITION__OUTGOING:
				getOutgoing().clear();
				getOutgoing().addAll((Collection<? extends ArcToEvent>)newValue);
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
			case AdaptiveSystemPackage.CONDITION__PRE_EVENTS:
				getPreEvents().clear();
				return;
			case AdaptiveSystemPackage.CONDITION__POST_EVENTS:
				getPostEvents().clear();
				return;
			case AdaptiveSystemPackage.CONDITION__TOKEN:
				setToken(TOKEN_EDEFAULT);
				return;
			case AdaptiveSystemPackage.CONDITION__INCOMING:
				getIncoming().clear();
				return;
			case AdaptiveSystemPackage.CONDITION__OUTGOING:
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
			case AdaptiveSystemPackage.CONDITION__PRE_EVENTS:
				return preEvents != null && !preEvents.isEmpty();
			case AdaptiveSystemPackage.CONDITION__POST_EVENTS:
				return postEvents != null && !postEvents.isEmpty();
			case AdaptiveSystemPackage.CONDITION__TOKEN:
				return token != TOKEN_EDEFAULT;
			case AdaptiveSystemPackage.CONDITION__MARKED:
				return isMarked() != MARKED_EDEFAULT;
			case AdaptiveSystemPackage.CONDITION__MAXIMAL:
				return isSetMaximal();
			case AdaptiveSystemPackage.CONDITION__MINIMAL:
				return isMinimal() != MINIMAL_EDEFAULT;
			case AdaptiveSystemPackage.CONDITION__INCOMING:
				return incoming != null && !incoming.isEmpty();
			case AdaptiveSystemPackage.CONDITION__OUTGOING:
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
		result.append(" (token: ");
		result.append(token);
		result.append(')');
		return result.toString();
	}

} //ConditionImpl

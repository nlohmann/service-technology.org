/**
 * Humboldt Universität zu Berlin, Thoery of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.impl;

import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToEvent;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Arc To Event</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.impl.ArcToEventImpl#getSource <em>Source</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.ArcToEventImpl#getDestination <em>Destination</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ArcToEventImpl extends ArcImpl implements ArcToEvent {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Thoery of Programming";

	/**
	 * The cached value of the '{@link #getSource() <em>Source</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSource()
	 * @generated
	 * @ordered
	 */
	protected Condition source;

	/**
	 * The cached value of the '{@link #getDestination() <em>Destination</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getDestination()
	 * @generated
	 * @ordered
	 */
	protected Event destination;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ArcToEventImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return AdaptiveSystemPackage.Literals.ARC_TO_EVENT;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Condition getSource() {
		if (source != null && source.eIsProxy()) {
			InternalEObject oldSource = (InternalEObject)source;
			source = (Condition)eResolveProxy(oldSource);
			if (source != oldSource) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE, oldSource, source));
			}
		}
		return source;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Condition basicGetSource() {
		return source;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetSource(Condition newSource, NotificationChain msgs) {
		Condition oldSource = source;
		source = newSource;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE, oldSource, newSource);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setSource(Condition newSource) {
		//Manja Wolf: START
		Condition oldSource = source;
		//if arc deleted then delete destination and remove postSet and preSet of oldSource an oldDestination
		if(newSource == null) {
			if(getDestination() != null) {
				//delete PostEvent on oldSource and automatically delete corresponding preCondition on destination.PreCondition
				if(oldSource != null && oldSource.getPostEvents() != null && oldSource.getPostEvents().contains(getDestination())) {
					oldSource.getPostEvents().remove(getDestination());
				}
			}		
		} else
		//set postEvent of source(condition) to destination (event)
		//if source != null
		//not used during delete - only for reorientation
		if(getDestination() != null) {
			if(oldSource != null && oldSource.getPostEvents() != null && oldSource.getPostEvents().contains(getDestination())) {
				oldSource.getPostEvents().remove(getDestination());
			}
			newSource.getPostEvents().add(getDestination());
		}
		//Manja Wolf: END
		if (newSource != source) {
			NotificationChain msgs = null;
			if (source != null)
				msgs = ((InternalEObject)source).eInverseRemove(this, AdaptiveSystemPackage.CONDITION__OUTGOING, Condition.class, msgs);
			if (newSource != null)
				msgs = ((InternalEObject)newSource).eInverseAdd(this, AdaptiveSystemPackage.CONDITION__OUTGOING, Condition.class, msgs);
			msgs = basicSetSource(newSource, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE, newSource, newSource));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Event getDestination() {
		if (destination != null && destination.eIsProxy()) {
			InternalEObject oldDestination = (InternalEObject)destination;
			destination = (Event)eResolveProxy(oldDestination);
			if (destination != oldDestination) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION, oldDestination, destination));
			}
		}
		return destination;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Event basicGetDestination() {
		return destination;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetDestination(Event newDestination, NotificationChain msgs) {
		Event oldDestination = destination;
		destination = newDestination;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION, oldDestination, newDestination);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated 
	 */
	public void setDestination(Event newDestination) {
		//Manja Wolf: START
		Event oldDestination = destination;
		//set preCondition of destination (event) to source(condition) of the arc
		if(getSource() != null) {
			if(oldDestination != null && oldDestination.getPreConditions() != null && oldDestination.getPreConditions().contains(getSource())) {
				oldDestination.getPreConditions().remove(getSource());
			}
			if(newDestination != null) {
				newDestination.getPreConditions().add(getSource());
			}
			//if source is not maxCondition it must have token = 0
			if(source.isSetMaximal() && !source.isMaximal()) source.setToken(0);
		}
		//Manja Wolf: END
		if (newDestination != destination) {
			NotificationChain msgs = null;
			if (destination != null)
				msgs = ((InternalEObject)destination).eInverseRemove(this, AdaptiveSystemPackage.EVENT__INCOMING, Event.class, msgs);
			if (newDestination != null)
				msgs = ((InternalEObject)newDestination).eInverseAdd(this, AdaptiveSystemPackage.EVENT__INCOMING, Event.class, msgs);
			msgs = basicSetDestination(newDestination, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION, newDestination, newDestination));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseAdd(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE:
				if (source != null)
					msgs = ((InternalEObject)source).eInverseRemove(this, AdaptiveSystemPackage.CONDITION__OUTGOING, Condition.class, msgs);
				return basicSetSource((Condition)otherEnd, msgs);
			case AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION:
				if (destination != null)
					msgs = ((InternalEObject)destination).eInverseRemove(this, AdaptiveSystemPackage.EVENT__INCOMING, Event.class, msgs);
				return basicSetDestination((Event)otherEnd, msgs);
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
			case AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE:
				return basicSetSource(null, msgs);
			case AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION:
				return basicSetDestination(null, msgs);
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
			case AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE:
				if (resolve) return getSource();
				return basicGetSource();
			case AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION:
				if (resolve) return getDestination();
				return basicGetDestination();
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
			case AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE:
				setSource((Condition)newValue);
				return;
			case AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION:
				setDestination((Event)newValue);
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
			case AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE:
				setSource((Condition)null);
				return;
			case AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION:
				setDestination((Event)null);
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
			case AdaptiveSystemPackage.ARC_TO_EVENT__SOURCE:
				return source != null;
			case AdaptiveSystemPackage.ARC_TO_EVENT__DESTINATION:
				return destination != null;
		}
		return super.eIsSet(featureID);
	}

} //ArcToEventImpl

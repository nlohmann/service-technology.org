/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.impl;

import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Arc To Condition</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.impl.ArcToConditionImpl#getSource <em>Source</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.ArcToConditionImpl#getDestination <em>Destination</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class ArcToConditionImpl extends ArcImpl implements ArcToCondition {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * The cached value of the '{@link #getSource() <em>Source</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSource()
	 * @generated
	 * @ordered
	 */
	protected Event source;

	/**
	 * The cached value of the '{@link #getDestination() <em>Destination</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getDestination()
	 * @generated
	 * @ordered
	 */
	protected Condition destination;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ArcToConditionImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return AdaptiveSystemPackage.Literals.ARC_TO_CONDITION;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Event getSource() {
		if (source != null && source.eIsProxy()) {
			InternalEObject oldSource = (InternalEObject)source;
			source = (Event)eResolveProxy(oldSource);
			if (source != oldSource) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE, oldSource, source));
			}
		}
		return source;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Event basicGetSource() {
		return source;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetSource(Event newSource, NotificationChain msgs) {
		Event oldSource = source;
		source = newSource;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE, oldSource, newSource);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public void setSource(Event newSource) {
		//Manja Wolf: START
		//if arc deleted then delete destination and remove postSet and preSet of oldSource an oldDestination
		Event oldSource = source;
		if(newSource == null) {
			if(getDestination() != null) {
				//delete PostCondition on oldSource and automatic delete corresponding preEvent on destination.PreEvents 
				if(oldSource != null && oldSource.getPostConditions() != null && oldSource.getPostConditions().contains(getDestination())) {
					oldSource.getPostConditions().remove(getDestination());
				}
			}		
		} else
		//Set PostCondition-Attribute to destination of the Arc
		//if source != null
		//not used during delete - only for reorientation  
		if(getDestination() != null) {
			if(oldSource != null && oldSource.getPostConditions() != null && oldSource.getPostConditions().contains(getDestination())) {
				oldSource.getPostConditions().remove(getDestination());
			}
			newSource.getPostConditions().add(getDestination());
		}
		//Manja Wolf: END
		if (newSource != source) {
			NotificationChain msgs = null;
			if (source != null)
				msgs = ((InternalEObject)source).eInverseRemove(this, AdaptiveSystemPackage.EVENT__OUTGOING, Event.class, msgs);
			if (newSource != null)
				msgs = ((InternalEObject)newSource).eInverseAdd(this, AdaptiveSystemPackage.EVENT__OUTGOING, Event.class, msgs);
			msgs = basicSetSource(newSource, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE, newSource, newSource));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Condition getDestination() {
		if (destination != null && destination.eIsProxy()) {
			InternalEObject oldDestination = (InternalEObject)destination;
			destination = (Condition)eResolveProxy(oldDestination);
			if (destination != oldDestination) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION, oldDestination, destination));
			}
		}
		return destination;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Condition basicGetDestination() {
		return destination;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetDestination(Condition newDestination, NotificationChain msgs) {
		Condition oldDestination = destination;
		destination = newDestination;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION, oldDestination, newDestination);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public void setDestination(Condition newDestination) {
		//Manja Wolf: START
		//Set preEvent to destination (condition) of the arc
		Condition oldDestination = destination;
		if(getSource() != null) {
			if(oldDestination != null && oldDestination.getPreEvents() != null && oldDestination.getPreEvents().contains(getSource())) {
				oldDestination.getPreEvents().remove(getSource());
			}
			//Set preEvent to destination (condition) of the arc and (automatically) postCondition to source (event) of the arc 
			if(newDestination != null) {
				newDestination.getPreEvents().add(getSource());
			}
		}
		//Manja Wolf: END
		if (newDestination != destination) {
			NotificationChain msgs = null;
			if (destination != null)
				msgs = ((InternalEObject)destination).eInverseRemove(this, AdaptiveSystemPackage.CONDITION__INCOMING, Condition.class, msgs);
			if (newDestination != null)
				msgs = ((InternalEObject)newDestination).eInverseAdd(this, AdaptiveSystemPackage.CONDITION__INCOMING, Condition.class, msgs);
			msgs = basicSetDestination(newDestination, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION, newDestination, newDestination));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseAdd(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE:
				if (source != null)
					msgs = ((InternalEObject)source).eInverseRemove(this, AdaptiveSystemPackage.EVENT__OUTGOING, Event.class, msgs);
				return basicSetSource((Event)otherEnd, msgs);
			case AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION:
				if (destination != null)
					msgs = ((InternalEObject)destination).eInverseRemove(this, AdaptiveSystemPackage.CONDITION__INCOMING, Condition.class, msgs);
				return basicSetDestination((Condition)otherEnd, msgs);
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
			case AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE:
				return basicSetSource(null, msgs);
			case AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION:
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
			case AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE:
				if (resolve) return getSource();
				return basicGetSource();
			case AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION:
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
			case AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE:
				setSource((Event)newValue);
				return;
			case AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION:
				setDestination((Condition)newValue);
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
			case AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE:
				setSource((Event)null);
				return;
			case AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION:
				setDestination((Condition)null);
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
			case AdaptiveSystemPackage.ARC_TO_CONDITION__SOURCE:
				return source != null;
			case AdaptiveSystemPackage.ARC_TO_CONDITION__DESTINATION:
				return destination != null;
		}
		return super.eIsSet(featureID);
	}

} //ArcToConditionImpl

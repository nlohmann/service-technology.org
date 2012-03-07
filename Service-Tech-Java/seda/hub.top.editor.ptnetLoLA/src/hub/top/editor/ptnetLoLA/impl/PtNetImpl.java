/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA.impl;

import hub.top.editor.ptnetLoLA.Annotation;
import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Marking;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;


/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Pt Net</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.editor.ptnetLoLA.impl.PtNetImpl#getPlaces <em>Places</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.impl.PtNetImpl#getTransitions <em>Transitions</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.impl.PtNetImpl#getInitialMarking <em>Initial Marking</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.impl.PtNetImpl#getAnnotation <em>Annotation</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.impl.PtNetImpl#getArcs <em>Arcs</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.impl.PtNetImpl#getFinalMarking <em>Final Marking</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class PtNetImpl extends EObjectImpl implements PtNet {
	/**
   * The cached value of the '{@link #getPlaces() <em>Places</em>}' containment reference list.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see #getPlaces()
   * @generated
   * @ordered
   */
	protected EList<Place> places;

	/**
   * The cached value of the '{@link #getTransitions() <em>Transitions</em>}' containment reference list.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see #getTransitions()
   * @generated
   * @ordered
   */
	protected EList<Transition> transitions;

	/**
   * The cached value of the '{@link #getInitialMarking() <em>Initial Marking</em>}' containment reference.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see #getInitialMarking()
   * @generated
   * @ordered
   */
	protected Marking initialMarking;

	/**
   * The cached value of the '{@link #getAnnotation() <em>Annotation</em>}' containment reference.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see #getAnnotation()
   * @generated
   * @ordered
   */
	protected Annotation annotation;

	/**
   * The cached value of the '{@link #getArcs() <em>Arcs</em>}' containment reference list.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see #getArcs()
   * @generated
   * @ordered
   */
	protected EList<Arc> arcs;

	/**
   * The cached value of the '{@link #getFinalMarking() <em>Final Marking</em>}' containment reference.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @see #getFinalMarking()
   * @generated
   * @ordered
   */
	protected Marking finalMarking;

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	protected PtNetImpl() {
    super();
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	@Override
	protected EClass eStaticClass() {
    return PtnetLoLAPackage.Literals.PT_NET;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EList<Place> getPlaces() {
    if (places == null) {
      places = new EObjectContainmentEList<Place>(Place.class, this, PtnetLoLAPackage.PT_NET__PLACES);
    }
    return places;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EList<Transition> getTransitions() {
    if (transitions == null) {
      transitions = new EObjectContainmentEList<Transition>(Transition.class, this, PtnetLoLAPackage.PT_NET__TRANSITIONS);
    }
    return transitions;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public Marking getInitialMarking() {
    return initialMarking;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public NotificationChain basicSetInitialMarking(Marking newInitialMarking, NotificationChain msgs) {
    Marking oldInitialMarking = initialMarking;
    initialMarking = newInitialMarking;
    if (eNotificationRequired()) {
      ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, PtnetLoLAPackage.PT_NET__INITIAL_MARKING, oldInitialMarking, newInitialMarking);
      if (msgs == null) msgs = notification; else msgs.add(notification);
    }
    return msgs;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public void setInitialMarking(Marking newInitialMarking) {
    if (newInitialMarking != initialMarking) {
      NotificationChain msgs = null;
      if (initialMarking != null)
        msgs = ((InternalEObject)initialMarking).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - PtnetLoLAPackage.PT_NET__INITIAL_MARKING, null, msgs);
      if (newInitialMarking != null)
        msgs = ((InternalEObject)newInitialMarking).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - PtnetLoLAPackage.PT_NET__INITIAL_MARKING, null, msgs);
      msgs = basicSetInitialMarking(newInitialMarking, msgs);
      if (msgs != null) msgs.dispatch();
    }
    else if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, PtnetLoLAPackage.PT_NET__INITIAL_MARKING, newInitialMarking, newInitialMarking));
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public Annotation getAnnotation() {
    return annotation;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public NotificationChain basicSetAnnotation(Annotation newAnnotation, NotificationChain msgs) {
    Annotation oldAnnotation = annotation;
    annotation = newAnnotation;
    if (eNotificationRequired()) {
      ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, PtnetLoLAPackage.PT_NET__ANNOTATION, oldAnnotation, newAnnotation);
      if (msgs == null) msgs = notification; else msgs.add(notification);
    }
    return msgs;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public void setAnnotation(Annotation newAnnotation) {
    if (newAnnotation != annotation) {
      NotificationChain msgs = null;
      if (annotation != null)
        msgs = ((InternalEObject)annotation).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - PtnetLoLAPackage.PT_NET__ANNOTATION, null, msgs);
      if (newAnnotation != null)
        msgs = ((InternalEObject)newAnnotation).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - PtnetLoLAPackage.PT_NET__ANNOTATION, null, msgs);
      msgs = basicSetAnnotation(newAnnotation, msgs);
      if (msgs != null) msgs.dispatch();
    }
    else if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, PtnetLoLAPackage.PT_NET__ANNOTATION, newAnnotation, newAnnotation));
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public EList<Arc> getArcs() {
    if (arcs == null) {
      arcs = new EObjectContainmentEList<Arc>(Arc.class, this, PtnetLoLAPackage.PT_NET__ARCS);
    }
    return arcs;
  }

	/**
	 * 
	 * @param newValue
	 * 
	 * @generated NOT
	 */
	private void setArcs(Collection<? extends Arc>newValue) {
		EList<Arc> deleteArcs = new BasicEList<Arc>(getArcs());	// copy list of arcs
		deleteArcs.removeAll(newValue);							// remove all values to be added
		
		getArcs().clear();
		getArcs().addAll((Collection<? extends Arc>)newValue);
		System.err.println("set arcs");
		for (Arc arc : deleteArcs) {
			System.err.println("removing arc "+arc);
			arc.setSource(null);
			arc.setTarget(null);
		}
	}
	
	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public Marking getFinalMarking() {
    return finalMarking;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public NotificationChain basicSetFinalMarking(Marking newFinalMarking, NotificationChain msgs) {
    Marking oldFinalMarking = finalMarking;
    finalMarking = newFinalMarking;
    if (eNotificationRequired()) {
      ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, PtnetLoLAPackage.PT_NET__FINAL_MARKING, oldFinalMarking, newFinalMarking);
      if (msgs == null) msgs = notification; else msgs.add(notification);
    }
    return msgs;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	public void setFinalMarking(Marking newFinalMarking) {
    if (newFinalMarking != finalMarking) {
      NotificationChain msgs = null;
      if (finalMarking != null)
        msgs = ((InternalEObject)finalMarking).eInverseRemove(this, EOPPOSITE_FEATURE_BASE - PtnetLoLAPackage.PT_NET__FINAL_MARKING, null, msgs);
      if (newFinalMarking != null)
        msgs = ((InternalEObject)newFinalMarking).eInverseAdd(this, EOPPOSITE_FEATURE_BASE - PtnetLoLAPackage.PT_NET__FINAL_MARKING, null, msgs);
      msgs = basicSetFinalMarking(newFinalMarking, msgs);
      if (msgs != null) msgs.dispatch();
    }
    else if (eNotificationRequired())
      eNotify(new ENotificationImpl(this, Notification.SET, PtnetLoLAPackage.PT_NET__FINAL_MARKING, newFinalMarking, newFinalMarking));
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	@Override
	public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case PtnetLoLAPackage.PT_NET__PLACES:
        return ((InternalEList<?>)getPlaces()).basicRemove(otherEnd, msgs);
      case PtnetLoLAPackage.PT_NET__TRANSITIONS:
        return ((InternalEList<?>)getTransitions()).basicRemove(otherEnd, msgs);
      case PtnetLoLAPackage.PT_NET__INITIAL_MARKING:
        return basicSetInitialMarking(null, msgs);
      case PtnetLoLAPackage.PT_NET__ANNOTATION:
        return basicSetAnnotation(null, msgs);
      case PtnetLoLAPackage.PT_NET__ARCS:
        return ((InternalEList<?>)getArcs()).basicRemove(otherEnd, msgs);
      case PtnetLoLAPackage.PT_NET__FINAL_MARKING:
        return basicSetFinalMarking(null, msgs);
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
      case PtnetLoLAPackage.PT_NET__PLACES:
        return getPlaces();
      case PtnetLoLAPackage.PT_NET__TRANSITIONS:
        return getTransitions();
      case PtnetLoLAPackage.PT_NET__INITIAL_MARKING:
        return getInitialMarking();
      case PtnetLoLAPackage.PT_NET__ANNOTATION:
        return getAnnotation();
      case PtnetLoLAPackage.PT_NET__ARCS:
        return getArcs();
      case PtnetLoLAPackage.PT_NET__FINAL_MARKING:
        return getFinalMarking();
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
      case PtnetLoLAPackage.PT_NET__PLACES:
        getPlaces().clear();
        getPlaces().addAll((Collection<? extends Place>)newValue);
        return;
      case PtnetLoLAPackage.PT_NET__TRANSITIONS:
        getTransitions().clear();
        getTransitions().addAll((Collection<? extends Transition>)newValue);
        return;
      case PtnetLoLAPackage.PT_NET__INITIAL_MARKING:
        setInitialMarking((Marking)newValue);
        return;
      case PtnetLoLAPackage.PT_NET__ANNOTATION:
        setAnnotation((Annotation)newValue);
        return;
      case PtnetLoLAPackage.PT_NET__ARCS:
        getArcs().clear();
        getArcs().addAll((Collection<? extends Arc>)newValue);
        return;
      case PtnetLoLAPackage.PT_NET__FINAL_MARKING:
        setFinalMarking((Marking)newValue);
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
      case PtnetLoLAPackage.PT_NET__PLACES:
        getPlaces().clear();
        return;
      case PtnetLoLAPackage.PT_NET__TRANSITIONS:
        getTransitions().clear();
        return;
      case PtnetLoLAPackage.PT_NET__INITIAL_MARKING:
        setInitialMarking((Marking)null);
        return;
      case PtnetLoLAPackage.PT_NET__ANNOTATION:
        setAnnotation((Annotation)null);
        return;
      case PtnetLoLAPackage.PT_NET__ARCS:
        getArcs().clear();
        return;
      case PtnetLoLAPackage.PT_NET__FINAL_MARKING:
        setFinalMarking((Marking)null);
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
      case PtnetLoLAPackage.PT_NET__PLACES:
        return places != null && !places.isEmpty();
      case PtnetLoLAPackage.PT_NET__TRANSITIONS:
        return transitions != null && !transitions.isEmpty();
      case PtnetLoLAPackage.PT_NET__INITIAL_MARKING:
        return initialMarking != null;
      case PtnetLoLAPackage.PT_NET__ANNOTATION:
        return annotation != null;
      case PtnetLoLAPackage.PT_NET__ARCS:
        return arcs != null && !arcs.isEmpty();
      case PtnetLoLAPackage.PT_NET__FINAL_MARKING:
        return finalMarking != null;
    }
    return super.eIsSet(featureID);
  }

} //PtNetImpl

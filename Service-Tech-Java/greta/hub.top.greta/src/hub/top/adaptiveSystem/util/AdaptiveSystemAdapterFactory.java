/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.util;

import hub.top.adaptiveSystem.*;

import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;

import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model.
 * It provides an adapter <code>createXXX</code> method for each class of the model.
 * <!-- end-user-doc -->
 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage
 * @generated
 */
public class AdaptiveSystemAdapterFactory extends AdapterFactoryImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * The cached model package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected static AdaptiveSystemPackage modelPackage;

	/**
	 * Creates an instance of the adapter factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public AdaptiveSystemAdapterFactory() {
		if (modelPackage == null) {
			modelPackage = AdaptiveSystemPackage.eINSTANCE;
		}
	}

	/**
	 * Returns whether this factory is applicable for the type of the object.
	 * <!-- begin-user-doc -->
	 * This implementation returns <code>true</code> if the object is either the model's package or is an instance object of the model.
	 * <!-- end-user-doc -->
	 * @return whether this factory is applicable for the type of the object.
	 * @generated
	 */
	@Override
	public boolean isFactoryForType(Object object) {
		if (object == modelPackage) {
			return true;
		}
		if (object instanceof EObject) {
			return ((EObject)object).eClass().getEPackage() == modelPackage;
		}
		return false;
	}

	/**
	 * The switch that delegates to the <code>createXXX</code> methods.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected AdaptiveSystemSwitch<Adapter> modelSwitch =
		new AdaptiveSystemSwitch<Adapter>() {
			@Override
			public Adapter caseAdaptiveSystem(AdaptiveSystem object) {
				return createAdaptiveSystemAdapter();
			}
			@Override
			public Adapter caseOclet(Oclet object) {
				return createOcletAdapter();
			}
			@Override
			public Adapter caseOccurrenceNet(OccurrenceNet object) {
				return createOccurrenceNetAdapter();
			}
			@Override
			public Adapter caseDoNet(DoNet object) {
				return createDoNetAdapter();
			}
			@Override
			public Adapter casePreNet(PreNet object) {
				return createPreNetAdapter();
			}
			@Override
			public Adapter caseAdaptiveProcess(AdaptiveProcess object) {
				return createAdaptiveProcessAdapter();
			}
			@Override
			public Adapter caseNode(Node object) {
				return createNodeAdapter();
			}
			@Override
			public Adapter caseCondition(Condition object) {
				return createConditionAdapter();
			}
			@Override
			public Adapter caseEvent(Event object) {
				return createEventAdapter();
			}
			@Override
			public Adapter caseArc(Arc object) {
				return createArcAdapter();
			}
			@Override
			public Adapter caseArcToEvent(ArcToEvent object) {
				return createArcToEventAdapter();
			}
			@Override
			public Adapter caseArcToCondition(ArcToCondition object) {
				return createArcToConditionAdapter();
			}
			@Override
			public Adapter defaultCase(EObject object) {
				return createEObjectAdapter();
			}
		};

	/**
	 * Creates an adapter for the <code>target</code>.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param target the object to adapt.
	 * @return the adapter for the <code>target</code>.
	 * @generated
	 */
	@Override
	public Adapter createAdapter(Notifier target) {
		return modelSwitch.doSwitch((EObject)target);
	}


	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.AdaptiveSystem <em>Adaptive System</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.AdaptiveSystem
	 * @generated
	 */
	public Adapter createAdaptiveSystemAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.Oclet <em>Oclet</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.Oclet
	 * @generated
	 */
	public Adapter createOcletAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.OccurrenceNet <em>Occurrence Net</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.OccurrenceNet
	 * @generated
	 */
	public Adapter createOccurrenceNetAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.DoNet <em>Do Net</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.DoNet
	 * @generated
	 */
	public Adapter createDoNetAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.PreNet <em>Pre Net</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.PreNet
	 * @generated
	 */
	public Adapter createPreNetAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.AdaptiveProcess <em>Adaptive Process</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.AdaptiveProcess
	 * @generated
	 */
	public Adapter createAdaptiveProcessAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.Node <em>Node</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.Node
	 * @generated
	 */
	public Adapter createNodeAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.Condition <em>Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.Condition
	 * @generated
	 */
	public Adapter createConditionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.Event <em>Event</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.Event
	 * @generated
	 */
	public Adapter createEventAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.Arc <em>Arc</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.Arc
	 * @generated
	 */
	public Adapter createArcAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.ArcToEvent <em>Arc To Event</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.ArcToEvent
	 * @generated
	 */
	public Adapter createArcToEventAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.adaptiveSystem.ArcToCondition <em>Arc To Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.adaptiveSystem.ArcToCondition
	 * @generated
	 */
	public Adapter createArcToConditionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for the default case.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @generated
	 */
	public Adapter createEObjectAdapter() {
		return null;
	}

} //AdaptiveSystemAdapterFactory

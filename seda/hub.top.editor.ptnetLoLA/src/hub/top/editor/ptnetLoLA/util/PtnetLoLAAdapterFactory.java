/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA.util;

import hub.top.editor.ptnetLoLA.*;
import hub.top.editor.ptnetLoLA.adapt.NodeAdapter;

import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;

import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;

import org.eclipse.emf.ecore.EObject;


/**
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model.
 * It provides an adapter <code>createXXX</code> method for each class of the model.
 * <!-- end-user-doc -->
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage
 * @generated
 */
public class PtnetLoLAAdapterFactory extends AdapterFactoryImpl {
	/**
	 * The cached model package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected static PtnetLoLAPackage modelPackage;

	/**
	 * Creates an instance of the adapter factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public PtnetLoLAAdapterFactory() {
		if (modelPackage == null) {
			modelPackage = PtnetLoLAPackage.eINSTANCE;
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
	protected PtnetLoLASwitch<Adapter> modelSwitch =
		new PtnetLoLASwitch<Adapter>() {
			@Override
			public Adapter casePlace(Place object) {
				return createPlaceAdapter();
			}
			@Override
			public Adapter casePtNet(PtNet object) {
				return createPtNetAdapter();
			}
			@Override
			public Adapter caseTransition(Transition object) {
				return createTransitionAdapter();
			}
			@Override
			public Adapter caseNode(Node object) {
				return createNodeAdapter();
			}
			@Override
			public Adapter caseMarking(Marking object) {
				return createMarkingAdapter();
			}
			@Override
			public Adapter casePlaceReference(PlaceReference object) {
				return createPlaceReferenceAdapter();
			}
			@Override
			public Adapter caseRefMarkedPlace(RefMarkedPlace object) {
				return createRefMarkedPlaceAdapter();
			}
			@Override
			public Adapter caseAnnotation(Annotation object) {
				return createAnnotationAdapter();
			}
			@Override
			public Adapter caseArc(Arc object) {
				return createArcAdapter();
			}
			@Override
			public Adapter caseArcToPlace(ArcToPlace object) {
				return createArcToPlaceAdapter();
			}
			@Override
			public Adapter caseArcToTransition(ArcToTransition object) {
				return createArcToTransitionAdapter();
			}
			@Override
			public Adapter caseTransitionExt(TransitionExt object) {
				return createTransitionExtAdapter();
			}
			@Override
			public Adapter casePlaceExt(PlaceExt object) {
				return createPlaceExtAdapter();
			}
			@Override
			public Adapter caseArcToPlaceExt(ArcToPlaceExt object) {
				return createArcToPlaceExtAdapter();
			}
			@Override
			public Adapter caseArcToTransitionExt(ArcToTransitionExt object) {
				return createArcToTransitionExtAdapter();
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
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.Place <em>Place</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.Place
	 * @generated
	 */
	public Adapter createPlaceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.PtNet <em>Pt Net</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.PtNet
	 * @generated
	 */
	public Adapter createPtNetAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.Transition <em>Transition</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.Transition
	 * @generated
	 */
	public Adapter createTransitionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.Node <em>Node</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.Node
	 * @generated
	 */
	public Adapter createNodeAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.Marking <em>Marking</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.Marking
	 * @generated
	 */
	public Adapter createMarkingAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.PlaceReference <em>Place Reference</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.PlaceReference
	 * @generated
	 */
	public Adapter createPlaceReferenceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.RefMarkedPlace <em>Ref Marked Place</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.RefMarkedPlace
	 * @generated
	 */
	public Adapter createRefMarkedPlaceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.Annotation <em>Annotation</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.Annotation
	 * @generated
	 */
	public Adapter createAnnotationAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.Arc <em>Arc</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.Arc
	 * @generated not
	 */
	public Adapter createArcAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.ArcToPlace <em>Arc To Place</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.ArcToPlace
	 * @generated
	 */
	public Adapter createArcToPlaceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.ArcToTransition <em>Arc To Transition</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.ArcToTransition
	 * @generated
	 */
	public Adapter createArcToTransitionAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.TransitionExt <em>Transition Ext</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.TransitionExt
	 * @generated
	 */
  public Adapter createTransitionExtAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.PlaceExt <em>Place Ext</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.PlaceExt
	 * @generated
	 */
  public Adapter createPlaceExtAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.ArcToPlaceExt <em>Arc To Place Ext</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.ArcToPlaceExt
	 * @generated
	 */
  public Adapter createArcToPlaceExtAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.editor.ptnetLoLA.ArcToTransitionExt <em>Arc To Transition Ext</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.editor.ptnetLoLA.ArcToTransitionExt
	 * @generated
	 */
  public Adapter createArcToTransitionExtAdapter() {
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

} //PtnetLoLAAdapterFactory

/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA;

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage
 * @generated
 */
public interface PtnetLoLAFactory extends EFactory {
	/**
	 * The singleton instance of the factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	PtnetLoLAFactory eINSTANCE = hub.top.editor.ptnetLoLA.impl.PtnetLoLAFactoryImpl.init();

	/**
	 * Returns a new object of class '<em>Place</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Place</em>'.
	 * @generated
	 */
	Place createPlace();

	/**
	 * Returns a new object of class '<em>Pt Net</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Pt Net</em>'.
	 * @generated
	 */
	PtNet createPtNet();

	/**
	 * Returns a new object of class '<em>Transition</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Transition</em>'.
	 * @generated
	 */
	Transition createTransition();

	/**
	 * Returns a new object of class '<em>Node</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Node</em>'.
	 * @generated
	 */
	Node createNode();

	/**
	 * Returns a new object of class '<em>Marking</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Marking</em>'.
	 * @generated
	 */
	Marking createMarking();

	/**
	 * Returns a new object of class '<em>Place Reference</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Place Reference</em>'.
	 * @generated
	 */
	PlaceReference createPlaceReference();

	/**
	 * Returns a new object of class '<em>Ref Marked Place</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Ref Marked Place</em>'.
	 * @generated
	 */
	RefMarkedPlace createRefMarkedPlace();

	/**
	 * Returns a new object of class '<em>Annotation</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Annotation</em>'.
	 * @generated
	 */
	Annotation createAnnotation();

	/**
	 * Returns a new object of class '<em>Arc</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arc</em>'.
	 * @generated
	 */
	Arc createArc();

	/**
	 * Returns a new object of class '<em>Arc To Place</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arc To Place</em>'.
	 * @generated
	 */
	ArcToPlace createArcToPlace();

	/**
	 * Returns a new object of class '<em>Arc To Transition</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arc To Transition</em>'.
	 * @generated
	 */
	ArcToTransition createArcToTransition();

	/**
	 * Returns a new object of class '<em>Transition Ext</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Transition Ext</em>'.
	 * @generated
	 */
  TransitionExt createTransitionExt();

  /**
	 * Returns a new object of class '<em>Place Ext</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Place Ext</em>'.
	 * @generated
	 */
  PlaceExt createPlaceExt();

  /**
	 * Returns a new object of class '<em>Arc To Place Ext</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arc To Place Ext</em>'.
	 * @generated
	 */
  ArcToPlaceExt createArcToPlaceExt();

  /**
	 * Returns a new object of class '<em>Arc To Transition Ext</em>'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arc To Transition Ext</em>'.
	 * @generated
	 */
  ArcToTransitionExt createArcToTransitionExt();

  /**
	 * Returns the package supported by this factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the package supported by this factory.
	 * @generated
	 */
	PtnetLoLAPackage getPtnetLoLAPackage();

} //PtnetLoLAFactory

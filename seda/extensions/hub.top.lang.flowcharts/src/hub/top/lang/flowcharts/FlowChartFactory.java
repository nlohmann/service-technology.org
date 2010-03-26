/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts;

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see hub.top.lang.flowcharts.FlowChartPackage
 * @generated
 */
public interface FlowChartFactory extends EFactory {
	/**
   * The singleton instance of the factory.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	FlowChartFactory eINSTANCE = hub.top.lang.flowcharts.impl.FlowChartFactoryImpl.init();

	/**
   * Returns a new object of class '<em>Workflow Diagram</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Workflow Diagram</em>'.
   * @generated
   */
	WorkflowDiagram createWorkflowDiagram();

	/**
   * Returns a new object of class '<em>Start Node</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Start Node</em>'.
   * @generated
   */
	StartNode createStartNode();

	/**
   * Returns a new object of class '<em>Endnode</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Endnode</em>'.
   * @generated
   */
	Endnode createEndnode();

	/**
   * Returns a new object of class '<em>Split Node</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Split Node</em>'.
   * @generated
   */
	SplitNode createSplitNode();

	/**
   * Returns a new object of class '<em>Merge Node</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Merge Node</em>'.
   * @generated
   */
	MergeNode createMergeNode();

	/**
   * Returns a new object of class '<em>Flow</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Flow</em>'.
   * @generated
   */
	Flow createFlow();

	/**
   * Returns a new object of class '<em>Simple Activity</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Simple Activity</em>'.
   * @generated
   */
	SimpleActivity createSimpleActivity();

	/**
   * Returns a new object of class '<em>Subprocess</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Subprocess</em>'.
   * @generated
   */
	Subprocess createSubprocess();

	/**
   * Returns a new object of class '<em>Workflow</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Workflow</em>'.
   * @generated
   */
	Workflow createWorkflow();

	/**
   * Returns a new object of class '<em>Persistent Resource</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Persistent Resource</em>'.
   * @generated
   */
	PersistentResource createPersistentResource();

	/**
   * Returns a new object of class '<em>Transient Resource</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Transient Resource</em>'.
   * @generated
   */
	TransientResource createTransientResource();

	/**
   * Returns a new object of class '<em>Document</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Document</em>'.
   * @generated
   */
	Document createDocument();

	/**
   * Returns a new object of class '<em>Event</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Event</em>'.
   * @generated
   */
	Event createEvent();

	/**
   * Returns a new object of class '<em>Diagram Arc</em>'.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return a new object of class '<em>Diagram Arc</em>'.
   * @generated
   */
	DiagramArc createDiagramArc();

	/**
   * Returns the package supported by this factory.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @return the package supported by this factory.
   * @generated
   */
	FlowChartPackage getFCPackage();

} //FlowChartFactory

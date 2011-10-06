/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.tests;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemFactory;

import junit.framework.TestCase;

import junit.textui.TestRunner;

/**
 * <!-- begin-user-doc -->
 * A test case for the model object '<em><b>Adaptive System</b></em>'.
 * <!-- end-user-doc -->
 * @generated
 */
public class AdaptiveSystemTest extends TestCase {

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * The fixture for this Adaptive System test case.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected AdaptiveSystem fixture = null;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static void main(String[] args) {
		TestRunner.run(AdaptiveSystemTest.class);
	}

	/**
	 * Constructs a new Adaptive System test case with the given name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public AdaptiveSystemTest(String name) {
		super(name);
	}

	/**
	 * Sets the fixture for this Adaptive System test case.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected void setFixture(AdaptiveSystem fixture) {
		this.fixture = fixture;
	}

	/**
	 * Returns the fixture for this Adaptive System test case.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected AdaptiveSystem getFixture() {
		return fixture;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see junit.framework.TestCase#setUp()
	 * @generated
	 */
	@Override
	protected void setUp() throws Exception {
		setFixture(AdaptiveSystemFactory.eINSTANCE.createAdaptiveSystem());
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see junit.framework.TestCase#tearDown()
	 * @generated
	 */
	@Override
	protected void tearDown() throws Exception {
		setFixture(null);
	}

} //AdaptiveSystemTest

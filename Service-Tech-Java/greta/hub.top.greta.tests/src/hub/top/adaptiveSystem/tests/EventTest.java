/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.tests;

import hub.top.adaptiveSystem.AdaptiveSystemFactory;
import hub.top.adaptiveSystem.Event;

import junit.textui.TestRunner;

/**
 * <!-- begin-user-doc -->
 * A test case for the model object '<em><b>Event</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are tested:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.Event#isSaturated() <em>Saturated</em>}</li>
 * </ul>
 * </p>
 * @generated
 */
public class EventTest extends NodeTest {

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static void main(String[] args) {
		TestRunner.run(EventTest.class);
	}

	/**
	 * Constructs a new Event test case with the given name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EventTest(String name) {
		super(name);
	}

	/**
	 * Returns the fixture for this Event test case.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected Event getFixture() {
		return (Event)fixture;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see junit.framework.TestCase#setUp()
	 * @generated
	 */
	@Override
	protected void setUp() throws Exception {
		setFixture(AdaptiveSystemFactory.eINSTANCE.createEvent());
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

	/**
	 * Tests the '{@link hub.top.adaptiveSystem.Event#isSaturated() <em>Saturated</em>}' feature getter.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.Event#isSaturated()
	 * @generated
	 */
	public void testIsSaturated() {
		// TODO: implement this feature getter test method
		// Ensure that you remove @generated or mark it @generated NOT
		fail();
	}

} //EventTest

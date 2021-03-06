/**
 * Humboldt Universitšt zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.tests;

import junit.framework.Test;
import junit.framework.TestSuite;

import junit.textui.TestRunner;

/**
 * <!-- begin-user-doc -->
 * A test suite for the '<em><b>adaptiveSystem</b></em>' package.
 * <!-- end-user-doc -->
 * @generated
 */
public class AdaptiveSystemTests extends TestSuite {

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
		TestRunner.run(suite());
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static Test suite() {
		TestSuite suite = new AdaptiveSystemTests("adaptiveSystem Tests");
		suite.addTestSuite(PreNetTest.class);
		suite.addTestSuite(AdaptiveProcessTest.class);
		suite.addTestSuite(ConditionTest.class);
		suite.addTestSuite(EventTest.class);
		return suite;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public AdaptiveSystemTests(String name) {
		super(name);
	}

} //AdaptiveSystemTests

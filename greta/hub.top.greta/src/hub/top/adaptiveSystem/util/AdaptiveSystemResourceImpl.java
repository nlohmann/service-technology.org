/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.util;

import org.eclipse.emf.common.util.URI;

import org.eclipse.emf.ecore.xmi.impl.XMIResourceImpl;

/**
 * <!-- begin-user-doc -->
 * The <b>Resource </b> associated with the package.
 * <!-- end-user-doc -->
 * @see hub.top.adaptiveSystem.util.AdaptiveSystemResourceFactoryImpl
 * @generated
 */
public class AdaptiveSystemResourceImpl extends XMIResourceImpl {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * Creates an instance of the resource.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param uri the URI of the new resource.
	 * @generated
	 */
	public AdaptiveSystemResourceImpl(URI uri) {
		super(uri);
	}
	
	/**
	 * overriden by Manja Wolf to enable the use of UUIDs
	 */
	@Override
	protected boolean useUUIDs() {
		return true;
	} 
	
} //AdaptiveSystemResourceImpl

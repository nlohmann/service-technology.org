package hub.top.editor.ptnetLoLA.transaction;

import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.transaction.TransactionalEditingDomain;

public class PtnetLoLAEditingDomainFactory implements
		TransactionalEditingDomain.Factory {

	public static final String EDITING_DOMAIN_ID = "hub.top.editor.ptnetLoLA.editingDomain";
	
	/**
	 * @see org.eclipse.emf.transaction.TransactionalEditingDomain.Factory#createEditingDomain()
	 */
	public TransactionalEditingDomain createEditingDomain() {
		TransactionalEditingDomain ed =
			TransactionalEditingDomain.Factory.INSTANCE.createEditingDomain();
		ed.addResourceSetListener(new PtnetLoLAResourceListener());
		return ed;
	}

	/**
	 * @see org.eclipse.emf.transaction.TransactionalEditingDomain.Factory#createEditingDomain(org.eclipse.emf.ecore.resource.ResourceSet)
	 */
	public TransactionalEditingDomain createEditingDomain(ResourceSet rset) {
		TransactionalEditingDomain ed =
			TransactionalEditingDomain.Factory.INSTANCE.createEditingDomain(rset);
		ed.addResourceSetListener(new PtnetLoLAResourceListener());
		return ed;

	}

	/**
	 * @see org.eclipse.emf.transaction.TransactionalEditingDomain.Factory#getEditingDomain(org.eclipse.emf.ecore.resource.ResourceSet)
	 */
	public TransactionalEditingDomain getEditingDomain(ResourceSet rset) {
		TransactionalEditingDomain ed =
			TransactionalEditingDomain.Factory.INSTANCE.getEditingDomain(rset);
		ed.addResourceSetListener(new PtnetLoLAResourceListener());
		return ed;
	}

}

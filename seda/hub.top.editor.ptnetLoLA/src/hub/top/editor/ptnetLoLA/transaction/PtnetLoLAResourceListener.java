package hub.top.editor.ptnetLoLA.transaction;

import java.util.Set;

import hub.top.editor.ptnetLoLA.transaction.adapt.PtnetTAdapter;
import hub.top.editor.ptnetLoLA.transaction.adapt.TransactionAdapter;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.transaction.ResourceSetChangeEvent;
import org.eclipse.emf.transaction.ResourceSetListenerImpl;
import org.eclipse.emf.transaction.RollbackException;

public class PtnetLoLAResourceListener extends ResourceSetListenerImpl {
	
	private Set<TransactionAdapter> transactionAdapters = null;

	public PtnetLoLAResourceListener () {
		super();
		init();
	}

	/**
	 * add transaction adapters to this listener
	 */
	private void init() {
		transactionAdapters = new java.util.HashSet<TransactionAdapter>();
		transactionAdapters.add(new PtnetTAdapter());
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.emf.transaction.ResourceSetListenerImpl#resourceSetChanged(org.eclipse.emf.transaction.ResourceSetChangeEvent)
	 */
	public void resourceSetChanged(ResourceSetChangeEvent event) {
		//System.out.println("A change has been made with "+event.getNotifications().size()+" notifications produced.");
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.emf.transaction.ResourceSetListenerImpl#transactionAboutToCommit(org.eclipse.emf.transaction.ResourceSetChangeEvent)
	 */
	public Command transactionAboutToCommit(ResourceSetChangeEvent event)
	throws RollbackException {

		CompoundCommand cc = new CompoundCommand();

		for (Notification n : event.getNotifications()) {
			if (n.getNotifier() instanceof EObject) {
				// consider notifications due to EObjects only
				EObject notifier = (EObject)n.getNotifier();

				// collect all commands from all handling adapters for this notifier
				for (TransactionAdapter adapter : getTransactionAdapters()) {
					if (adapter.handleEventForNotifier(notifier)) {
						Command cmd = adapter.transactionAboutToCommit(event, n);
						if (cmd != null)
							cc.append(cmd);
					}
				}
			}
		}

		// It is important to return null if we have nothing to 
		// contribute to this transaction.
		return cc.isEmpty() ? null : cc;
	}

	/**
	 * the transaction adapters registered to this listener provide
	 * further transaction commands for a running transaction 
	 * @return 
	 */
	protected Set<TransactionAdapter> getTransactionAdapters() {
		return transactionAdapters;
	}

}

/**
 * 
 */
package hub.top.editor.ptnetLoLA.transaction.adapt;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.transaction.ResourceSetChangeEvent;

/**
 * @author Dirk Fahland
 *
 */
public interface TransactionAdapter {

	/**
	 * filter method to choose whether this adapter is responsible
	 * for handling a notification due to the given notifier object
	 * @param notifier
	 * @return true iff the handler will look into the vent
	 */
	public boolean handleEventForNotifier(EObject notifier);
	
	/**
	 * The main handler method is called because of an event that notifies
	 * the change that is about to be committed. A transaction adapter can
	 * take this event and provide further commands to be executed in the
	 * transaction that triggered the event.
	 * 
	 * @param event that bundles a set of notifications due to a transaction
	 * and is sent by the transaction framework
	 * @param notification encapsulates one atomic change on the resource in
	 * the running transaction
	 * @return a (compound) command that shall appended to the current
	 * transaction, must be <code>null</code> if this adapter makes no
	 * contribution to the transaction 
	 */
	public Command transactionAboutToCommit(ResourceSetChangeEvent event,
			Notification notification);
}

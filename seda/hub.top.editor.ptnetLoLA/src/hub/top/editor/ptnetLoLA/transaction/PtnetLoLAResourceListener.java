/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - PetriNet Editor Framework
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is this file as it was released on August 30, 2008.
 * The Initial Developer of the Original Code is
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

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
		System.out.println("A change has been made with "+event.getNotifications().size()+" notifications produced.");
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

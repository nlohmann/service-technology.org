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

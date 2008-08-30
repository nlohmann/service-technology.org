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

import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.transaction.ResourceSetChangeEvent;

public class PtnetTAdapter implements TransactionAdapter {

	/**
	 * @see hub.top.editor.ptnetLoLA.transaction.adapt.TransactionAdapter#handleEventForNotifier(org.eclipse.emf.ecore.EObject)
	 */
	public boolean handleEventForNotifier(EObject notifier) {
		if (notifier instanceof PtNet)
			return true;
		return false;
	}

	/**
	 * @see hub.top.editor.ptnetLoLA.transaction.adapt.TransactionAdapter#transactionAboutToCommit(org.eclipse.emf.transaction.ResourceSetChangeEvent, org.eclipse.emf.common.notify.Notification)
	 */
	public Command transactionAboutToCommit(ResourceSetChangeEvent event,
			Notification notification)
	{
		
		if (PtnetLoLAPackage.PT_NET__ARCS == notification.getFeatureID(PtNet.class) &&
				Notification.REMOVE	== notification.getEventType())
    {
			return handle_Arc_remove(event, notification);
		}
		return null;
	}

	/**
	 * handle the removal of arcs from a net, during removal, the source and
	 * the target of the arc must be set to null to remove the arc from the
	 * incoming and outgoing arc lists of source and target node
	 * @param event
	 * @param notification
	 * @return
	 */
	private Command handle_Arc_remove (ResourceSetChangeEvent event,
			Notification notification)
	{
		// find the arc that is removed in the notification
    	Arc arc = (Arc) notification.getOldValue();
    	if (arc == null) arc = (Arc) notification.getNewValue();
    	if (arc == null) return null;
    	
    	// create a compound command of two set commands updating the
    	// arc's source and target
    	CompoundCommand cc = new CompoundCommand();
   		cc.append(new SetCommand(event.getEditingDomain(),
   					// arc.setSource(null);
    				arc, PtnetLoLAPackage.eINSTANCE.getArc_Source(), null));
   		cc.append(new SetCommand(event.getEditingDomain(),
   					// arc.setTarget(null);
    				arc, PtnetLoLAPackage.eINSTANCE.getArc_Target(), null));
    	return cc;
	}
}
